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

Camera::Camera() : Component(),
	_frustum(nullptr), _renderTarget(nullptr), _depthBuffer(nullptr)
{

}

Camera::~Camera(void)
{

}

void Camera::Initialize()
{
	_FOV = 60.0f;
	_clippingNear = 0.01f;
	_clippingFar = 50.0f;

	Size<unsigned int> windowSize = Director::GetInstance()->GetWindowSize();
	_aspect = (float)windowSize.w / (float)windowSize.h;

	_camType    = Type::Perspective;
	_clearColor = Color(0.5f, 0.5f, 1.0f, 1.0f);

	_frustum = new Frustum(0.0f);		

	_renderTarget = new Texture::RenderTexture;
	_renderTarget->Create(windowSize);

	_depthBuffer =  new Texture::RenderTexture;
	_depthBuffer->Create(windowSize);

	_constBuffer = new Buffer::ConstBuffer;
	if(_constBuffer->Create(sizeof(CameraConstBuffer)) == false)
		ASSERT_MSG("Error, cam->constbuffer->Create");

	//_clearFlag = ClearFlag::FlagSolidColor;
}

void Camera::Destroy()
{
	SAFE_DELETE(_frustum);
	SAFE_DELETE(_renderTarget);
	SAFE_DELETE(_depthBuffer);
}

void Camera::CalcAspect()
{
	Size<unsigned int> windowSize =  Device::Director::GetInstance()->GetWindowSize();
	_aspect = (float)windowSize.w / (float)windowSize.h;
}

void Camera::ProjectionMatrix(Math::Matrix& outMatrix)
{
	if(_camType == Type::Perspective)
	{
		float radian = _FOV * PI / 180.0f;
		Matrix::PerspectiveFovLH(outMatrix, _aspect, radian, _clippingNear, _clippingFar);
	}
	else if(_camType == Type::Orthographic)
	{
		Size<unsigned int> windowSize = Device::Director::GetInstance()->GetWindowSize();
		Matrix::OrthoLH(outMatrix, (float)(windowSize.w), (float)(windowSize.h), _clippingNear, _clippingFar);
	}
}

void Camera::ViewMatrix(Math::Matrix& outMatrix)
{
	Transform* ownerTransform = _owner->GetTransform();
	ownerTransform->WorldMatrix(outMatrix);

	Vector3 worldPos;
	worldPos.x = outMatrix._41;
	worldPos.y = outMatrix._42;
	worldPos.z = outMatrix._43;

	Vector3 p;
	p.x = -Vector3::Dot(ownerTransform->GetRight(), worldPos);
	p.y = -Vector3::Dot(ownerTransform->GetUp(), worldPos);
	p.z = -Vector3::Dot(ownerTransform->GetForward(), worldPos);

	outMatrix._41 = p.x;
	outMatrix._42 = p.y;
	outMatrix._43 = p.z;
	outMatrix._44 = 1.0f;
}

void Camera::UpdateTransformCBAndCheckRender(const Structure::Vector<std::string, Core::Object>& objects)
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
		GET_CONTENT_FROM_ITERATOR(iter)->UpdateTransformCBAndCheckRender(tfParam);
	}
}