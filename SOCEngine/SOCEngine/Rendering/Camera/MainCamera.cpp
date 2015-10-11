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
	_blendedDepthBuffer(nullptr), _albedo_emission(nullptr),
	_specular_metallic(nullptr), _normal_roughness(nullptr),
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

	_albedo_emission = new Texture::RenderTexture;
	ASSERT_COND_MSG( 
		_albedo_emission->Initialize(backBufferSize, DXGI_FORMAT_R16G16B16A16_FLOAT, 0),
		"GBuffer Error : cant create albedo_emission render texture" 
		);

	_specular_metallic = new Texture::RenderTexture;
	ASSERT_COND_MSG( 
		_specular_metallic->Initialize(backBufferSize, DXGI_FORMAT_R16G16B16A16_FLOAT, 0),
		"GBuffer Error : cant create _specular_metallic render texture"
		);

	_normal_roughness = new Texture::RenderTexture;
	ASSERT_COND_MSG( 
		_normal_roughness->Initialize(backBufferSize, DXGI_FORMAT_R16G16B16A16_FLOAT, 0),
		"GBuffer Error : cant create _normal_roughness render texture" 
		);

	_opaqueDepthBuffer = new Texture::DepthBuffer;
	_opaqueDepthBuffer->Initialize(backBufferSize, true);

	EnableRenderTransparentMesh(true);

	_deferredShadingWithLightCulling = new TBDR::ShadingWithLightCulling;
	_deferredShadingWithLightCulling->Initialize(_opaqueDepthBuffer, _albedo_emission, _specular_metallic, _normal_roughness, backBufferSize);

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
	SAFE_DELETE(_albedo_emission);
	SAFE_DELETE(_specular_metallic);
	SAFE_DELETE(_normal_roughness);
	SAFE_DELETE(_blendedDepthBuffer);

	SAFE_DELETE(_tbrParamConstBuffer);
	SAFE_DELETE(_deferredShadingWithLightCulling);
	SAFE_DELETE(_offScreen);
	SAFE_DELETE(_blendedMeshLightCulling);

	CameraForm::Destroy();
}

void MainCamera::UpdateConstBuffer(const Device::DirectX* dx, const std::vector<Core::Object*>& objects, const Manager::LightManager* lightManager)
{
	Matrix worldMat, viewMat, projMat;
	{
		_owner->GetTransform()->FetchWorldMatrix(worldMat);
		CameraForm::GetViewMatrix(viewMat, worldMat);
		GetProjectionMatrix(projMat, false);

		_frustum->Make(viewMat * projMat);
		GetProjectionMatrix(projMat, true);
		_viewProjMat = viewMat * projMat;

		for(auto iter = objects.begin(); iter != objects.end(); ++iter)
		{
			(*iter)->Culling(_frustum);
			(*iter)->UpdateTransformCB(viewMat, projMat);
		}
	}

	LightCulling::TBRParam tbrParam;
	{
		Matrix::Transpose(tbrParam.viewMat, viewMat);

		Matrix invProjMat;
		Matrix::Inverse(invProjMat, projMat);
		Matrix::Transpose(tbrParam.invProjMat, invProjMat);

		Matrix invViewportMat;
		{
			Matrix viewportMat;
			dx->GetViewportMatrix(viewportMat);

			Matrix::Inverse(invViewportMat, viewportMat);
		}

		Matrix invViewProj;
		Matrix::Inverse(invViewProj, _viewProjMat);
		Matrix invViewProjViewport = invViewportMat * invViewProj;

		Matrix::Transpose(tbrParam.invViewProjViewport, invViewProjViewport);

		tbrParam.viewportSize = Director::GetInstance()->GetBackBufferSize().Cast<float>();
		tbrParam.packedNumOfLights = lightManager->GetPackedLightCount();

		tbrParam.maxNumOfperLightInTile = LightCulling::CalcMaxNumLightsInTile();

		tbrParam.camWorldPosition 
			= Math::Vector4(worldMat._41, worldMat._42, worldMat._43, worldMat._44);
	}

	if( memcmp(&_prevParamData, &tbrParam, sizeof(LightCulling::TBRParam)) != 0 )
	{
		// Update Const Buffer
		ID3D11DeviceContext* context = dx->GetContext();
		_tbrParamConstBuffer->UpdateSubResource(context, &tbrParam);

		_prevParamData = tbrParam;
	}
}

