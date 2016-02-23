#include "SkyBox.h"
#include "ResourceManager.h"

using namespace Rendering;
using namespace Rendering::Sky;
using namespace Rendering::Camera;
using namespace Core;
using namespace Device;
using namespace Resource;
using namespace Math;
using namespace Rendering::Manager;
using namespace Rendering::Texture;

SkyBox::SkyBox() : SkyForm(), _material(nullptr)
{
	memset(&_prevWorldViewProjMat, 0, sizeof(Math::Matrix));
}

SkyBox::~SkyBox()
{
	Destroy();
}

void SkyBox::Initialize(const std::string& materialName, const std::string& cubeMapFilePath)
{
	_material = new SkyBoxMaterial(materialName);
	_material->Initialize();

	const ResourceManager* resMgr	= ResourceManager::SharedInstance();
	TextureManager* texMgr			= resMgr->GetTextureManager();

	Texture2D* cubeMap = texMgr->LoadTextureFromFile(cubeMapFilePath, false);
	_material->UpdateCubeMap(cubeMap);

	SkyForm::Initialize(_material);
}

void SkyBox::Initialize(SkyBoxMaterial* material)
{
	_material = material;
	SkyForm::Initialize(_material);
}

void SkyBox::Destroy()
{
	SAFE_DELETE(_material);
	SkyForm::Destroy();
}

void SkyBox::Render(const DirectX* dx, const CameraForm* camera, const Texture::RenderTexture*& renderTarget, const Texture::DepthBuffer*& opaqueDepthBuffer)
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

		_material->UpdateWVPMatrix(dx, worldViewProj);
	}

	SkyForm::_Render(dx, renderTarget, opaqueDepthBuffer);
}