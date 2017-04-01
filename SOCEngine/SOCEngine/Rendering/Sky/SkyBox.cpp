#include "SkyBox.h"
#include "BindIndexInfo.h"
#include <math.h>

using namespace Rendering;
using namespace Rendering::Sky;
using namespace Rendering::Shader;
using namespace Core;
using namespace Device;
using namespace Math;
using namespace Rendering::Manager;
using namespace Rendering::Texture;

void SkyBox::Initialize(
	DirectX& dx,
	ShaderManager& shaderMgr,
	Texture2DManager& tex2DMgr,
	BufferManager& bufferMgr,
	MaterialManager& materialMgr,
	const std::string& materialName, const std::string& cubeMapFilePath)
{
	auto cubeMap = tex2DMgr.LoadTextureFromFile(dx, cubeMapFilePath, false);

	SkyBoxMaterial material(materialName);
	material.Initialize(dx, shaderMgr);
	material.UpdateCubeMap(TextureBindIndex::SkyBoxCubeMap, (*cubeMap));

	materialMgr.Get().Add(materialName, material);

	_base.Initialize(dx, bufferMgr);
}