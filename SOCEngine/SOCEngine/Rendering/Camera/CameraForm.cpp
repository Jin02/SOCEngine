#include "CameraForm.h"
#include "Object.h"
#include "Director.h"

using namespace Math;
using namespace std;
using namespace Rendering::Buffer;
using namespace Rendering::Light;
using namespace Intersection;
using namespace Device;
using namespace Core;
using namespace Rendering::Camera;
using namespace Rendering::Manager;

CameraForm::CameraForm(Usage usage) 
	: Component(), _frustum(nullptr), _renderTarget(nullptr), _camMatConstBuffer(nullptr), _usage(usage)
{
}

CameraForm::~CameraForm(void)
{
	Destroy();
}

void CameraForm::Initialize(uint mainRTSampleCount)
{
	_fieldOfViewDegree	= 45.0f;
	_clippingNear		= 0.1f;
	_clippingFar		= 5000.0f;

	const Size<unsigned int>& backBufferSize = Director::GetInstance()->GetBackBufferSize();
	_aspect = (float)backBufferSize.w / (float)backBufferSize.h;

	_projectionType    = ProjectionType::Perspective;
	_clearColor = Color(0.5f, 0.5f, 1.0f, 1.0f);

	_frustum = new Frustum(0.0f);		

	_renderTarget = new Texture::RenderTexture;
	_renderTarget->Initialize(backBufferSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, mainRTSampleCount);

	//_clearFlag = ClearFlag::FlagSolidColor;

	_camMatConstBuffer = new ConstBuffer;
	_camMatConstBuffer->Initialize(sizeof(CamMatCBData));

	auto camMgr = Device::Director::GetInstance()->GetCurrentScene()->GetCameraManager();
	CameraForm* thisCam = this;
	camMgr->Add(_owner->GetName(), thisCam);
}

void CameraForm::Destroy()
{
	SAFE_DELETE(_frustum);
	SAFE_DELETE(_renderTarget);
	SAFE_DELETE(_camMatConstBuffer);
}

void CameraForm::CalcAspect()
{
	const Size<unsigned int>& backBufferSize =  Device::Director::GetInstance()->GetBackBufferSize();
	_aspect = (float)backBufferSize.w / (float)backBufferSize.h;
}

void CameraForm::GetPerspectiveMatrix(Math::Matrix &outMatrix, bool isInverted) const
{
	float fovRadian = Math::Common::Deg2Rad(_fieldOfViewDegree);

	float clippingNear	= _clippingNear;
	float clippingFar	= _clippingFar;

	if(isInverted)
	{
		clippingNear	= _clippingFar;
		clippingFar		= _clippingNear;
	}

	Matrix::PerspectiveFovLH(outMatrix, _aspect, fovRadian, clippingNear, clippingFar);
}

void CameraForm::GetOrthogonalMatrix(Math::Matrix &outMatrix, bool isInverted, const Math::Size<uint>* customWH) const
{
	const Size<uint>& wh = customWH ? (*customWH) : Device::Director::GetInstance()->GetBackBufferSize();

	float clippingNear	= _clippingNear;
	float clippingFar	= _clippingFar;

	if(isInverted)
	{
		clippingNear	= _clippingFar;
		clippingFar		= _clippingNear;
	}

	Matrix::OrthoLH(outMatrix, (float)(wh.w), (float)(wh.h), clippingNear, clippingFar);
}

void CameraForm::GetProjectionMatrix(Math::Matrix& outMatrix, bool isInverted) const
{
	if(_projectionType == ProjectionType::Perspective)
		GetPerspectiveMatrix(outMatrix, isInverted);
	else if(_projectionType == ProjectionType::Orthographic)
		GetOrthogonalMatrix(outMatrix, isInverted);
}

void CameraForm::GetViewMatrix(Math::Matrix &outMatrix, const Math::Matrix &worldMatrix)
{
	outMatrix = worldMatrix;

	Vector3 worldPos;
	worldPos.x = worldMatrix._41;
	worldPos.y = worldMatrix._42;
	worldPos.z = worldMatrix._43;

	Math::Vector3 right		= Math::Vector3(worldMatrix._11, worldMatrix._21, worldMatrix._31);
	Math::Vector3 up		= Math::Vector3(worldMatrix._12, worldMatrix._22, worldMatrix._32);
	Math::Vector3 forward	= Math::Vector3(worldMatrix._13, worldMatrix._23, worldMatrix._33);

	Vector3 p;
	p.x = -Vector3::Dot(right,		worldPos);
	p.y = -Vector3::Dot(up,			worldPos);
	p.z = -Vector3::Dot(forward,	worldPos);

	outMatrix._41 = p.x;
	outMatrix._42 = p.y;
	outMatrix._43 = p.z;
	outMatrix._44 = 1.0f;
}

