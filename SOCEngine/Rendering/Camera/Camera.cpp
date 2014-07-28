#include "Camera.h"
#include "Object.h"
#include "Director.h"

using namespace Math;
using namespace std;
using namespace Rendering::Light;
using namespace Device;
using namespace Core;

namespace Rendering
{
	Camera::Camera() : Component()
	{

	}

	Camera::~Camera(void)
	{
	
	}

	void Camera::Initialize()
	{
		_FOV = 60;
		_clippingNear = 0.01f;
		_clippingFar = 1000.0f;

		Size<int> windowSize = Director::GetInstance()->GetWindowSize();
		_aspect = (float)windowSize.w / (float)windowSize.h;

		_camType    = Type::Perspective;
		_clearColor = Color(0.5f, 0.5f, 1.0f,1.0f);

		_frustum = new Frustum(0.0f);		

		_clearFlag = ClearFlag::FlagSolidColor;
	}

	void Camera::Destroy()
	{
		//Utility::SAFE_DELETE(rtShader);
		//Utility::SAFE_DELETE(renderTarget);
		SAFE_DELETE(_frustum);
	}

	void Camera::Clear(DirectX *dx)
	{
	}

	void Camera::CalcAspect()
	{
		Size<int> windowSize =  Device::Director::GetInstance()->GetWindowSize();
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
			Size<int> windowSize = Device::Director::GetInstance()->GetWindowSize();
			Matrix::OrthoLH(outMatrix, (float)(windowSize.w), (float)(windowSize.h), _clippingNear, _clippingFar);
		}
	}

	void Camera::ViewMatrix(Math::Matrix& outMatrix)
	{
		//_owner->GetTransform()->WorldMatrix(outMatrix);
		//ownerTransform->GetWorldMatrix(outMatrix);

		//Vector3 worldPos = ownerTransform->GetWorldPosition();

		//Vector3 p = Vector3(
		//	-SOCVec3Dot(&ownerTransform->GetRight(), &worldPos),
		//	-SOCVec3Dot(&ownerTransform->GetUp(), &worldPos),
		//	-SOCVec3Dot(&ownerTransform->GetForward(), &worldPos));

		//outMatrix->_41 = p.x;
		//outMatrix->_42 = p.y;
		//outMatrix->_43 = p.z;
		//outMatrix->_44 = 1.0f;
	}

	void Camera::RenderObjects(std::vector<Object*>::iterator &objectBegin,	std::vector<Object*>::iterator &objectEnd, Light::LightManager* sceneLights)
	{
		Math::Matrix projMat, viewMat, viewProjMat;
		ProjectionMatrix(projMat);
		ViewMatrix(viewMat);

		viewProjMat = viewMat * projMat;

		//Clear();
		_frustum->Make(viewProjMat);

		//추후 작업.

		vector<LightForm*> lights;
		if( sceneLights->Intersects(lights, _frustum) )
		{
			//월드 상의 빛에서 절두체에 겹치는거 모두 찾음.

			for(auto iter = objectBegin; iter != objectEnd; ++iter)
			{
				(*iter)->Culling(_frustum);
				(*iter)->Render(lights, viewMat, projMat, viewProjMat);
			}
		}
	}

	void Camera::Render(const Structure::Vector<Core::Object>& objects, Light::LightManager* sceneLights)
	{

	}

}