#include "SkyBox.h"
#include "BindIndexInfo.h"
#include <math.h>

using namespace Core;
using namespace Device;
using namespace Math;
using namespace Rendering;
using namespace Rendering::Sky;
using namespace Rendering::Shader;
using namespace Rendering::Material;
using namespace Rendering::Manager;
using namespace Rendering::Texture;
using namespace Rendering::RenderState;

void SkyBox::Initialize(DirectX& dx)
{
	_geometry.Initialize(dx);
	_transformCB.Initialize(dx);
}

void SkyBox::UpdateCB(DirectX& dx, const Vector3& worldPos, const Matrix& viewProjMat, float far)
{
	Matrix matrix;
	{
		matrix._11 = 
		matrix._22 =
		matrix._33 = far;
		
		matrix._41 = worldPos.x;
		matrix._42 = worldPos.y;
		matrix._43 = worldPos.z;
		matrix._44 = 1.0f;
	}

	matrix = Matrix::Transpose(matrix * viewProjMat);
	_transformCB.UpdateSubResource(dx, matrix);
}

void SkyBox::Render(DirectX& dx, RenderTexture& target, const DepthMap& targetDepthMap, const Material::SkyBoxMaterial& material)
{
	// Render Setting
	{
		dx.SetRasterizerState(RasterizerState::CWDisableCulling);
		dx.SetBlendState(BlendState::Opaque);	
		dx.SetDepthStencilState(DepthState::GreaterEqualAndDisableDepthWrite, 0);
		dx.SetRenderTarget(target, const_cast<DepthMap&>(targetDepthMap));	// depth state Âü°í.
		dx.SetViewport(Rect<float>(0.0f, 0.0f, target.GetSize().Cast<float>()));
		dx.SetPrimitiveTopology(PrimitiveTopology::TriangleList);
	}

	_geometry.GetVertexBuffer().IASetBuffer(dx);
	_geometry.GetIndexBuffer().IASetBuffer(dx);

	const auto& vs = material.GetVertexShader();
	vs.BindShaderToContext(dx);
	vs.BindInputLayoutToContext(dx);
	material.GetPixelShader().BindShaderToContext(dx);

	VertexShader::BindConstBuffer(dx, ConstBufferBindIndex::SkyBoxWVP, _transformCB);
	PixelShader::BindShaderResourceView(dx, TextureBindIndex::SkyBoxCubeMap, material.GetCubeMap().GetShaderResourceView());
	PixelShader::BindSamplerState(dx, SamplerStateBindIndex::DefaultSamplerState, SamplerState::Linear);

	dx.GetContext()->DrawIndexed(_geometry.GetIndexBuffer().GetIndexCount(), 0, 0);

	VertexShader::UnBindConstBuffer(dx, ConstBufferBindIndex::SkyBoxWVP);
	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex::SkyBoxCubeMap);
	PixelShader::UnBindSamplerState(dx, SamplerStateBindIndex::DefaultSamplerState);

	vs.UnBindShaderToContext(dx);
	vs.UnBindInputLayoutToContext(dx);
	material.GetPixelShader().UnBindShaderToContext(dx);
}
