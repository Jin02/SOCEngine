#include "SkyBoxMaterial.h"
#include "BindIndexInfo.h"
#include "EngineShaderFactory.hpp"

using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Rendering::Factory;
using namespace Rendering::Shader;
using namespace Rendering::Texture;
using namespace Math;
using namespace Device;
using namespace Rendering;

void SkyBoxMaterial::Initialize(DirectX& dx, ShaderManager& shaderMgr)
{
	EngineShaderFactory factory(&shaderMgr);

	ShaderGroup shaderGroup;
	factory.LoadShader(dx, "SkyBox", "VS", "PS", "", nullptr, &shaderGroup.vs, &shaderGroup.ps, nullptr);

	assert(shaderGroup.ableRender());
	
	_vs = (*shaderGroup.vs);
	_ps = (*shaderGroup.ps);
	
	_wvpCB.Initialize(dx);
}

void SkyBoxMaterial::UpdateConstBuffer(DirectX& dx, const Matrix& transposedWVPMat)
{
	_wvpCB.UpdateSubResource(dx, transposedWVPMat);
}

void SkyBoxMaterial::UpdateCubeMap(TextureBindIndex bind, const Texture2D& tex)
{
	BindTextured2D bindData;
	{
		bindData.resource = tex;
		bindData.bindIndex = static_cast<uint>(TextureBindIndex::SkyBoxCubeMap);
		bindData.usePS = true;
	}

	GetTextureBook().Add("CubeMap", bindData);
}
