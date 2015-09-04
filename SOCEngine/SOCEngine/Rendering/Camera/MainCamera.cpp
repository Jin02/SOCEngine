#include "MainCamera.h"
#include "Director.h"
#include "EngineShaderFactory.hpp"
#include "ResourceManager.h"
#include "PhysicallyBasedMaterial.h"

using namespace Rendering::Camera;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Device;
using namespace Math;
using namespace Rendering::Factory;
using namespace Rendering::Light;
using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Rendering::TBDR;
using namespace Rendering;

MainCamera::MainCamera() : CameraForm(),
	_blendedDepthBuffer(nullptr), _albedo_metallic(nullptr),
	_specular_fresnel0(nullptr), _normal_roughness(nullptr),
	_useTransparent(false), _opaqueDepthBuffer(nullptr),
	_tbrParamConstBuffer(nullptr), _offScreen(nullptr),
	_blendedMeshLightCulling(nullptr)
{
}

MainCamera::~MainCamera()
{
	OnDestroy();
}

void MainCamera::OnInitialize()
{
	CameraForm::Initialize();

	Size<unsigned int> backBufferSize = Director::GetInstance()->GetBackBufferSize();

	_albedo_metallic = new Texture::RenderTexture;
	ASSERT_COND_MSG( 
		_albedo_metallic->Initialize(backBufferSize, DXGI_FORMAT_R8G8B8A8_UNORM, 0),
		"GBuffer Error : cant create albedo_opacity render texture" 
		);

	_specular_fresnel0 = new Texture::RenderTexture;
	ASSERT_COND_MSG( 
		_specular_fresnel0->Initialize(backBufferSize, DXGI_FORMAT_R8G8B8A8_UNORM, 0),
		"GBuffer Error : cant create _specular_fresnel0 render texture"
		);

	_normal_roughness = new Texture::RenderTexture;
	ASSERT_COND_MSG( 
		_normal_roughness->Initialize(backBufferSize, DXGI_FORMAT_R8G8B8A8_UNORM, 0),
		"GBuffer Error : cant create _normal_roughness render texture" 
		);

	_opaqueDepthBuffer = new Texture::DepthBuffer;
	_opaqueDepthBuffer->Initialize(backBufferSize, true);

	EnableRenderTransparentMesh(true);

	_deferredShadingWithLightCulling = new TBDR::ShadingWithLightCulling;
	_deferredShadingWithLightCulling->Initialize(_opaqueDepthBuffer, _albedo_metallic, _specular_fresnel0, _normal_roughness, backBufferSize);

	_tbrParamConstBuffer = new ConstBuffer;
	_tbrParamConstBuffer->Initialize(sizeof(LightCulling::TBRParam));

	_offScreen = new OffScreen;
	_offScreen->Initialize(_deferredShadingWithLightCulling->GetOffScreen()->GetRenderTexture());

	auto camMgr = Device::Director::GetInstance()->GetCurrentScene()->GetCameraManager();
	CameraForm* thisCam = this;
	camMgr->Add(_owner->GetName(), thisCam);
}

void MainCamera::OnDestroy()
{
	SAFE_DELETE(_albedo_metallic);
	SAFE_DELETE(_specular_fresnel0);
	SAFE_DELETE(_normal_roughness);
	SAFE_DELETE(_blendedDepthBuffer);

	SAFE_DELETE(_tbrParamConstBuffer);
	SAFE_DELETE(_deferredShadingWithLightCulling);
	SAFE_DELETE(_offScreen);
	SAFE_DELETE(_blendedMeshLightCulling);

	CameraForm::Destroy();
}

