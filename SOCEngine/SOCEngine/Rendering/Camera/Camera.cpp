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
	_clippingFar = 100.0f;

	Size<unsigned int> windowSize = Director::GetInstance()->GetWindowSize();
	_aspect = (float)windowSize.w / (float)windowSize.h;

	_camType    = Type::Perspective;
	_clearColor = Color(0.5f, 0.5f, 1.0f);

	_frustum = new Frustum(0.0f);		

	_renderTarget = new Texture::RenderTexture;
	_renderTarget->Create(windowSize);

	_depthBuffer =  new Texture::RenderTexture;
	_depthBuffer->Create(windowSize);

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

void Camera::RenderObjects(const Device::DirectX* dx, const Rendering::Manager::RenderManager* renderMgr)
{
	enum MeshRenderOption
	{
		DepthWriteWithTest,
		AlphaTest,
		Common,
	};
	auto MeshRender = [&](ID3D11DeviceContext* context, Material* customMaterial, Manager::RenderManager::MeshType type, MeshRenderOption option)
	{
		Material* currentUseMaterial = nullptr;
		if(customMaterial && 
			(option == MeshRenderOption::DepthWriteWithTest || 
			 option == MeshRenderOption::AlphaTest) )
			ASSERT("Invalid Arg");

		auto RenderIter = [&](Material* material, Mesh::Mesh* mesh)
		{
			if(customMaterial)
				material = customMaterial;
			else if(option == MeshRenderOption::DepthWriteWithTest)
				material = mesh->GetMeshRenderer()->GetDepthWriteMaterial();
			else if(option == MeshRenderOption::AlphaTest)
				material =  mesh->GetMeshRenderer()->GetAlphaTestMaterial();

			if(currentUseMaterial != material)
			{
				currentUseMaterial = material;	
				currentUseMaterial->UpdateShader(context);
			}
			
			mesh->Render(customMaterial);
		};

		renderMgr->Iterate(RenderIter, type);
	};

	//graphics part
	{
		ID3D11DeviceContext* context = dx->GetContext();

		//Test
		{
			ID3D11RenderTargetView* rtv = dx->GetBackBuffer();
			context->OMSetRenderTargets(1, &rtv, dx->GetDepthBuffer()->GetDepthStencilView());
			context->ClearRenderTargetView(dx->GetBackBuffer(), _clearColor.color);			
			dx->GetDepthBuffer()->Clear(1.0f, 0);
			MeshRender(context, nullptr, Manager::RenderManager::MeshType::nonAlpha, MeshRenderOption::AlphaTest);
		}

		//depth clear
		{

		}

		//off alpha blending
		{
			float blendFactor[1] = { 0.0f };
			//context->OMSetBlendState(dx->GetOpaqueBlendState(), blendFactor, 0xffffffff);
		}

		//Render
		{
			//Early-Z
			{
		/*		ID3D11RenderTargetView* rtv = _depthBuffer->GetRenderTargetView();
				context->OMSetRenderTargets(1, &rtv, dx->GetDepthBuffer()->GetDepthStencilView());
				context->OMSetDepthStencilState(dx->GetDepthLessEqualState(), 0);
				MeshRender(context, nullptr, RenderManager::MeshType::nonAlpha, MeshRenderOption::Depth);

				context->RSSetState(dx->GetDisableCullingRasterizerState());
				MeshRender(context, nullptr, RenderManager::MeshType::hasAlpha);
				context->RSSetState(nullptr);*/
			}

			//Light Culling
			{

			}

			//Forward Rendering
			{
				//MeshRender(context, nullptr, Manager::RenderManager::MeshType::nonAlpha);
			}
		}


		IDXGISwapChain* swapChain = dx->GetSwapChain();
		swapChain->Present(0, 0);
	}
}
