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

Camera::Camera() : Component(),
	_frustum(nullptr), _depthBuffer(nullptr), _renderTarget(nullptr)
{

}

Camera::~Camera(void)
{

}

void Camera::Initialize()
{
	_FOV = 60.0f;
	_clippingNear = 0.01f;
	_clippingFar = 100.0f;

	Size<unsigned int> windowSize = Director::GetInstance()->GetWindowSize();
	_aspect = (float)windowSize.w / (float)windowSize.h;

	_camType    = Type::Perspective;
	_clearColor = Color(0.5f, 0.5f, 1.0f);

	_frustum = new Frustum(0.0f);		

	_depthBuffer = new Texture::DepthBuffer;
	_depthBuffer->Create(windowSize);

	_renderTarget = new Texture::RenderTexture;
	_renderTarget->Create(windowSize);

	//_clearFlag = ClearFlag::FlagSolidColor;
}

void Camera::Destroy()
{
	SAFE_DELETE(_frustum);
	SAFE_DELETE(_depthBuffer);
	SAFE_DELETE(_renderTarget);
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

void Camera::UpdateTransformAndCheckRender(const Structure::Vector<std::string, Core::Object>& objects)
{
	TransformPipelineParam tfParam;
	ProjectionMatrix(tfParam.projMat);
	ViewMatrix(tfParam.viewMat);

	Matrix viewProj = tfParam.viewMat * tfParam.projMat;
	_frustum->Make(viewProj);

	auto& dataInobjects = objects.GetVector();
	for(auto iter = dataInobjects.begin(); iter != dataInobjects.end(); ++iter)
	{				
		GET_CONTENT_FROM_ITERATOR(iter)->Culling(_frustum);
		GET_CONTENT_FROM_ITERATOR(iter)->UpdateTransformAndCheckRender(tfParam);
	}
}

void Camera::RenderObjects(const Device::DirectX* dx, const Rendering::Manager::MeshManager* meshMgr)
{
	auto NonAlphaMeshRender = [&]()
	{
		auto NonAlphaMeshIter = [](Mesh::Mesh* mesh)
		{
			mesh->Render();
			return;
		};
		meshMgr->Iterate(NonAlphaMeshIter, Manager::MeshManager::MeshType::nonAlpha);
	};
	auto AlphaMeshRender = [&]()
	{
		auto AlphaMeshIter = [](Mesh::Mesh* mesh)
		{
			mesh->Render();
			return;
		};

		meshMgr->Iterate(AlphaMeshIter, Manager::MeshManager::MeshType::hasAlpha);
	};

	//_renderTarget->SetRenderTarget(_depthBuffer, dx);

	//graphics part
	{
		ID3D11DeviceContext* context = dx->GetContext();

		//depth clear
		{
			//context->ClearRenderTargetView(dx->GetRenderTargetView(), _clearColor.color);
			_renderTarget->Clear(_clearColor, dx);
			_depthBuffer->Clear(1.0f, 0, dx);
		}

		//off alpha blending
		{
			float blendFactor[1] = { 0.0f };
			context->OMSetBlendState(dx->GetOpaqueBlendState(), blendFactor, 0xffffffff);
		}

		//Render
		{
			//Early-Z
			{
				//ID3D11RenderTargetView* rtv = dx->GetRenderTargetView();
				//context->OMSetRenderTargets(1, &rtv, _depthBuffer->GetDepthStencilView());
				_depthBuffer->SetRenderTarget(dx);
				context->OMSetDepthStencilState(dx->GetDepthLessEqualState(), 0);
				NonAlphaMeshRender();

				context->RSSetState(dx->GetDisableCullingRasterizerState());
				AlphaMeshRender();
				context->RSSetState(nullptr);
			}

			//Light Culling
			{

			}

			//Forward Rendering
			{

			}
		}

//		NonAlphaMeshRender();

		IDXGISwapChain* swapChain = dx->GetSwapChain();
		swapChain->Present(0, 0);
	}
}