void CameraForm::GetViewMatrix(Math::Matrix& outMatrix) const
{
	Matrix worldMat;
	_owner->GetTransform()->FetchWorldMatrix(worldMat);

	GetViewMatrix(outMatrix, worldMat);
}

void CameraForm::CullingWithUpdateCB(const Device::DirectX* dx, const std::vector<Core::Object*>& objects, const LightManager* lightManager)
{
	CamMatCBData cbData;
	{
		Matrix& viewMat = cbData.viewMat;
		GetViewMatrix(cbData.viewMat);

		Matrix projMat;
		GetProjectionMatrix(projMat, true);
		cbData.viewProjMat = viewMat * projMat;
	}

	bool updatedVP = memcmp(&_prevCamMatCBData, &cbData, sizeof(CamMatCBData)) != 0;
	if(updatedVP)
	{
		// Make Frustum
		{
			Matrix notInvProj;
			GetProjectionMatrix(notInvProj, false);
			_frustum->Make(cbData.viewMat * notInvProj);
		}

		_prevCamMatCBData = cbData;

		Matrix::Transpose(cbData.viewMat, cbData.viewMat);
		Matrix::Transpose(cbData.viewProjMat, cbData.viewProjMat);

		_camMatConstBuffer->UpdateSubResource(dx->GetContext(), &cbData);
	}

	for(auto iter = objects.begin(); iter != objects.end(); ++iter)
		(*iter)->Culling(_frustum);
}

void CameraForm::SortTransparentMeshRenderQueue(const RenderManager* renderMgr)
{
	const RenderManager::MeshList& transparentList = renderMgr->GetTransparentMeshes();
	if( transparentList.updateCounter != _transparentMeshQueue.updateCounter )
	{
		const auto& transparentMeshAddrSet = transparentList.meshes.GetVector();
		auto& thisCamMeshes = _transparentMeshQueue.meshes;

		thisCamMeshes.clear();
		for(auto addrSetIter = transparentMeshAddrSet.begin();
			addrSetIter != transparentMeshAddrSet.end(); ++addrSetIter)
		{
			for(auto iter = addrSetIter->begin(); iter != addrSetIter->end(); ++iter)
			{
				RenderManager::MeshList::meshkey addr = *iter;

				const Geometry::Mesh* mesh = reinterpret_cast<const Geometry::Mesh*>(addr);
				thisCamMeshes.push_back(mesh);
			}
		}

		_transparentMeshQueue.updateCounter = transparentList.updateCounter;
	}

	const Transform* transform = _owner->GetTransform();

	Math::Vector3 camPos;
	transform->FetchWorldPosition(camPos);

	auto SortingByDistance = [&](const Geometry::Mesh*& left, const Geometry::Mesh*& right) -> bool
	{
		float leftDistance = D3D11_FLOAT32_MAX;
		{
			Math::Vector3 leftPos;
			left->GetOwner()->GetTransform()->FetchWorldPosition(leftPos);
			leftDistance = Math::Vector3::Distance(leftPos, camPos);
		}

		float rightDistance = D3D11_FLOAT32_MAX;
		{
			Math::Vector3 rightPos;
			right->GetOwner()->GetTransform()->FetchWorldPosition(rightPos);
			rightDistance = Math::Vector3::Distance(rightPos, camPos);
		}

		return leftDistance < rightDistance;
	};

	std::sort(_transparentMeshQueue.meshes.begin(), _transparentMeshQueue.meshes.end(), SortingByDistance);
}

void CameraForm::_Clone(CameraForm* newCam) const
{
	memcpy(newCam, this, sizeof(CameraForm));

	newCam->_frustum		= new Frustum(0.0f);
	newCam->_renderTarget	= new Texture::RenderTexture;
	{
		const Size<unsigned int>& size = Director::GetInstance()->GetBackBufferSize();

		D3D11_TEXTURE2D_DESC desc;
		newCam->_renderTarget->GetTexture()->GetDesc(&desc);		
		newCam->_renderTarget->Initialize(size, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, desc.BindFlags, desc.SampleDesc.Count);
	}
}