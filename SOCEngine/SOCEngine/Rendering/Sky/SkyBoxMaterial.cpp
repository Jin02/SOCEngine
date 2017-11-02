#include "SkyBoxMaterial.h"
#include "BindIndexInfo.h"
#include "EngineShaderFactory.hpp"

using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Rendering::Factory;
using namespace Rendering::Shader;
using namespace Rendering::Texture;
using namespace Rendering::Material;
using namespace Math;
using namespace Device;
using namespace Rendering;

void SkyBoxMaterial::UpdateCubeMap(const Texture2D& tex)
{
	MaterialForm::BindTextured2D bindData;
	{
		bindData.resource	= tex;
		bindData.bindIndex	= static_cast<uint>(TextureBindIndex::SkyBoxCubeMap);
		bindData.usePS		= true;
	}

	_cubeMap = GetTextureBook().Add("CubeMap", bindData).resource;
}