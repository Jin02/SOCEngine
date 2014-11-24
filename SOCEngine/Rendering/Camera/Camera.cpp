#include "Camera.h"
#include "Object.h"
#include "Director.h"
#include "TransformPipelineParam.h"

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
		_FOV = 60.0f;
		_clippingNear = 0.01f;
		_clippingFar = 1000.0f;

		Size<int> windowSize = Director::GetInstance()->GetWindowSize();
		_aspect = (float)windowSize.w / (float)windowSize.h;

		_camType    = Type::Perspective;
		_clearColor = Color(0.5f, 0.5f, 1.0f);

		_frustum = new Frustum(0.0f);		

		_clearFlag = ClearFlag::FlagSolidColor;
	}

	void Camera::Destroy()
	{
		//Utility::SAFE_DELETE(rtShader);
		//Utility::SAFE_DELETE(renderTarget);
		SAFE_DELETE(_frustum);
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
		Transform* ownerTransform = _owner->GetTransform();
		ownerTransform->WorldMatrix(outMatrix);

		Vector3 worldPos;
		worldPos.x = outMatrix._14;
		worldPos.y = outMatrix._24;
		worldPos.z = outMatrix._34;

		Vector3 p;
		p.x = -Vector3::Dot(ownerTransform->GetRight(), worldPos);
		p.y = -Vector3::Dot(ownerTransform->GetUp(), worldPos);
		p.z = -Vector3::Dot(ownerTransform->GetForward(), worldPos);

		outMatrix._14 = p.x;
		outMatrix._24 = p.y;
		outMatrix._34 = p.z;
		outMatrix._44 = 1.0f;
	}

	void Camera::Clear(ID3D11DeviceContext* context)
	{

	}

	void Camera::RenderObjects(const Device::DirectX* dx, const Structure::Vector<Core::Object>& objects)
	{
		ID3D11DeviceContext* context = dx->GetContext();
		context->ClearRenderTargetView(dx->GetRenderTarget(), _clearColor.color);
		context->ClearDepthStencilView(dx->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

		TransformPipelineParam tfParam;
		ProjectionMatrix(tfParam.projMat);
		ViewMatrix(tfParam.viewMat);

		Matrix viewProj = tfParam.viewMat * tfParam.projMat;
		_frustum->Make(viewProj);

		vector<LightForm*> lights;
		//if( sceneLights->Intersects(lights, _frustum) )
		{
			auto& dataInobjects = objects.GetVector();
			for(auto iter = dataInobjects.begin(); iter != dataInobjects.end(); ++iter)
			{				
				GET_CONTENT_FROM_ITERATOR(iter)->Culling(_frustum);
				GET_CONTENT_FROM_ITERATOR(iter)->Render(lights, tfParam);
			}
		}

		IDXGISwapChain* swapChain = dx->GetSwapChain();
		swapChain->Present(0, 0);
	}

	void Camera::Render(const Structure::Vector<Core::Object>& objects)
	{
		const Device::DirectX* dx = Device::Director::GetInstance()->GetDirectX();
		RenderObjects(dx, objects);
	}

}