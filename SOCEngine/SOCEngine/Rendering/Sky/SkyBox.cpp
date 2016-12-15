#include "SkyBox.h"
#include "ResourceManager.h"
#include "BindIndexInfo.h"

using namespace Rendering;
using namespace Rendering::Sky;
using namespace Rendering::Camera;
using namespace Rendering::Shader;
using namespace Core;
using namespace Device;
using namespace Resource;
using namespace Math;
using namespace Rendering::Manager;
using namespace Rendering::Texture;

SkyBox::SkyBox() : SkyForm(Type::Box), _skyBoxMaterialForReflectionProbe(nullptr), _skyBoxMaterial(nullptr), _cubeMap(nullptr)
{
	memset(&_prevWorldViewProjMat, 0, sizeof(Math::Matrix));
}

SkyBox::~SkyBox()
{
	Destroy();
}

void SkyBox::Initialize(const std::string& materialName, const std::string& cubeMapFilePath)
{
	_skyBoxMaterial = new SkyBoxMaterial(materialName);
	_skyBoxMaterial->Initialize();

	_skyBoxMaterialForReflectionProbe = new SkyBoxCubeMaterial(materialName + " Cube");
	_skyBoxMaterialForReflectionProbe->Initialize();

	const ResourceManager* resMgr	= ResourceManager::SharedInstance();
	TextureManager* texMgr			= resMgr->GetTextureManager();

	Texture2D* cubeMap = texMgr->LoadTextureFromFile(cubeMapFilePath, false);
	_skyBoxMaterial->UpdateCubeMap(cubeMap);
	_skyBoxMaterialForReflectionProbe->UpdateCubeMap(cubeMap);
	_cubeMap = cubeMap;

	auto log2 = [](float f) -> float
	{
		return log(f) / log(2.0f);
	};

	_maxMipCount = log2(cubeMap->GetSize().w);

	SkyForm::Initialize();
}

void SkyBox::Destroy()
{
	SAFE_DELETE(_skyBoxMaterial);
	SAFE_DELETE(_skyBoxMaterialForReflectionProbe);

	SkyForm::Destroy();
}

void SkyBox::Render(const DirectX* dx, const CameraForm* camera,
					const Texture::RenderTexture* out, const Texture::DepthBuffer* depthBuffer,
					const Manager::LightManager* lightMgr)
{
	Matrix worldViewProj;
	{
		Matrix world;

		Transform* transform = camera->GetOwner()->GetTransform();
		transform->FetchWorldMatrix(world);

		Matrix view;
		camera->GetViewMatrix(view, world);

		Vector3 camWorldPos(world._41, world._42, world._43);

		Matrix::Identity(world);
		world._41 = camWorldPos.x;
		world._42 = camWorldPos.y;
		world._43 = camWorldPos.z;

		world._11 = camera->GetFar();
		world._22 = camera->GetFar();
		world._33 = camera->GetFar();

		Matrix proj;
		camera->GetProjectionMatrix(proj, true);

		worldViewProj = world * view * proj;
	}

	bool isChanged = memcmp(&worldViewProj, &_prevWorldViewProjMat, sizeof(Matrix)) != 0;
	if (isChanged)
	{
		_prevWorldViewProjMat = worldViewProj;
		Matrix::Transpose(worldViewProj, worldViewProj);
		
		_skyBoxMaterial->UpdateWVPMatrix(dx, worldViewProj);
	}

	SkyForm::_Render(dx, _skyBoxMaterial, out->GetRenderTargetView(), depthBuffer->GetDepthStencilView());
}

void SkyBox::Render(const DirectX* dx, const ReflectionProbe* probe,
					const TextureCube* out, const DepthBufferCube* depthBuffer,
					const Manager::LightManager* lightMgr)
{
	ID3D11DeviceContext* context = dx->GetContext();

	GeometryShader::BindConstBuffer(context, ConstBufferBindIndex::ReflectionProbe_Info, probe->GetInfoConstBuffer());
	PixelShader::BindConstBuffer(context, ConstBufferBindIndex::ReflectionProbe_Info, probe->GetInfoConstBuffer());

	SkyForm::_Render(dx, _skyBoxMaterialForReflectionProbe, out->GetRenderTargetView(), depthBuffer->GetDepthStencilView());

	GeometryShader::BindConstBuffer(context, ConstBufferBindIndex::ReflectionProbe_Info, nullptr);
	PixelShader::BindConstBuffer(context, ConstBufferBindIndex::ReflectionProbe_Info, nullptr);
}
