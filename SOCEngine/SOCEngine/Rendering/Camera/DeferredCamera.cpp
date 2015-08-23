#include "DeferredCamera.h"
#include "Director.h"
#include "EngineShaderFactory.hpp"
#include "ResourceManager.h"

using namespace Rendering::Camera;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Device;
using namespace Math;
using namespace Rendering::Factory;
using namespace Rendering::Light;
using namespace Rendering::Manager;
using namespace Rendering::Buffer;

DeferredCamera::DeferredCamera() : CameraForm(),
	_blendedDepthBuffer(nullptr), _albedo_metallic(nullptr),
	_specular_fresnel0(nullptr), _normal_roughness(nullptr),
	_useTransparent(false), _opaqueDepthBuffer(nullptr),
	_tbrParamConstBuffer(nullptr)
{
}

DeferredCamera::~DeferredCamera()
{
}

void DeferredCamera::OnInitialize()
{
	CameraForm::OnInitialize();

	Size<unsigned int> windowSize = Director::GetInstance()->GetWindowSize();

	_albedo_metallic = new Texture::RenderTexture;
	ASSERT_COND_MSG( _albedo_metallic->Initialize(windowSize, DXGI_FORMAT_R8G8B8A8_UNORM), "GBuffer Error : cant create albedo_opacity render texture" );

	_specular_fresnel0 = new Texture::RenderTexture;
	ASSERT_COND_MSG( _specular_fresnel0->Initialize(windowSize, DXGI_FORMAT_R8G8B8A8_UNORM), "GBuffer Error : cant create _specular_fresnel0 render texture" );

	_normal_roughness = new Texture::RenderTexture;
	ASSERT_COND_MSG( _normal_roughness->Initialize(windowSize, DXGI_FORMAT_R8G8B8A8_UNORM), "GBuffer Error : cant create _normal_roughness render texture" );

	_opaqueDepthBuffer = new Texture::DepthBuffer;
	_opaqueDepthBuffer->Initialize(windowSize);

	_deferredShadingWithLightCulling = new DeferredShadingWithLightCulling;
	_deferredShadingWithLightCulling->Initialize(_opaqueDepthBuffer, _albedo_metallic, _specular_fresnel0, _normal_roughness, windowSize);

	EnableRenderTransparentMesh(true);

	_tbrParamConstBuffer = new ConstBuffer;
	_tbrParamConstBuffer->Initialize(sizeof(LightCulling::TBRParam));

	auto camMgr = Device::Director::GetInstance()->GetCurrentScene()->GetCameraManager();
	CameraForm* thisCam = this;
	camMgr->Add(_owner->GetName(), thisCam);
}

void DeferredCamera::OnDestroy()
{
	SAFE_DELETE(_albedo_metallic);
	SAFE_DELETE(_specular_fresnel0);
	SAFE_DELETE(_normal_roughness);
	SAFE_DELETE(_blendedDepthBuffer);

	SAFE_DELETE(_tbrParamConstBuffer);

	CameraForm::OnDestroy();
}

