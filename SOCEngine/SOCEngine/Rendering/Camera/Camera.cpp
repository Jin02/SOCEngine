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
		ASSERT("Error, cam->constbuffer->Create");

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

void Camera::RenderObjects(const Device::DirectX* dx, const Rendering::Manager::RenderManager* renderMgr)
{
	enum MeshRenderOption
	{
		DepthWriteWithTest,
		AlphaTest,
		Common,
	};
	auto MeshRender = [&](ID3D11DeviceContext* context, BasicMaterial* customMaterial, Manager::RenderManager::MeshType type, MeshRenderOption option)
	{
		BasicMaterial* currentUseMaterial = nullptr;
		if(customMaterial && 
			(option == MeshRenderOption::DepthWriteWithTest || 
			 option == MeshRenderOption::AlphaTest) )
			ASSERT("Invalid Arg");

		auto RenderIter = [&](BasicMaterial* material, Mesh::Mesh* mesh)
		{
			if(customMaterial)
				material = customMaterial;
			else if(option == MeshRenderOption::DepthWriteWithTest)
			{
				material = mesh->GetMeshRenderer()->GetDepthWriteMaterial();
				customMaterial = material;
			}
			else if(option == MeshRenderOption::AlphaTest)
			{
				material =  mesh->GetMeshRenderer()->GetAlphaTestMaterial();
				customMaterial = material;
			}

			if(currentUseMaterial != material)
			{
				currentUseMaterial = material;	
				currentUseMaterial->UpdateShader(context);
			}
			
			mesh->Render(customMaterial, _constBuffer);
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
			MeshRender(context, nullptr, Manager::RenderManager::MeshType::nonAlpha, MeshRenderOption::Common);
		}

		//clear backbuffer, depthbuffer
		{
			//context->ClearRenderTargetView(dx->GetBackBuffer(), _clearColor.color);
			//dx->GetDepthBuffer()->Clear(0.0f, 0);
			//_depthBuffer->Clear(Rendering::Color::white(), dx);
		}

		//off alpha blending
		{
			//float blendFactor[1] = { 0.0f };
			//context->OMSetBlendState(dx->GetOpaqueBlendState(), blendFactor, 0xffffffff);
		}

		//Render
		{
		/*	ID3D11RenderTargetView*		nullRTV		= nullptr;
			ID3D11DepthStencilView*		nullDSV		= nullptr;
			ID3D11ShaderResourceView*	nullSRV		= nullptr;
			ID3D11UnorderedAccessView*	nullUAV		= nullptr;
			ID3D11SamplerState*			nullSampler = nullptr;*/

			//Early-Z
			{				
				//ID3D11RenderTargetView* rtv = _depthBuffer->GetRenderTargetView();
				//context->OMSetRenderTargets(1, &rtv, dx->GetDepthBuffer()->GetDepthStencilView());
				//context->OMSetDepthStencilState(dx->GetDepthLessEqualState(), 0);
				//MeshRender(context, nullptr, RenderManager::MeshType::nonAlpha, MeshRenderOption::DepthWriteWithTest);

				//rtv = nullptr;
				//context->OMSetRenderTargets(1, &rtv, dx->GetDepthBuffer()->GetDepthStencilView());
				//context->RSSetState(dx->GetDisableCullingRasterizerState());
				//MeshRender(context, nullptr, RenderManager::MeshType::hasAlpha, MeshRenderOption::AlphaTest);
				//context->RSSetState(nullptr);
			}

			//Light Culling
			{
				Director* director = Director::GetInstance();
				LightCulling* lightCullingCS = director->GetCurrentScene()->GetCameraManager()->GetLightCullingCS();
				
				LightCulling::CullingConstBuffer cb;
				{
					// worldView
					{
						_owner->GetTransform()->WorldMatrix(cb.worldViewMat);

						Matrix view;
						ViewMatrix(view);

						cb.worldViewMat *= view;
					}

					// proj
					{
						ProjectionMatrix(cb.invProjMat);
						Matrix::Inverse(cb.invProjMat, cb.invProjMat);
					}

					// screenSize
					{
						cb.screenSize.x = static_cast<float>(director->GetWindowSize().w);
						cb.screenSize.y = static_cast<float>(director->GetWindowSize().h);
					}

					cb.clippingFar = _clippingFar;				
				}

				//lightCullingCS->UpdateInputBuffer(cb, 0, 0);
				lightCullingCS->Dispatch(context, _depthBuffer);
			}

			//Forward Rendering
			{
			}
		}


		IDXGISwapChain* swapChain = dx->GetSwapChain();
		swapChain->Present(0, 0);
	}
}