void MainCamera::Render(const Device::DirectX* dx, const RenderManager* renderManager, const LightManager* lightManager)
{
	dx->ClearDeviceContext();
	ID3D11DeviceContext* context = dx->GetContext();

	//Clear
	{
		Color allZeroColor(0.f, 0.f, 0.f, 0.f);

		_albedo_emission->Clear(context, allZeroColor);
		_specular_metallic->Clear(context, allZeroColor);
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
	float blendFactor[1] = {0, };
	context->OMSetBlendState(dx->GetBlendStateOpaque(), blendFactor, 0xffffffff);

	//struct MeshInForwardRendering
	//{
	//	const Geometry::Mesh*	mesh;
	//	const Material*		material;
	//};
	
	// TBFR = Tile Based Forward Rendering.
	// Used to render transparent meshes
	//std::vector<MeshInForwardRendering> tbfrQueue;
	//auto HasCustomShaderWithAddMeshToTBFRQueue = [&](const Material* material, const Geometry::Mesh* mesh)
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
	auto RenderMeshWithoutIASetVB = [&](const Geometry::Mesh* mesh, RenderType renderType)
	{
		Geometry::MeshFilter* filter = mesh->GetMeshFilter();

		filter->GetIndexBuffer()->IASetBuffer(context);

		ShaderGroup shaders;
		if(renderType == RenderType::Opaque || renderType == RenderType::AlphaMesh)
			renderManager->FindGBufferShader(shaders, filter->GetBufferFlag(), renderType == RenderType::AlphaMesh);
		else if(renderType == RenderType::Transparency || renderType == RenderType::Transparency_DepthOnly)
			renderManager->FindTransparencyShader(shaders, filter->GetBufferFlag(), renderType == RenderType::Transparency_DepthOnly);

		VertexShader* vs = shaders.vs;

		if(vs)
		{
			shaders.vs->SetShaderToContext(context);
			shaders.vs->SetInputLayoutToContext(context);
		}

		Geometry::MeshRenderer* renderer	= mesh->GetMeshRenderer();
		const auto& materials			= renderer->GetMaterials();
		for(auto iter = materials.begin(); iter != materials.end(); ++iter)
		{					
			Material* material = (*iter);
			const Material::CustomShader& customShader = material->GetCustomShader();
			PixelShader* ps = shaders.ps;

			if(customShader.shaderGroup.IsAllEmpty() == false)
			{
				if(customShader.isDeferred == false)
				{
					ASSERT_MSG("Error, Current version doesn't support custom shader(and normal forward rendering)");
					continue;
				}

				const ShaderGroup& shaderGroup = customShader.shaderGroup;
				ps = shaderGroup.ps;
			//	gs = shaderGroup.gs;
			//	hs = shaderGroup.hs;

				VertexShader* vs = shaderGroup.vs;
				ASSERT_COND_MSG(vs, "VS is null");
				{
					vs->SetShaderToContext(context);
					vs->SetInputLayoutToContext(context);
				}
			}

			std::vector<ShaderForm::InputConstBuffer> constBuffers = material->GetConstBuffers();
			{
				// Setting Transform ConstBuffer
				{
					uint semanticIdx = (uint)PhysicallyBasedMaterial::InputConstBufferShaderIndex::Transform;
					ShaderForm::InputConstBuffer buf = ShaderForm::InputConstBuffer(semanticIdx, mesh->GetTransformConstBuffer(), true, false, false, false);
					constBuffers.push_back(buf);
				}
			}

			const auto& textures	= material->GetTextures();
			const auto& srBuffers	= material->GetShaderResourceBuffers();

			vs->UpdateResources(context, &constBuffers, &textures, &srBuffers);

			if(ps && (renderType != RenderType::Transparency_DepthOnly) )
			{
				ps->UpdateResources(context, &constBuffers, &textures, &srBuffers);

				ps->SetShaderToContext(context);
				ps->UpdateResources(context, &constBuffers, &textures, &srBuffers);
			}

			context->DrawIndexed(filter->GetIndexCount(), 0, 0);
		}
	};
	auto RenderMeshesUsingMeshList = [&](const RenderManager::MeshList& meshes, RenderType renderType)
	{
		const auto& sortedMeshAddrByVertexBuffer = meshes.meshes.GetVector();
		for(auto iter = sortedMeshAddrByVertexBuffer.begin();
			iter != sortedMeshAddrByVertexBuffer.end(); ++iter)
		{
			const std::set<RenderManager::MeshList::meshkey>& meshAddrSets = *iter;

			bool updateVB = false;
			for(auto iter = meshAddrSets.begin(); iter != meshAddrSets.end(); ++iter)
			{
				const Geometry::Mesh* mesh = reinterpret_cast<const Geometry::Mesh*>(*iter);

				const Core::Object* obj = mesh->GetOwner();
				if( (obj->GetCulled() == false) || (obj->GetUse() == false) )
				{
					if(updateVB == false)
					{
						mesh->GetMeshFilter()->GetVertexBuffer()->IASetBuffer(context);
						updateVB = true;
					}

					RenderMeshWithoutIASetVB(mesh, renderType);
				}
			}
		}
	};
	auto RenderMeshesUsingMeshVector = [&](const std::vector<const Geometry::Mesh*>& meshes, RenderType renderType)
	{
		for(auto meshIter = meshes.begin(); meshIter != meshes.end(); ++meshIter)
		{
			const Geometry::Mesh* mesh = (*meshIter);

			const Core::Object* obj = mesh->GetOwner();
			if( (obj->GetCulled() == false) || (obj->GetUse() == false) )
			{
				Geometry::MeshFilter* filter = mesh->GetMeshFilter();
				filter->GetVertexBuffer()->IASetBuffer(context);

				RenderMeshWithoutIASetVB(mesh, renderType);
			}
		}
	};
	
	ID3D11SamplerState* samplerState = dx->GetSamplerStateAnisotropic();
	context->PSSetSamplers(0, 1, &samplerState);

	//GBuffer
	{
		ID3D11RenderTargetView* renderTargetViews[] = {
			_albedo_emission->GetRenderTargetView(),
			_specular_metallic->GetRenderTargetView(),
			_normal_roughness->GetRenderTargetView(),
			nullptr
		};

		ID3D11DepthStencilView* dsv = _opaqueDepthBuffer->GetDepthStencilView();
		context->OMSetRenderTargets(NumOfRenderTargets, renderTargetViews, dsv);
		context->OMSetDepthStencilState(dx->GetDepthStateGreater(), 0);

		//Opaque Mesh
		{
			const auto& meshList = renderManager->GetOpaqueMeshes();
			uint count = meshList.meshes.GetVector().size();

			if(count > 0)
				RenderMeshesUsingMeshList(meshList, RenderType::Opaque);
		}

		//Alpha Test Mesh
		{
			const auto& meshList = renderManager->GetAlphaTestMeshes();
			uint count = meshList.meshes.GetVector().size();

			if(count > 0)
			{
				bool useMSAA = dx->GetMSAADesc().Count > 1;

				if(useMSAA) //on alpha blending
					context->OMSetBlendState(dx->GetBlendStateAlphaToCoverage(), blendFactor, 0xffffffff);

				context->RSSetState( dx->GetRasterizerStateCWDisableCulling() );
		
				RenderMeshesUsingMeshList(meshList, RenderType::AlphaMesh);

				context->RSSetState( nullptr );

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
			context->PSSetShader(nullptr, nullptr, 0);

			const std::vector<const Geometry::Mesh*>& meshes = _transparentMeshQueue.meshes;
			RenderMeshesUsingMeshVector(meshes, RenderType::Transparency_DepthOnly);
		}
	}

	// Light Culling and Deferred Shading
	{
		ID3D11RenderTargetView* nullRTVs[] = {nullptr, nullptr, nullptr};
		ID3D11DepthStencilView* nullDSV = nullptr;
		context->OMSetRenderTargets(NumOfRenderTargets, nullRTVs, nullDSV);
		context->OMSetDepthStencilState(dx->GetDepthStateDisableDepthTest(), 0);

		context->VSSetShader(nullptr, nullptr, 0);
		context->PSSetShader(nullptr, nullptr, 0);
		ID3D11SamplerState* nullSampler = nullptr;
		context->PSSetSamplers(0, 1, &nullSampler);

		_deferredShadingWithLightCulling->Dispatch(dx, _tbrParamConstBuffer);

		if(_useTransparent)
			_blendedMeshLightCulling->Dispatch(dx, _tbrParamConstBuffer);
	}

	// Main RT
	{
		_offScreen->Render(_renderTarget, dx->GetSamplerStateLinear());
	}

	// Transparency
	if(_useTransparent)
	{
		const std::vector<const Geometry::Mesh*>& meshes = _transparentMeshQueue.meshes;

		if(meshes.size() > 0)
		{
			context->RSSetState(dx->GetRasterizerStateCWDisableCulling());
			context->OMSetBlendState(dx->GetBlendStateAlpha(), blendFactor, 0xffffffff);

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

			// Light Culling Buffer
			context->PSSetShaderResources((uint)InputShaderResourceBuffer::LightCullingBuffer,
				1, _blendedMeshLightCulling->GetLightIndexBuffer()->GetShaderResourceView());

			ID3D11Buffer* tbrCB = _tbrParamConstBuffer->GetBuffer();
			context->VSSetConstantBuffers((uint)TBDR::InputConstBufferShaderIndex::TBRParam, 1, &tbrCB);
			context->PSSetConstantBuffers((uint)TBDR::InputConstBufferShaderIndex::TBRParam, 1, &tbrCB);

			RenderMeshesUsingMeshVector(meshes, RenderType::Transparency);

			context->RSSetState(nullptr);
			context->OMSetBlendState(dx->GetBlendStateOpaque(), blendFactor, 0xffffffff);

			const uint startIdx	= (uint)InputBufferShaderIndex::PointLightRadiusWithCenter;
			const uint srvNum	= (uint)InputBufferShaderIndex::DirectionalLightParam - (uint)InputBufferShaderIndex::PointLightRadiusWithCenter + 1;

			ID3D11ShaderResourceView* nullSRV[srvNum] = {nullptr, };
			context->PSSetShaderResources(startIdx,	srvNum, nullSRV);
		
			context->OMSetDepthStencilState(dx->GetDepthStateGreater(), 0);
		}
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
			_blendedMeshLightCulling->Initialize(_opaqueDepthBuffer, _blendedDepthBuffer);
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