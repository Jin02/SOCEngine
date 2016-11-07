#include "CameraForm.h"
#include "Object.h"
#include "Director.h"

using namespace Math;
using namespace std;
using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Rendering::Light;
using namespace Intersection;
using namespace Device;
using namespace Core;
using namespace Rendering::Camera;
using namespace Rendering::Manager;

CameraForm::CameraForm(Usage usage) 
	: Component(), _frustum(nullptr), _renderTarget(nullptr), _camMatConstBuffer(nullptr), _usage(usage),
		_camCBChangeState(TransformCB::ChangeState::No)
{
	Matrix::Identity(_prevViewProjMat);
}

CameraForm::~CameraForm(void)
{
	Destroy();
}

void CameraForm::Initialize(const Math::Rect<float>& renderRect)
{
	_fieldOfViewDegree	= 45.0f;
	_clippingNear		= 0.1f;
	_clippingFar		= 1000.0f;

	const Size<unsigned int>& backBufferSize = Director::SharedInstance()->GetBackBufferSize();
	_aspect = (float)backBufferSize.w / (float)backBufferSize.h;

	_projectionType    = ProjectionType::Perspective;
	_clearColor = Color(0.5f, 0.5f, 1.0f, 1.0f);

	_frustum = new Frustum(0.0f);		

	_renderTarget = new Texture::RenderTexture;
	_renderTarget->Initialize(backBufferSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 0);

	//_clearFlag = ClearFlag::FlagSolidColor;
	_renderRect = renderRect;

	_camMatConstBuffer = new ConstBuffer;
	_camMatConstBuffer->Initialize(sizeof(CameraCBData));

	Device::Director::SharedInstance()->GetCurrentScene()->GetCameraManager()->Add(this);
}

void CameraForm::Destroy()
{
	SAFE_DELETE(_frustum);
	SAFE_DELETE(_renderTarget);
	SAFE_DELETE(_camMatConstBuffer);
}

void CameraForm::CalcAspect()
{
	const Size<unsigned int>& backBufferSize =  Device::Director::SharedInstance()->GetBackBufferSize();
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
	const Size<uint>& wh = customWH ? (*customWH) : Device::Director::SharedInstance()->GetBackBufferSize();

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

void CameraForm::GetViewportMatrix(Math::Matrix& outMat, const Math::Rect<float>& rect)
{
	outMat._11 = rect.size.w /  2.0f;
	outMat._12 = 0.0f;
	outMat._13 = 0.0f;
	outMat._14 = 0.0f;

	outMat._21 = 0.0f;
	outMat._22 = -rect.size.h / 2.0f;
	outMat._23 = 0.0f;
	outMat._24 = 0.0f;

	outMat._31 = 0.0f;
	outMat._32 = 0.0f;
	outMat._33 = 1.0f;
	outMat._34 = 0.0f;

	outMat._41 = rect.x + rect.size.w / 2.0f;
	outMat._42 = rect.y + rect.size.h / 2.0f;
	outMat._43 = 0.0f;
	outMat._44 = 1.0f;
}

void CameraForm::GetInvViewportMatrix(Math::Matrix& outMat, const Math::Rect<float>& rect)
{
	Math::Matrix viewportMat;
	GetViewportMatrix(viewportMat, rect);

	Math::Matrix::Inverse(outMat, viewportMat);
}

bool CameraForm::_CullingWithUpdateCB(const Device::DirectX* dx,
				      const std::vector<Core::Object*>& objects,
				      const LightManager* lightManager,
				      CameraCBData* outResultCamCBData, Math::Matrix* outProjMat)
{
	Matrix worldMat;
	_owner->GetTransform()->FetchWorldMatrix(worldMat);

	CameraCBData cbData;
	{
		Matrix& viewMat = cbData.viewMat;
		GetViewMatrix(cbData.viewMat, worldMat);

		Matrix projMat;
		GetProjectionMatrix(projMat, true);

		if(outProjMat)
			(*outProjMat) = projMat;
	
		cbData.viewProjMat	= viewMat * projMat;
		cbData.worldPos		= Vector3(worldMat._41, worldMat._42, worldMat._43);	
		cbData.prevViewProjMat	= _prevViewProjMat;
		cbData.packedCamNearFar = (Common::FloatToHalf(_clippingNear) << 16) | Common::FloatToHalf(_clippingFar);
	}
	if(outResultCamCBData)
		(*outResultCamCBData) = cbData;

	bool isChanged = (cbData.viewProjMat != _prevViewProjMat);
	if(isChanged)
	{
		// Make Frustum
		{
			Matrix notInvProj;
			GetProjectionMatrix(notInvProj, false);
			_frustum->Make(cbData.viewMat * notInvProj);
		}
		
		_camCBChangeState = TransformCB::ChangeState::HasChanged;
	}

	
	
	bool isUpdate = (_camCBChangeState != TransformCB::ChangeState::No);
	
	if(isUpdate)
	{		
		_prevViewProjMat = cbData.viewProjMat;

		Matrix::Transpose(cbData.viewMat, cbData.viewMat);
		Matrix::Transpose(cbData.viewProjMat, cbData.viewProjMat);
		Matrix::Transpose(cbData.prevViewProjMat, cbData.prevViewProjMat);

		_camMatConstBuffer->UpdateSubResource(dx->GetContext(), &cbData);
		
		_camCBChangeState = (static_cast<uint>(_camCBChangeState) + 1) % static_cast<uint>(TransformCB::ChangeState::MAX);
	}

	for(auto iter = objects.begin(); iter != objects.end(); ++iter)
		(*iter)->Culling(_frustum);	
	
	return isUpdate;
}

void CameraForm::SortTransparentMeshRenderQueue(RenderQueue& inoutTranparentMeshQ, const Transform* ownerTF, const RenderManager* renderMgr)
{
	const RenderManager::MeshList& transparentList = renderMgr->GetTransparentMeshes();
	if( transparentList.updateCounter != inoutTranparentMeshQ.updateCounter )
	{
		const auto& transparentMeshAddrSet = transparentList.meshes.GetVector();
		auto& thisCamMeshes = inoutTranparentMeshQ.meshes;

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

		inoutTranparentMeshQ.updateCounter = transparentList.updateCounter;
	}

	Math::Vector3 camPos;
	ownerTF->FetchWorldPosition(camPos);

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

	std::sort(inoutTranparentMeshQ.meshes.begin(), inoutTranparentMeshQ.meshes.end(), SortingByDistance);
}

void CameraForm::_Clone(CameraForm* newCam) const
{
	memcpy(newCam, this, sizeof(CameraForm));

	newCam->_frustum		= new Frustum(0.0f);
	newCam->_renderTarget	= new Texture::RenderTexture;
	{
		const Size<unsigned int>& size = Director::SharedInstance()->GetBackBufferSize();

		D3D11_TEXTURE2D_DESC desc;
		newCam->_renderTarget->GetTexture()->GetDesc(&desc);		
		newCam->_renderTarget->Initialize(size, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, desc.BindFlags, desc.SampleDesc.Count);
	}
}
