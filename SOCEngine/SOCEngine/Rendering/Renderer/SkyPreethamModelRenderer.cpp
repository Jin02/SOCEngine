#include "SkyPreethamModelRenderer.h"
#include "BindIndexInfo.h"
#include "AutoBinder.hpp"
#include "ShaderFactory.hpp"
#include <algorithm>

#undef min

using namespace Core;
using namespace Device;
using namespace Math;
using namespace Rendering;
using namespace Rendering::Sky;
using namespace Rendering::Renderer;
using namespace Rendering::Shader;
using namespace Rendering::Material;
using namespace Rendering::Manager;
using namespace Rendering::Texture;
using namespace Rendering::RenderState;
using namespace Rendering::Buffer;
using namespace Rendering::Factory;
using namespace Rendering::Light;
using namespace Rendering::Camera;

SkyPreethamModelRenderer::SkyPreethamModelRenderer()
	: _resultMaterial("@SkyScatteringPreethamModel")
{
}

void SkyPreethamModelRenderer::Initialize(DirectX& dx, BufferManager& bufferMgr, ShaderManager& shaderMgr, MaterialManager& materialMgr, uint resolution)
{
	std::shared_ptr<VertexShader>	vs(nullptr);
	std::shared_ptr<GeometryShader>	gs(nullptr);
	std::shared_ptr<PixelShader>	ps(nullptr);

	std::vector<ShaderMacro> macro{dx.GetMSAAShaderMacro()};
	ShaderFactory::LoadShader(dx, shaderMgr,
								"AtmosphericScattering-PreethamModel",
								"VS", "PS", "GS", &macro, &vs, &ps, &gs);

	_vertexShader	= *vs;
	_geometryShader	= *gs;
	_pixelShader	= *ps;

	_geometry.Initialize(dx, bufferMgr);

	_lpParamCB.Initialize(dx);
	{
		const Vector3 forwards[6] = 
		{
			Vector3( 1.0f,  0.0f,  0.0f),	// +X
			Vector3(-1.0f,  0.0f,  0.0f),	// -X
			Vector3( 0.0f,  1.0f,  0.0f),	// +Y
			Vector3( 0.0f, -1.0f,  0.0f),	// -Y
			Vector3( 0.0f,  0.0f,  1.0f),	// +Z
			Vector3( 0.0f,  0.0f, -1.0f),	// -Z
		};
		const Vector3 ups[6] = 
		{
			Vector3( 0.0f,  1.0f,  0.0f),	// +X
			Vector3( 0.0f,  1.0f,  0.0f),	// -X
			Vector3( 0.0f,  0.0f, -1.0f),	// +Y
			Vector3( 0.0f,  0.0f,  1.0f),	// -Y
			Vector3( 0.0f,  1.0f,  0.0f),	// +Z
			Vector3( 0.0f,  1.0f,  0.0f),	// -Z
		};

		Matrix projMat	= Matrix::PerspectiveFovLH(1.0f, DEG_2_RAD(90.0f), 0.001f, 1.0f);

		LightProbeParam data;
		{
			for (uint i = 0; i < 6; ++i)
				data.viewProjMats[i] = Matrix::Transpose(Matrix::ComputeViewProjMatrix(Vector3::Zero(), forwards[i], ups[i], projMat));
		}

		_lpParamCB.UpdateSubResource(dx, data);
	}
	_ssParamCB.Initialize(dx);

	_renderTarget.Initialize(dx, resolution, DXGI_FORMAT_R16G16B16A16_FLOAT);

	_resultMaterial.Initialize(dx, shaderMgr);
	_resultMaterial.UpdateCubeMap(*_renderTarget.GetTexture2D());

	_materialID = materialMgr.Add(_resultMaterial).first;
}

void SkyPreethamModelRenderer::Destroy()
{
	_resultMaterial.Destroy();

	_ssParamData = SkyScatteringParam();

	_geometry.Destroy();
	_renderTarget.Destroy();

	_vertexShader.Destroy();
	_geometryShader.Destroy();
	_pixelShader.Destroy();

	_lpParamCB.Destroy();
	_ssParamCB.Destroy();

	_renderAble			= false;
	_dirty				= true;
	_directionalLightID	= ObjectID::Undefined();
	_materialID			= MaterialID::Undefined();
}

void SkyPreethamModelRenderer::CheckRenderAbleWithUpdateCB(DirectX& dx, const LightManager& lightMgr, const MainCamera& mainCam)
{
	uint lightIndex = lightMgr.GetIndexer<DirectionalLight>().Find(_directionalLightID.Literal());

	_renderAble = lightIndex != ObjectID::UndefinedLiteral();
	if (_renderAble == false)
		return;

	if (_dirty)
	{	
		_ssParamData.directionalLightIndex	= static_cast<ushort>(lightIndex);
		_ssParamCB.UpdateSubResource(dx, _ssParamData);
	}

	_dirty = false;
}

void SkyPreethamModelRenderer::Render(DirectX& dx, const MainCamera& mainCam, const LightManager& lightMgr)
{
	// Render Setting
	{
		dx.SetRasterizerState(RasterizerState::CCWDefault);
		dx.SetBlendState(BlendState::Opaque);	
		dx.SetDepthStencilState(DepthState::DisableDepthTestWrite, 0);
		dx.SetRenderTarget(_renderTarget);
		dx.SetViewport(Rect <float>(0.0f, 0.0f, _renderTarget.GetTexture2D()->GetSize().Cast<float>()));
		dx.SetPrimitiveTopology(PrimitiveTopology::TriangleList);
	}

	_geometry.GetVertexBuffer().IASetBuffer(dx);
	_geometry.GetIndexBuffer().IASetBuffer(dx);

	AutoBinderShaderToContext<VertexShader>		vs(dx, _vertexShader);
	AutoBinderShaderToContext<GeometryShader>	gs(dx, _geometryShader);
	AutoBinderShaderToContext<PixelShader>		ps(dx, _pixelShader);

	AutoBinderInputLayoutToContext				layout(dx, _vertexShader);

	AutoBinderCB<GeometryShader>	lpParam(dx,		ConstBufferBindIndex(0),		_lpParamCB);
	AutoBinderCB<GeometryShader>	camera(dx,		ConstBufferBindIndex::Camera,	mainCam.GetCameraCB());
	AutoBinderCB<PixelShader>		ssParamPS(dx,	ConstBufferBindIndex(3),		_ssParamCB);

	const auto& dlBuffers = lightMgr.GetBuffer<DirectionalLight>();
	AutoBinderSRV<GeometryShader>	dlDirXYGS(dx,	TextureBindIndex::DirectionalLightDirXY,				dlBuffers.GetTransformSRBuffer().GetShaderResourceView());
	AutoBinderSRV<GeometryShader>	dlOParamGS(dx,	TextureBindIndex::DirectionalLightOptionalParamIndex,	dlBuffers.GetOptionalParamIndexSRBuffer().GetShaderResourceView());

	AutoBinderSRV<PixelShader>		dlDirXYPS(dx,	TextureBindIndex::DirectionalLightDirXY,				dlBuffers.GetTransformSRBuffer().GetShaderResourceView());
	AutoBinderSRV<PixelShader>		dlOParamPS(dx,	TextureBindIndex::DirectionalLightOptionalParamIndex,	dlBuffers.GetOptionalParamIndexSRBuffer().GetShaderResourceView());

	dx.GetContext()->DrawIndexed(_geometry.GetIndexBuffer().GetIndexCount(), 0, 0);
	dx.ReSetRenderTargets(1);

	_renderTarget.GetTexture2D()->GenerateMips(dx);
}