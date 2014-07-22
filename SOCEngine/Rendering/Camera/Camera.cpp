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
		FOV = 60;
		clippingNear = 0.01f;
		clippingFar = 1000.0f;

		//이게 정상인가? 그런갑지;
		Size<int> windowSize = Director::GetInstance()->GetWindowSize();
		aspect = (float)windowSize.w / (float)windowSize.h;

		camType    = Type::Perspective;
		clearColor = Color(0.5f, 0.5f, 1.0f,1.0f);

		frustum = new Frustum(0.0f);		

		clearFlag = ClearFlag::FlagSolidColor;
	}

	void Camera::Destroy()
	{
		//Utility::SAFE_DELETE(rtShader);
		//Utility::SAFE_DELETE(renderTarget);
		SAFE_DELETE(frustum);
	}

	void Camera::Clear(DX *dx)
	{
	}

	void Camera::CalcAspect()
	{
		Size<int> windowSize =  Device::Director::GetInstance()->GetWindowSize();
		aspect = (float)windowSize.w / (float)windowSize.h;
	}

	void Camera::ProjectionMatrix(Math::Matrix& outMatrix)
	{
		if(camType == Type::Perspective)
		{
			float radian = FOV * PI / 180.0f;
			Matrix::PerspectiveFovLH(outMatrix, aspect, radian, clippingNear, clippingFar);
		}
		else if(camType == Type::Orthographic)
		{
			Size<int> windowSize = Device::Director::GetInstance()->GetWindowSize();
			Matrix::OrthoLH(outMatrix, (float)(windowSize.w), (float)(windowSize.h), clippingNear, clippingFar);
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
		frustum->Make(viewProjMat);

		//추후 작업.

		vector<LightForm*> lights;
		if( sceneLights->Intersects(lights, frustum) )
		{
			//월드 상의 빛에서 절두체에 겹치는거 모두 찾음.

			for(auto iter = objectBegin; iter != objectEnd; ++iter)
			{
				(*iter)->Culling(frustum);
				(*iter)->Render(lights, viewMat, projMat, viewProjMat);
			}
		}
	}

	void Camera::Render(std::vector<Object*>::iterator &objectBegin, std::vector<Object*>::iterator &objectEnd,	Light::LightManager* sceneLights)
	{
	}

	void Camera::SceneRender(Camera *cam, std::vector<Object*>::iterator& objectBegin,
			std::vector<Object*>::iterator& objectEnd, Light::LightManager* sceneLights)
	{
	}

}