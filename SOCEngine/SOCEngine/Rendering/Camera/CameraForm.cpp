#include "CameraForm.h"
#include "Object.h"
#include "Director.h"
#include "TransformPipelineParam.h"

using namespace Math;
using namespace std;
using namespace Rendering::Light;
using namespace Device;
using namespace Core;
using namespace Rendering::Camera;
using namespace Rendering::Manager;

CameraForm::CameraForm() 
	: Component(),	_frustum(nullptr), _renderTarget(nullptr), _isInvertedDepthWriting(false)
{
}

CameraForm::~CameraForm(void)
{

}

void CameraForm::OnInitialize()
{
	_FOV			= 60.0f;
	_clippingNear	= 0.1f;
	_clippingFar	= 1000.0f;

	const Size<unsigned int>& backBufferSize = Director::GetInstance()->GetBackBufferSize();
	_aspect = (float)backBufferSize.w / (float)backBufferSize.h;

	_projectionType    = ProjectionType::Perspective;
	_clearColor = Color(0.5f, 0.5f, 1.0f, 1.0f);

	_frustum = new Frustum(0.0f);		

	_renderTarget = new Texture::RenderTexture;
	_renderTarget->Initialize(backBufferSize, DXGI_FORMAT_R8G8B8A8_UNORM);

	_camConstBuffer = new Buffer::ConstBuffer;
	if(_camConstBuffer->Initialize(sizeof(ConstBufferParam)) == false)
		ASSERT_MSG("Error, cam->constbuffer->Initialize");

	//_clearFlag = ClearFlag::FlagSolidColor;
}

void CameraForm::OnDestroy()
{
	SAFE_DELETE(_frustum);
	SAFE_DELETE(_renderTarget);
}

void CameraForm::CalcAspect()
{
	const Size<unsigned int>& backBufferSize =  Device::Director::GetInstance()->GetBackBufferSize();
	_aspect = (float)backBufferSize.w / (float)backBufferSize.h;
}

void CameraForm::GetProjectionMatrix(Math::Matrix& outMatrix) const
{
	if(_projectionType == ProjectionType::Perspective)
	{
		float radian = _FOV * PI / 180.0f;

		float clippingNear = _clippingNear;
		float clippingFar = _clippingFar;

		if(_isInvertedDepthWriting)
			std::swap(clippingNear, clippingFar);

		Matrix::PerspectiveFovLH(outMatrix, _aspect, radian, clippingNear, clippingFar);
	}
	else if(_projectionType == ProjectionType::Orthographic)
	{
		const Size<unsigned int>& backBufferSize = Device::Director::GetInstance()->GetBackBufferSize();
		Matrix::OrthoLH(outMatrix, (float)(backBufferSize.w), (float)(backBufferSize.h), _clippingNear, _clippingFar);
	}
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

void CameraForm::RenderPreviewWithUpdateTransformCB(const std::vector<Core::Object*>& objects)
{
	TransformPipelineParam tfParam;
	GetProjectionMatrix(tfParam.projMat);
	GetViewMatrix(tfParam.viewMat);

	_viewProjMatrixInPrevRenderState = tfParam.viewMat * tfParam.projMat;
	_frustum->Make(_viewProjMatrixInPrevRenderState);

	ConstBufferParam camCB;
	{
		Matrix worldMat;
		_owner->GetTransform()->FetchWorldMatrix(worldMat);

		camCB.worldPos		= Vector4(worldMat._41, worldMat._42, worldMat._43, 1.0f);
		camCB.clippingNear	= _clippingNear;
		camCB.clippingFar	= _clippingFar;
		camCB.screenSize	= Device::Director::GetInstance()->GetBackBufferSize().Cast<float>();
	}
	
	if( memcmp(&_prevConstBufferData, &camCB, sizeof(ConstBufferParam)) != 0 )
	{
		ID3D11DeviceContext* context = Device::Director::GetInstance()->GetDirectX()->GetContext();
		_camConstBuffer->UpdateSubResource(context, &camCB);

		_prevConstBufferData = camCB;
	}

	for(auto iter = objects.begin(); iter != objects.end(); ++iter)
	{
		(*iter)->Culling(_frustum);
		(*iter)->RenderPreviewWithUpdateTransformCB(tfParam);
	}
}

void CameraForm::SortTransparentMeshRenderQueue()
{
	RenderManager* renderMgr = Director::GetInstance()->GetCurrentScene()->GetRenderManager();
	
	const RenderManager::MeshList transparentList = renderMgr->GetTransparentMeshes();
	if( transparentList.updateCounter > _transparentMeshQueue.updateCounter )
	{
		const auto& meshes = transparentList.meshes.GetVector();

		_transparentMeshQueue.meshes.clear();
		for(auto iter = meshes.begin(); iter != meshes.end(); ++iter)
			_transparentMeshQueue.meshes.push_back((*iter));

		_transparentMeshQueue.updateCounter = transparentList.updateCounter;
	}

	const Transform* transform = _owner->GetTransform();

	Math::Vector3 camPos;
	transform->FetchWorldPosition(camPos);

	auto SortingByDistance = [&](const Mesh::Mesh*& left, const Mesh::Mesh*& right) -> bool
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
	(*newCam) = (*this);
	newCam->_frustum		= new Frustum(0.0f);
	newCam->_renderTarget	= new Texture::RenderTexture;
	{
		const Size<unsigned int>& size = Director::GetInstance()->GetBackBufferSize();
		newCam->_renderTarget->Initialize(size, DXGI_FORMAT_R8G8B8A8_UNORM);
	}

	newCam->_camConstBuffer	= new Buffer::ConstBuffer;
	if(_camConstBuffer->Initialize(sizeof(ConstBufferParam)) == false)
		ASSERT_MSG("Error, cant create const buffer in _Clone");
}