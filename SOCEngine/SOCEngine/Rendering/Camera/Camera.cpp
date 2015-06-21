#include "Camera.h"
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

Camera::Camera() 
	: Component(),	_frustum(nullptr), _renderTarget(nullptr), _isInvertedDepthWriting(false)
{
	_renderType = RenderType::Unknown;
}

Camera::~Camera(void)
{

}

void Camera::OnInitialize()
{
	_FOV			= 60.0f;
	_clippingNear	= 0.1f;
	_clippingFar	= 1000.0f;

	Size<unsigned int> windowSize = Director::GetInstance()->GetWindowSize();
	_aspect = (float)windowSize.w / (float)windowSize.h;

	_projectionType    = ProjectionType::Perspective;
	_clearColor = Color(0.5f, 0.5f, 1.0f, 1.0f);

	_frustum = new Frustum(0.0f);		

	_renderTarget = new Texture::RenderTexture;
	_renderTarget->Initialize(windowSize);

	_constBuffer = new Buffer::ConstBuffer;
	if(_constBuffer->Initialize(sizeof(CameraConstBuffer)) == false)
		ASSERT_MSG("Error, cam->constbuffer->Initialize");

	//_clearFlag = ClearFlag::FlagSolidColor;
}

void Camera::OnDestroy()
{
	SAFE_DELETE(_frustum);
	SAFE_DELETE(_renderTarget);
}

void Camera::CalcAspect()
{
	Size<unsigned int> windowSize =  Device::Director::GetInstance()->GetWindowSize();
	_aspect = (float)windowSize.w / (float)windowSize.h;
}

void Camera::ProjectionMatrix(Math::Matrix& outMatrix)
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
		Size<unsigned int> windowSize = Device::Director::GetInstance()->GetWindowSize();
		Matrix::OrthoLH(outMatrix, (float)(windowSize.w), (float)(windowSize.h), _clippingNear, _clippingFar);
	}
}

void Camera::ViewMatrix(Math::Matrix &outMatrix, const Math::Matrix &worldMatrix)
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

void Camera::ViewMatrix(Math::Matrix& outMatrix)
{
	Matrix worldMat;
	_owner->GetTransform()->FetchWorldMatrix(worldMat);

	ViewMatrix(outMatrix, worldMat);
}

void Camera::RenderPreviewWithUpdateTransformCB(const Structure::Vector<std::string, Core::Object>& objects)
{
	TransformPipelineParam tfParam;
	ProjectionMatrix(tfParam.projMat);
	ViewMatrix(tfParam.viewMat);

	Matrix viewProj = tfParam.viewMat * tfParam.projMat;
	_frustum->Make(viewProj);

	CameraConstBuffer camCB;
	{
		const Math::Matrix& viewMat = tfParam.viewMat;
		camCB.viewPos = Vector4(viewMat._41, viewMat._42, viewMat._43, 1.0f);
		camCB.clippingNear = _clippingNear;
		camCB.clippingFar = _clippingFar;
		const auto& size = Device::Director::GetInstance()->GetWindowSize();
		camCB.screenSize.w = static_cast<float>(size.w);
		camCB.screenSize.h = static_cast<float>(size.h);
	}

	ID3D11DeviceContext* context = Device::Director::GetInstance()->GetDirectX()->GetContext();
	_constBuffer->Update(context, &camCB);

	auto& dataInobjects = objects.GetVector();
	for(auto iter = dataInobjects.begin(); iter != dataInobjects.end(); ++iter)
	{
		GET_CONTENT_FROM_ITERATOR(iter)->Culling(_frustum);
		GET_CONTENT_FROM_ITERATOR(iter)->RenderPreviewWithUpdateTransformCB(tfParam);
	}
}

void Camera::SortTransparentMeshRenderQueue()
{
	RenderManager* renderMgr = Director::GetInstance()->GetCurrentScene()->GetRenderManager();
	
	const RenderManager::MeshList transparentList = renderMgr->GetTransparentMeshList();
	if( transparentList.updateCounter > _transparentMeshQueue.updateCounter )
	{
		const auto& map = transparentList.meshes.GetMap();

		_transparentMeshQueue.meshes.clear();
		for(auto iter = map.begin(); iter != map.end(); ++iter)
			_transparentMeshQueue.meshes.push_back(GET_CONTENT_FROM_ITERATOR(iter));

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