void MainCamera::Render(const Device::DirectX* dx, const RenderManager* renderManager, const LightManager* lightManager)
{
	ID3D11DeviceContext* context = dx->GetContext();

	//Clear
	{
		Color allZeroColor(0.f, 0.f, 0.f, 0.f);

		_albedo_metallic->Clear(context, allZeroColor);
		_specular_fresnel0->Clear(context, allZeroColor);
		_normal_roughness->Clear(context, allZeroColor);
	}

	//inverted depth, so clear value is 0
	_opaqueDepthBuffer->Clear(context, 0.0f, 0);

	if(_useTransparent)
	{
		//inverted depth, so clear value is 0
		_blendedDepthBuffer->Clear(context, 0.0f, 0);
		SortTransparentMeshRenderQueue(renderManager);
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
	//
	//		tbfrQueue.push_back(mfr);
	//		return true;
	//	}
	//
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
				renderManager->FindGBufferShader(shaders, filter->GetBufferFlag(), renderType == RenderType::AlphaMesh);
			else if(renderType == RenderType::Transparency || renderType == RenderType::Transparency_DepthOnly)
				renderManager->FindTransparencyShader(shaders, filter->GetBufferFlag(), renderType == RenderType::Transparency_DepthOnly);

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

				std::vector<ShaderForm::InputConstBuffer> constBuffers = material->GetConstBuffers();
				{
					// Setting Camera CosntBuffer
					{
						uint semanticIdx = (uint)InputConstBufferShaderIndex::Camera;
						ShaderForm::InputConstBuffer buf = ShaderForm::InputConstBuffer(semanticIdx, _camConstBuffer, true, false, false, true);
						constBuffers.push_back(buf);
					}

					// Setting Transform ConstBuffer
					{
						uint semanticIdx = (uint)PhysicallyBasedMaterial::InputConstBufferShaderIndex::Transform;
						ShaderForm::InputConstBuffer buf = ShaderForm::InputConstBuffer(semanticIdx, mesh->GetTransformConstBuffer(), true, false, false, false);
						constBuffers.push_back(buf);
					}
				}

				const auto& srBuffers = material->GetShaderResourceBuffers();

				shaders.vs->SetShaderToContext(context);
				shaders.vs->SetInputLayoutToContext(context);
				shaders.vs->UpdateResources(context, &constBuffers, &tex, &srBuffers);

				if(renderType != RenderType::Transparency_DepthOnly)
				{
					shaders.ps->SetShaderToContext(context);
					shaders.ps->UpdateResources(context, &constBuffers, &tex, &srBuffers);
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
			const std::vector<const Mesh::Mesh*>& meshes = renderManager->GetOpaqueMeshes().meshes.GetVector();
			RenderMesh(meshes, RenderType::Opaque);
		}

		//Alpha Test Mesh
		{
			const std::vector<const Mesh::Mesh*>& meshes = renderManager->GetAlphaTestMeshes().meshes.GetVector();

			if(meshes.size() > 0)
			{
				bool useMSAA = dx->GetMSAADesc().Count > 1;

				if(useMSAA) //on alpha blending
					context->OMSetBlendState(dx->GetBlendStateAlphaToCoverage(), blendFactor, 0xffffffff);

				context->RSSetState( dx->GetRasterizerStateDisableCulling() );
		
				RenderMesh(meshes, RenderType::AlphaMesh);

				context->RSSetState(nullptr);

				if(useMSAA) //off alpha blending
					context->OMSetBlendState(dx->GetBlendStateOpaque(), blendFactor, 0xffffffff);
			}
		}

		// Transparent Mesh
		// Write blended depth buffer
		if(_useTransparent)
		{
			ID3D11RenderTargetView* nullRTV = nullptr;
			context->OMSetRenderTargets(1, &nullRTV, _blendedDepthBuffer->GetDepthStencilView());
			
			const std::vector<const Mesh::Mesh*>& meshes = renderManager->GetTransparentMeshes().meshes.GetVector();
			RenderMesh(meshes, RenderType::Transparency_DepthOnly);
		}
	}

	// Light Culling and Deferred DeferredShading
	{
		ID3D11RenderTargetView* nullRTVs[] = {nullptr, nullptr, nullptr};
		ID3D11DepthStencilView* nullDSV = nullptr;
		context->OMSetRenderTargets(NumOfRenderTargets, nullRTVs, nullDSV);
		context->OMSetDepthStencilState(dx->GetDepthStateDisableDepthTest(), 0);

		context->VSSetShader(nullptr, nullptr, 0);
		context->PSSetShader(nullptr, nullptr, 0);
		ID3D11SamplerState* nullSampler = nullptr;
		context->PSSetSamplers(0, 1, &nullSampler);

		LightCulling::TBRChangeableParam changeableParam;
		{
			changeableParam.lightNum = lightManager->GetPackedLightCount();
			GetViewMatrix(changeableParam.viewMat);

			//inverse 처리는, const buffer 업데이트 직전에 함
			GetProjectionMatrix(changeableParam.invProjMat);
		}

		if( memcmp(&_prevParamData, &changeableParam, sizeof(LightCulling::TBRChangeableParam)) != 0 )
		{
			LightCulling::TBRParam tbrParam;
			//viewMat, lightNum
			memcpy(&tbrParam, &changeableParam, sizeof(LightCulling::TBRChangeableParam));

			const Matrix& viewMat = tbrParam.viewMat;
			const Matrix& projMat = tbrParam.invProjMat; //아직 inverse 처리 안함

			Matrix viewportMat;
			dx->GetViewportMatrix(viewportMat);

			tbrParam.invViewProjViewport = viewMat * projMat * viewportMat;
			Matrix::Inverse(tbrParam.invViewProjViewport, tbrParam.invViewProjViewport); //invViewProjViewport
			Matrix::Inverse(tbrParam.invProjMat, tbrParam.invProjMat); //invProj

			tbrParam.screenSize = Director::GetInstance()->GetBackBufferSize().Cast<float>();
			tbrParam.maxNumOfperLightInTile = LightCulling::CalcMaxNumLightsInTile();

			_tbrParamConstBuffer->UpdateSubResource(context, &tbrParam);

			_prevParamData = changeableParam;
		}

		_deferredShadingWithLightCulling->Dispatch(dx, _tbrParamConstBuffer, _camConstBuffer);

		if(_useTransparent)
			_blendedMeshLightCulling->Dispatch(dx, _tbrParamConstBuffer, _camConstBuffer);
	}

	// Main RT
	{
		_offScreen->Render(_renderTarget, dx->GetSamplerStateLinear());
	}

	// Transparency
	if(_useTransparent)
	{
		ID3D11RenderTargetView* thisCamRTV = _renderTarget->GetRenderTargetView();	
		context->OMSetRenderTargets(1, &thisCamRTV, _opaqueDepthBuffer->GetDepthStencilView());
		context->OMSetDepthStencilState(dx->GetDepthStateGreaterAndDisableDepthWrite(), 0x00);
				
		context->PSSetShaderResources((uint)InputBufferShaderIndex::PointLightRadiusWithCenter, 
			1, lightManager->GetPointLightTransformBufferSR()->GetShaderResourceView());
		context->PSSetShaderResources((uint)InputBufferShaderIndex::PointLightColor, 
			1, lightManager->GetPointLightColorBufferSR()->GetShaderResourceView());
		context->PSSetShaderResources((uint)InputBufferShaderIndex::SpotLightRadiusWithCenter, 
			1, lightManager->GetSpotLightTransformBufferSR()->GetShaderResourceView());
		context->PSSetShaderResources((uint)InputBufferShaderIndex::SpotLightColor, 
			1, lightManager->GetSpotLightColorBufferSR()->GetShaderResourceView());
		context->PSSetShaderResources((uint)InputBufferShaderIndex::SpotLightParam,
			1, lightManager->GetSpotLightParamBufferSR()->GetShaderResourceView());
		context->PSSetShaderResources((uint)InputBufferShaderIndex::DirectionalLightCenterWithDirZ,
			1, lightManager->GetDirectionalLightTransformBufferSR()->GetShaderResourceView());
		context->PSSetShaderResources((uint)InputBufferShaderIndex::DirectionalLightColor,
			1, lightManager->GetDirectionalLightColorBufferSR()->GetShaderResourceView());
		context->PSSetShaderResources((uint)InputBufferShaderIndex::DirectionalLightParam,
			1, lightManager->GetDirectionalLightParamBufferSR()->GetShaderResourceView());

		const std::vector<const Mesh::Mesh*>& meshes = renderManager->GetTransparentMeshes().meshes.GetVector();
		RenderMesh(meshes, RenderType::Transparency);

		ID3D11ShaderResourceView* nullSRV = nullptr;
		context->PSSetShaderResources((uint)InputBufferShaderIndex::PointLightRadiusWithCenter,		1, nullptr);
		context->PSSetShaderResources((uint)InputBufferShaderIndex::PointLightColor,				1, nullptr);
		context->PSSetShaderResources((uint)InputBufferShaderIndex::SpotLightRadiusWithCenter,		1, nullptr);
		context->PSSetShaderResources((uint)InputBufferShaderIndex::SpotLightColor,					1, nullptr);
		context->PSSetShaderResources((uint)InputBufferShaderIndex::SpotLightParam,					1, nullptr);
		context->PSSetShaderResources((uint)InputBufferShaderIndex::DirectionalLightCenterWithDirZ, 1, nullptr);
		context->PSSetShaderResources((uint)InputBufferShaderIndex::DirectionalLightColor,			1, nullptr);
		context->PSSetShaderResources((uint)InputBufferShaderIndex::DirectionalLightParam,			1, nullptr);
		
		context->OMSetDepthStencilState(dx->GetDepthStateGreater(), 0);
	}
}

void MainCamera::EnableRenderTransparentMesh(bool enable)
{	
	if(enable)
	{
		const Size<unsigned int> backBufferSize = Director::GetInstance()->GetBackBufferSize();

		ASSERT_COND_MSG(_blendedDepthBuffer == nullptr, "Error, Already allocated depth");
		{
			_blendedDepthBuffer =  new DepthBuffer;
			_blendedDepthBuffer->Initialize(backBufferSize, true);
		}

		ASSERT_COND_MSG(_blendedMeshLightCulling == nullptr, "Error, Already allocated depth");
		{
			_blendedMeshLightCulling = new OnlyLightCulling;
			_blendedMeshLightCulling->Initialize(_opaqueDepthBuffer, _blendedDepthBuffer, RenderType::TBDR);
		}
	}
	else // enable == false
	{
		SAFE_DELETE(_blendedDepthBuffer);
		SAFE_DELETE(_blendedMeshLightCulling);
	}

	_useTransparent = enable;
}

Core::Component* MainCamera::Clone() const
{
	return nullptr;
}