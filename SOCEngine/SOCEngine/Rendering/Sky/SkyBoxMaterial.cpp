#include "SkyBoxMaterial.h"
#include "BindIndexInfo.h"
#include "ShaderFactory.hpp"

using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Rendering::Factory;
using namespace Rendering::Shader;
using namespace Rendering::Texture;
using namespace Rendering::Material;
using namespace Math;
using namespace Device;
using namespace Rendering;

void SkyBoxMaterial::Initialize(DirectX& dx, ShaderManager& shaderMgr)
{
	Factory::ShaderFactory loader(&shaderMgr);

	std::shared_ptr<VertexShader>	vs(nullptr);
	std::shared_ptr<PixelShader>	ps(nullptr);

	auto result = loader.LoadShader(dx, "SkyBox", "VS", "PS", "", nullptr, &vs, &ps, nullptr);
	assert(result.loadVS & result.loadPS); // Error, cant load shader

	_vertexShader = *vs;
	_pixelShader = *ps;
}

void SkyBoxMaterial::UpdateCubeMap(const Texture2D& tex)
{
	MaterialForm::BindTextured2D bindData;
	{
		bindData.resource	= tex;
		bindData.bindIndex	= static_cast<uint>(TextureBindIndex::SkyBoxCubeMap);
		bindData.usePS		= true;
	}

	_cubeMap		= GetTextures().Add("CubeMap", bindData).resource;
	_maxMipLevel	= log(float(_cubeMap.GetSize().w)) / log(2.0f);
}