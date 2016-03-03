#include "SkyBoxMaterial.h"
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

SkyBoxMaterial::SkyBoxMaterial(const std::string& name)
	: Material(name, Type::Sky), _wvpConstBuffer(nullptr)
{
}

SkyBoxMaterial::~SkyBoxMaterial()
{
	SAFE_DELETE(_wvpConstBuffer);
}

void SkyBoxMaterial::Initialize()
{
	const ResourceManager* resMgr = ResourceManager::SharedInstance();

	ShaderManager* shaderMgr = resMgr->GetShaderManager();
	{
		EngineFactory factory(shaderMgr);

		ShaderGroup& shaderGroup = _customShaders.shaderGroup;
		factory.LoadShader("SkyBox", "VS", "PS", "", nullptr, &shaderGroup.vs, &shaderGroup.ps, &shaderGroup.gs);

		if (shaderGroup.ableRender() == false)
			ASSERT_MSG("Error, Invalid Shader");
	}

	_wvpConstBuffer = new ConstBuffer;
	_wvpConstBuffer->Initialize(sizeof(Matrix));

	SetConstBufferUseBindIndex(uint(ConstBufferBindIndex::SkyBoxWVP), _wvpConstBuffer, ShaderForm::Usage(true, false, false, false));
}

void SkyBoxMaterial::Destroy()
{

}

void SkyBoxMaterial::UpdateCubeMap(const Texture2D* tex)
{
	SetTextureUseBindIndex(0, tex, ShaderForm::Usage(false, false, false, true));
	_hasAlpha = tex->GetHasAlpha();
}

void SkyBoxMaterial::UpdateWVPMatrix(const Device::DirectX* dx, const Math::Matrix& transposedWVPMat)
{
	_wvpConstBuffer->UpdateSubResource(dx->GetContext(), &transposedWVPMat);
}