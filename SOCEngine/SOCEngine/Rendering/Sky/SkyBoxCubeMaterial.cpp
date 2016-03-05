#include "SkyBoxCubeMaterial.h"
#include "BindIndexInfo.h"

#include "ResourceManager.h"
#include "EngineShaderFactory.hpp"

using namespace Rendering::Sky;
using namespace Rendering::Geometry;
using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Rendering::Factory;
using namespace Rendering::Shader;
using namespace Rendering::Camera;
using namespace Rendering::Texture;
using namespace Resource;
using namespace Math;

SkyBoxCubeMaterial::SkyBoxCubeMaterial(const std::string& name)
	: Material(name, Type::Sky)
{
}

SkyBoxCubeMaterial::~SkyBoxCubeMaterial()
{
}

void SkyBoxCubeMaterial::Initialize()
{
	const ResourceManager* resMgr = ResourceManager::SharedInstance();

	ShaderManager* shaderMgr = resMgr->GetShaderManager();
	{
		EngineFactory factory(shaderMgr);

		ShaderGroup& shaderGroup = _customShaders.shaderGroup;
		factory.LoadShader("SkyBoxForReflectionProbe", "VS", "PS", "GS", nullptr, &shaderGroup.vs, &shaderGroup.ps, &shaderGroup.gs);

		if (shaderGroup.ableRender() == false)
			ASSERT_MSG("Error, Invalid Shader");
	}
}

void SkyBoxCubeMaterial::Destroy()
{
}

void SkyBoxCubeMaterial::UpdateCubeMap(const Texture2D* tex)
{
	SetTextureUseBindIndex(0, tex, ShaderForm::Usage(false, false, false, true));
	_hasAlpha = tex->GetHasAlpha();
}