#include "SkyBoxCubeMaterial.h"
#include "EngineShaderFactory.hpp"

using namespace Rendering::Sky;
using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Rendering::Factory;
using namespace Rendering::Shader;
using namespace Rendering::Texture;
using namespace Math;

void SkyBoxCubeMaterial::Initialize(Device::DirectX& dx, ShaderManager& shaderMgr)
{
	EngineShaderFactory factory(&shaderMgr);

	ShaderGroup shaderGroup;
	factory.LoadShader(dx, "SkyBoxForReflectionProbe", "VS", "PS", "GS", nullptr, &shaderGroup.vs, &shaderGroup.ps, &shaderGroup.gs);

	assert(shaderGroup.ableRender());

	SetShaders(shaderGroup);

}