void DeferredCamera::Render(float dt, Device::DirectX* dx)
{
	RenderManager* renderMgr = Director::GetInstance()->GetCurrentScene()->GetRenderManager();
	ID3D11DeviceContext* context = dx->GetContext();

	//Clear
	{
		Color allZeroColor(0.f, 0.f, 0.f, 0.f);

		_albedo_metallic->Clear(context, allZeroColor);
		_specular_fresnel0->Clear(context, allZeroColor);
		_normal_roughness->Clear(context, allZeroColor);
	}

	_opaqueDepthBuffer->Clear(context, 0.0f, 0); //inverted depth

	if(_useTransparent)
	{
		_blendedDepthBuffer->Clear(context, 0.0f, 0);
		SortTransparentMeshRenderQueue();
	}

	// off alpha blending
	float blendFactor[4] = {0, };
	context->OMSetBlendState(dx->GetBlendStateOpaque(), blendFactor, 0xffffffff);

	struct MeshInForwardRendering
	{
		const Mesh::Mesh*	mesh;
		const Material*		material;
	};
	
	// TBFR = Tile Based Forward Rendering.
	// Used to render transparent meshes
	//std::vector<MeshInForwardRendering> tbfrQueue;
	//auto HasCustomShaderWithAddMeshToTBFRQueue = [&](const Material* material, const Mesh::Mesh* mesh)
	//{
	//	if( material->GetCustomShader().IsAllEmpty() == false )
	//	{
	//		MeshInForwardRendering mfr;
	//		{
	//			mfr.material = material;
	//			mfr.mesh = mesh;
	//		}

	//		tbfrQueue.push_back(mfr);
	//		return true;
	//	}

	//	return false;
	//};
	
	enum class RenderType { AlphaMesh, Opaque, Transparency, Transparency_DepthOnly };
	auto RenderMesh = [&](const std::vector<const Mesh::Mesh*>& meshes, RenderType renderType)
	{
		for(auto meshIter = meshes.begin(); meshIter != meshes.end(); ++meshIter)
		{
			const Mesh::Mesh* mesh = (*meshIter);
			Mesh::MeshFilter* filter = mesh->GetMeshFilter();
			filter->IASetBuffer(dx);

			ShaderGroup shaders;
			if(renderType == RenderType::Opaque || renderType == RenderType::AlphaMesh)
				renderMgr->FindGBufferShader(shaders, filter->GetBufferFlag(), renderType == RenderType::AlphaMesh);
			else if(renderType == RenderType::Transparency || renderType == RenderType::Transparency_DepthOnly)
				renderMgr->FindTransparencyShader(shaders, filter->GetBufferFlag(), renderType == RenderType::Transparency_DepthOnly);

			Mesh::MeshRenderer* renderer	= mesh->GetMeshRenderer();
			const auto& materials			= renderer->GetMaterials();
			for(auto iter = materials.begin(); iter != materials.end(); ++iter)
			{					
				Material* material = (*iter);	
				if(material->GetCustomShader().IsAllEmpty() == false)
				{
					DEBUG_LOG("Warning, Current version doesn't support custom shader(and normal forward rendering)");
					continue;
				}

				const auto& tex	= material->GetTextures();

				std::vector<BaseShader::BufferType> constBuffers = material->GetConstBuffers();
				{
					constBuffers.push_back(BaseShader::BufferType(0, _camConstBuffer));
					constBuffers.push_back(BaseShader::BufferType(1, mesh->GetConstBuffer()));
				}

				shaders.vs->SetShaderToContext(context);
				shaders.vs->SetInputLayoutToContext(context);
				shaders.vs->UpdateResources(context, &constBuffers, nullptr);

				if(renderType != RenderType::Transparency_DepthOnly)
				{
					shaders.ps->SetShaderToContext(context);
					shaders.ps->UpdateResources(context, &constBuffers, &tex);
				}

				context->DrawIndexed(filter->GetIndexCount(), 0, 0);
			}
		}
	};


	//GBuffer
	{
		ID3D11RenderTargetView* renderTargetViews[] = {
			_albedo_metallic->GetRenderTargetView(),
			_normal_roughness->GetRenderTargetView(),
			_specular_fresnel0->GetRenderTargetView(),
		};

		context->OMSetRenderTargets(NumOfRenderTargets, renderTargetViews, _opaqueDepthBuffer->GetDepthStencilView());
		context->OMSetDepthStencilState(dx->GetDepthStateGreater(), 0);

		ID3D11SamplerState* samplerState = dx->GetSamplerStateAnisotropic();
		context->PSSetSamplers(0, 1, &samplerState);

		//Opaque Mesh
		{
			const std::vector<const Mesh::Mesh*>& meshes = renderMgr->GetOpaqueMeshes().meshes.GetVector();
			RenderMesh(meshes, RenderType::Opaque);
		}

		//Alpha Test Mesh
		{
			bool useMSAA = dx->GetMSAADesc().Count > 1;

			if(useMSAA) //on alpha blending
				context->OMSetBlendState(dx->GetBlendStateAlphaToCoverage(), blendFactor, 0xffffffff);

			context->RSSetState( dx->GetRasterizerStateDisableCulling() );
		
			const std::vector<const Mesh::Mesh*>& meshes = renderMgr->GetAlphaTestMeshes().meshes.GetVector();
			RenderMesh(meshes, RenderType::AlphaMesh);

			context->RSSetState(nullptr);

			if(useMSAA) //off alpha blending
				context->OMSetBlendState(dx->GetBlendStateOpaque(), blendFactor, 0xffffffff);
		}

		// Transparent Mesh
		// Write blended depth buffer
		if(_useTransparent)
		{
			ID3D11RenderTargetView* nullRTV = nullptr;
			context->OMSetRenderTargets(1, &nullRTV, _blendedDepthBuffer->GetDepthStencilView());
			
			const std::vector<const Mesh::Mesh*>& meshes = renderMgr->GetTransparentMeshes().meshes.GetVector();
			RenderMesh(meshes, RenderType::Transparency_DepthOnly);
		}
	}

	LightManager* lightManager = Director::GetInstance()->GetCurrentScene()->GetLightManager();

	// Light Culling and Deferred Shading
	{
		ID3D11RenderTargetView* nullRTVs[] = {nullptr, nullptr, nullptr};
		ID3D11DepthStencilView* nullDSV = nullptr;
		context->OMSetRenderTargets(NumOfRenderTargets, nullRTVs, nullDSV);
		context->OMSetDepthStencilState(dx->GetDepthStateDisableDepthTest(), 0);
		
		LightCulling::TBRChangeableParam changeableParam;
		{
			changeableParam.lightNum = lightManager->GetPackedLightCount();
			GetViewMatrix(changeableParam.viewMat);

			//inverse 처리는, const buffer 업데이트 직전에 함
			GetProjectionMatrix(changeableParam.invProjMat);
		}

		if( memcmp(&_prevParamData, &changeableParam, sizeof(LightCulling::TBRChangeableParam)) != 0 )
		{
			LightCulling::TBRParam param;
			//viewMat, lightNum
			memcpy(&param, &changeableParam, sizeof(LightCulling::TBRChangeableParam));

			const Matrix& viewMat = param.viewMat;
			const Matrix& projMat = param.invProjMat; //아직 inverse 처리 안함

			Matrix viewportMat;
			dx->GetViewportMatrix(viewportMat);

			param.invViewProjViewport = viewMat * projMat * viewportMat;
			Matrix::Inverse(param.invViewProjViewport, param.invViewProjViewport); //invViewProjViewport
			Matrix::Inverse(param.invProjMat, param.invProjMat); //invProj

			param.screenSize = Director::GetInstance()->GetWindowSize().Cast<float>();
			param.maxNumOfperLightInTile = LightCulling::LightMaxNumInTile;

			_tbrParamConstBuffer->UpdateSubResource(context, &param);

			_prevParamData = param;
		}

		_deferredShadingWithLightCulling->Dispatch(dx, _tbrParamConstBuffer);

		if(_useTransparent)
			_blendedMeshLightCulling->Dispatch(dx, _tbrParamConstBuffer);
	}

	// TBFR
	{
		/*
		struct MeshInForwardRendering
		{
			const Mesh::Mesh*	mesh;
			const Material*		material;
		};
		*/
		std::vector<MeshInForwardRendering> tbfrQueue;
		for(auto iter = tbfrQueue.begin(); iter != tbfrQueue.end(); ++iter)
		{

		}
	}

	// Transparency
	if(_useTransparent)
	{
		ID3D11RenderTargetView* thisCamRTV = _renderTarget->GetRenderTargetView();	
		context->OMSetRenderTargets(1, &thisCamRTV, _opaqueDepthBuffer->GetDepthStencilView());
		context->OMSetDepthStencilState(dx->GetDepthStateGreaterAndDisableDepthWrite(), 0x00);
		
		context->PSSetShaderResources(0, 1, lightManager->GetPointLightTransformBufferSR()->GetShaderResourceView());
		context->PSSetShaderResources(1, 1, lightManager->GetPointLightColorBufferSR()->GetShaderResourceView());
		context->PSSetShaderResources(2, 1, lightManager->GetSpotLightTransformBufferSR()->GetShaderResourceView());
		context->PSSetShaderResources(3, 1, lightManager->GetSpotLightColorBufferSR()->GetShaderResourceView());
		context->PSSetShaderResources(4, 1, lightManager->GetSpotLightParamBufferSR()->GetShaderResourceView());
		context->PSSetShaderResources(5, 1, lightManager->GetDirectionalLightTransformBufferSR()->GetShaderResourceView());
		context->PSSetShaderResources(6, 1, lightManager->GetDirectionalLightColorBufferSR()->GetShaderResourceView());
		context->PSSetShaderResources(7, 1, lightManager->GetDirectionalLightParamBufferSR()->GetShaderResourceView());

		ID3D11RenderTargetView* nullRTV = nullptr;
		context->OMSetRenderTargets(1, &nullRTV, _blendedDepthBuffer->GetDepthStencilView());

		const std::vector<const Mesh::Mesh*>& meshes = renderMgr->GetTransparentMeshes().meshes.GetVector();
		RenderMesh(meshes, RenderType::Transparency);

		ID3D11ShaderResourceView* nullSRV = nullptr;
		context->PSSetShaderResources(0, 1, &nullSRV);
		context->PSSetShaderResources(1, 1, &nullSRV);
		context->PSSetShaderResources(2, 1, &nullSRV);
		context->PSSetShaderResources(3, 1, &nullSRV);
		context->PSSetShaderResources(4, 1, &nullSRV);
		context->PSSetShaderResources(5, 1, &nullSRV);
		context->PSSetShaderResources(6, 1, &nullSRV);
		context->PSSetShaderResources(7, 1, &nullSRV);
	}
}

void DeferredCamera::EnableRenderTransparentMesh(bool enable)
{	
	if(enable)
	{
		const Size<unsigned int> windowSize = Director::GetInstance()->GetWindowSize();

		ASSERT_COND_MSG(_blendedDepthBuffer, "Error, Already allocated depth");
		{
			_blendedDepthBuffer =  new DepthBuffer;
			_blendedDepthBuffer->Initialize(windowSize);
		}


		ASSERT_COND_MSG(_blendedMeshLightCulling, "Error, Already allocated depth");
		{
			_blendedMeshLightCulling = new OnlyLightCulling;
			{
				EngineFactory shaderFactory(nullptr); //only use FetchShaderFullPath

				std::string path = "";
				shaderFactory.FetchShaderFullPath(path, "TileBasedDeferredShading");

				ASSERT_COND_MSG(path.empty() == false, "Error, path is null");
				_blendedMeshLightCulling->Initialize(path, "CS", true, _opaqueDepthBuffer, _blendedDepthBuffer);
			}
		}
	}
	else // enable == false
	{
		SAFE_DELETE(_blendedDepthBuffer);
		SAFE_DELETE(_blendedMeshLightCulling);
	}
}

Core::Component* DeferredCamera::Clone() const
{
	return nullptr;
}