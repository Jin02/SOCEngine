#include "MeshCamera.h"
#include "Director.h"
#include "EngineShaderFactory.hpp"
#include "ResourceManager.h"
#include "PhysicallyBasedMaterial.h"

using namespace Rendering::Camera;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Device;
using namespace Math;
using namespace Intersection;
using namespace Rendering::Factory;
using namespace Rendering::Light;
using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Rendering::TBDR;
using namespace Rendering;

MeshCamera::MeshCamera() : CameraForm(Usage::MeshRender),
	_blendedDepthBuffer(nullptr), _albedo_occlusion(nullptr),
	_motionXY_metallic_specularity(nullptr), _normal_roughness(nullptr),
	_useTransparent(false), _opaqueDepthBuffer(nullptr),
	_tbrParamConstBuffer(nullptr), _offScreen(nullptr),
	_blendedMeshLightCulling(nullptr), _emission_materialFlag(nullptr)
{
}

MeshCamera::~MeshCamera()
{
	OnDestroy();
}

void MeshCamera::OnInitialize()
{
	Size<unsigned int> backBufferSize = Director::SharedInstance()->GetBackBufferSize();
	CameraForm::Initialize(Math::Rect<float>(0.0f, 0.0f, float(backBufferSize.w), float(backBufferSize.h)));

	_albedo_occlusion = new Texture::RenderTexture;
	ASSERT_COND_MSG( 
		_albedo_occlusion->Initialize(backBufferSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0),
		"GBuffer Error : cant create albedo_occlusion render texture" 
		);

	_motionXY_metallic_specularity = new Texture::RenderTexture;
	ASSERT_COND_MSG( 
		_motionXY_metallic_specularity->Initialize(backBufferSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0),
		"GBuffer Error : cant create _motionXY_metallic_specularity render texture"
		);

	_normal_roughness = new Texture::RenderTexture;
	ASSERT_COND_MSG( 
		_normal_roughness->Initialize(backBufferSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0),
		"GBuffer Error : cant create _normal_roughness render texture" 
		);

	_emission_materialFlag = new Texture::RenderTexture;
	ASSERT_COND_MSG(
		_emission_materialFlag->Initialize(backBufferSize, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN, 0),
		"GBuffer Error : cant create _emission_materialFlag render texture"
		);

	_opaqueDepthBuffer = new Texture::DepthBuffer;
	_opaqueDepthBuffer->Initialize(backBufferSize, true);

	EnableRenderTransparentMesh(false);

	_deferredShadingWithLightCulling = new ShadingWithLightCulling;
	{
		ShadingWithLightCulling::GBuffers gbuffer;
		{
			gbuffer.albedo_occlusion				= _albedo_occlusion;
			gbuffer.normal_roughness				= _normal_roughness;
			gbuffer.motionXY_metallic_specularity	= _motionXY_metallic_specularity;
			gbuffer.emission_materialFlag			= _emission_materialFlag;
		}

		_deferredShadingWithLightCulling->Initialize(_opaqueDepthBuffer, gbuffer, backBufferSize, true);
	}

	_tbrParamConstBuffer = new ConstBuffer;
	_tbrParamConstBuffer->Initialize(sizeof(LightCulling::TBRParam));

	_offScreen = new OffScreen;
	_offScreen->Initialize(	_deferredShadingWithLightCulling->GetDiffuseLightBuffer(),
							_deferredShadingWithLightCulling->GetSpecularLightBuffer(),
							false);
}

void MeshCamera::OnDestroy()
{
	SAFE_DELETE(_tbrParamConstBuffer);

	SAFE_DELETE(_albedo_occlusion);
	SAFE_DELETE(_motionXY_metallic_specularity);
	SAFE_DELETE(_normal_roughness);
	SAFE_DELETE(_emission_materialFlag);

	SAFE_DELETE(_deferredShadingWithLightCulling);
	SAFE_DELETE(_opaqueDepthBuffer);

	SAFE_DELETE(_blendedMeshLightCulling);
	SAFE_DELETE(_blendedDepthBuffer);
	SAFE_DELETE(_offScreen);

	CameraForm::Destroy();
}

void MeshCamera::CullingWithUpdateCB(const Device::DirectX* dx, const std::vector<Core::Object*>& objects, const Manager::LightManager* lightManager)
{
	CameraCBData camConstBufferData;

	Matrix	worldMat;
	Matrix& viewMat = camConstBufferData.viewMat;
	Matrix	projMat;
	Matrix	viewProjMat;
	{
		_owner->GetTransform()->FetchWorldMatrix(worldMat);

		CameraForm::GetViewMatrix(viewMat, worldMat);
		GetProjectionMatrix(projMat, true);
		viewProjMat = viewMat * projMat;

		camConstBufferData.worldPos = Vector4(worldMat._41, worldMat._42, worldMat._43, 1.0f);

		bool updatedVP = memcmp(&_prevCamMatCBData, &camConstBufferData, sizeof(CameraCBData)) != 0;
		if(updatedVP)
		{
			// Make Frustum
			{
				Matrix notInvProj;
				GetProjectionMatrix(notInvProj, false);
				_frustum->Make(camConstBufferData.viewMat * notInvProj);
			}

			_prevCamMatCBData = camConstBufferData;

			Matrix::Transpose(camConstBufferData.viewMat,		camConstBufferData.viewMat);
			Matrix::Transpose(camConstBufferData.viewProjMat,	viewProjMat);

			_camMatConstBuffer->UpdateSubResource(dx->GetContext(), &camConstBufferData);
		}

		for(auto iter = objects.begin(); iter != objects.end(); ++iter)
			(*iter)->Culling(_frustum);
	}

	LightCulling::TBRParam tbrParam;
	{
		tbrParam.viewMat = viewMat;

		Matrix invProjMat;
		Matrix::Inverse(invProjMat, projMat);
		Matrix::Transpose(tbrParam.invProjMat, invProjMat);

		Matrix invViewportMat;
		GetInvViewportMatrix(invViewportMat, _renderRect);

		Matrix invViewProj;
		Matrix::Inverse(invViewProj, viewProjMat);
		Matrix invViewProjViewport = invViewportMat * invViewProj;

		Matrix::Transpose(tbrParam.invViewProjViewport, invViewProjViewport);

		Size<uint> viewportSize = Director::SharedInstance()->GetBackBufferSize();
		tbrParam.packedViewportSize		= (viewportSize.w << 16) | viewportSize.h;
		tbrParam.packedNumOfLights		= lightManager->GetPackedLightCount();
		tbrParam.maxNumOfperLightInTile	= LightCulling::CalcMaxNumLightsInTile();

		tbrParam.camWorldPosition		= Math::Vector3(worldMat._41, worldMat._42, worldMat._43);
		tbrParam.cameraNear				= _clippingNear;
		tbrParam.cameraFar				= _clippingFar;
	}

	if( memcmp(&_prevParamData, &tbrParam, sizeof(LightCulling::TBRParam)) != 0 )
	{
		// Update Const Buffer
		ID3D11DeviceContext* context = dx->GetContext();
		_tbrParamConstBuffer->UpdateSubResource(context, &tbrParam);

		_prevParamData = tbrParam;
	}
}

void MeshCamera::RenderMeshWithoutIASetVB(
	const Device::DirectX* dx, const RenderManager* renderManager,
	const Geometry::Mesh* mesh, RenderType renderType, 
	const ConstBuffer* camMatConstBuffer)
{
	ID3D11DeviceContext* context = dx->GetContext();

	Geometry::MeshFilter* filter = mesh->GetMeshFilter();
	filter->GetIndexBuffer()->IASetBuffer(context);

	ShaderGroup shaders;

	if(renderManager->FindShader(shaders, filter->GetBufferFlag(), renderType) == false)
		ASSERT_MSG("Error, Unsupported renderType");

	Geometry::MeshRenderer* renderer	= mesh->GetMeshRenderer();
	const auto& materials				= renderer->GetMaterials();
	for(auto iter = materials.begin(); iter != materials.end(); ++iter)
	{					
		const Material* material = (*iter);
		const Material::CustomShader& customShader = material->GetCustomShader();
		PixelShader* ps		= shaders.ps;
		VertexShader* vs	= shaders.vs;
		GeometryShader* gs	= shaders.gs;

		if(customShader.shaderGroup.IsAllEmpty() == false)
		{
			if(customShader.isDeferred == false)
			{
				ASSERT_MSG("Error, Current version doesn't support custom shader(and normal forward rendering)");
				continue;
			}

			const ShaderGroup& shaderGroup = customShader.shaderGroup;
			ps = shaderGroup.ps;
			vs = shaderGroup.vs;
			gs = shaderGroup.gs;
		}

		std::vector<ShaderForm::InputConstBuffer> constBuffers = material->GetConstBuffers();
		{
			// Setting Transform ConstBuffer
			{
				uint bindIndex = (uint)ConstBufferBindIndex::World;
				ShaderForm::InputConstBuffer buf = ShaderForm::InputConstBuffer(bindIndex, mesh->GetWorldMatrixConstBuffer(), true, gs != nullptr, false, false);
				constBuffers.push_back(buf);
			}

			// Camera
			if(camMatConstBuffer)
			{
				uint bindIndex = (uint)ConstBufferBindIndex::Camera;
				ShaderForm::InputConstBuffer buf = ShaderForm::InputConstBuffer(bindIndex, camMatConstBuffer, true, gs != nullptr, false, false);
				constBuffers.push_back(buf);
			}
		}

		const auto& textures	= material->GetTextures();
		const auto& srBuffers	= material->GetShaderResourceBuffers();

		vs->BindShaderToContext(context);
		vs->BindInputLayoutToContext(context);
		vs->BindResourcesToContext(context, &constBuffers, &textures, &srBuffers);

		if(ps && (renderType != RenderType::Forward_OnlyDepth) )
		{
			ps->BindShaderToContext(context);
			ps->BindResourcesToContext(context, &constBuffers, &textures, &srBuffers);
		}
		else
		{
			context->PSSetShader(nullptr, nullptr, 0);
		}

		if(gs)
		{
			gs->BindShaderToContext(context);
			gs->BindResourcesToContext(context, &constBuffers, &textures, &srBuffers);
		}

		context->DrawIndexed(filter->GetIndexCount(), 0, 0);

		if(vs) context->VSSetShader(nullptr, nullptr, 0);
		if(ps) context->PSSetShader(nullptr, nullptr, 0);
		if(gs) context->GSSetShader(nullptr, nullptr, 0);
	}
}

void MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(
	const Device::DirectX* dx, const Manager::RenderManager* renderManager,
	const Manager::RenderManager::MeshList& meshes, RenderType renderType,
	const Buffer::ConstBuffer* camMatConstBuffer,
	std::function<bool(const Intersection::Sphere&)>* intersectFunc)
{
	ID3D11DeviceContext* context = dx->GetContext();

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
			if(obj->GetUse())
			{
				bool isCulled = obj->GetCulled(); //In Mesh Camera
				if(intersectFunc)
				{
					Vector3 worldPos;
					obj->GetTransform()->FetchWorldPosition(worldPos);

					Sphere sphere(worldPos, obj->GetRadius());
					isCulled |= ((*intersectFunc)(sphere) == false);
				}

				if(isCulled == false)
				{
					if(updateVB == false)
					{
						mesh->GetMeshFilter()->GetVertexBuffer()->IASetBuffer(context);
						updateVB = true;
					}

					RenderMeshWithoutIASetVB(dx, renderManager, mesh, renderType, camMatConstBuffer);
				}
			}
		}
	}
}

void MeshCamera::RenderMeshesUsingMeshVector(
	const Device::DirectX* dx, const Manager::RenderManager* renderManager,
	const std::vector<const Geometry::Mesh*>& meshes, 
	RenderType renderType, const Buffer::ConstBuffer* camMatConstBuffer,
	std::function<bool(const Intersection::Sphere&)>* intersectFunc)
{
	ID3D11DeviceContext* context = dx->GetContext();

	for(auto meshIter = meshes.begin(); meshIter != meshes.end(); ++meshIter)
	{
		const Geometry::Mesh* mesh = (*meshIter);

		const Core::Object* obj = mesh->GetOwner();
		if(obj->GetUse())
		{
			bool isCulled = obj->GetCulled(); //In Mesh Camera
			if(intersectFunc)
			{
				Vector3 worldPos;
				obj->GetTransform()->FetchWorldPosition(worldPos);

				Sphere sphere(worldPos, obj->GetRadius());
				isCulled |= (*intersectFunc)(sphere) == false;
			}

			// VB기준으로 정렬되어 있지 않기 때문에,
			// RenderMeshesUsingSortedMeshVectorByVB와 달리 그냥 매 오브젝트마다 IASetBuffer를 해준다.
			// 뭐 이것저것 따지면서 하자면 더 효율적으로 고칠 수 있겠다만, 많이 귀찮다.
			if(isCulled == false)
			{
				Geometry::MeshFilter* filter = mesh->GetMeshFilter();
				filter->GetVertexBuffer()->IASetBuffer(context);

				MeshCamera::RenderMeshWithoutIASetVB(dx, renderManager, mesh, renderType, camMatConstBuffer);
			}
		}
	}
}

void MeshCamera::Render(const Device::DirectX* dx,
						const RenderManager* renderManager, const LightManager* lightManager,
						const Buffer::ConstBuffer* shadowGlobalParamCB, bool neverUseVSM,
						Sky::SkyForm* sky,
						std::function<const RenderTexture*(MeshCamera*)> giPass)
{
	auto SetCurrentViewport = [](ID3D11DeviceContext* context, const Rect<float>& renderRect) -> void
	{
		D3D11_VIEWPORT viewport;
		{
			viewport.TopLeftX	= renderRect.x;
			viewport.TopLeftY	= renderRect.y;
			viewport.MinDepth	= 0.0f;
			viewport.MaxDepth	= 1.0f;
			viewport.Width		= renderRect.size.w;
			viewport.Height		= renderRect.size.h;
		}
		context->RSSetViewports(1, &viewport);
	};

	ID3D11DeviceContext* context = dx->GetContext();

	//Clear
	{
		Color allZeroColor(0.f, 0.f, 0.f, 0.f);

		_albedo_occlusion->Clear(context, allZeroColor);
		_motionXY_metallic_specularity->Clear(context, allZeroColor);
		_normal_roughness->Clear(context, allZeroColor);
		_emission_materialFlag->Clear(context, allZeroColor);
	}

	//inverted depth, so clear value is 0
	_opaqueDepthBuffer->Clear(context, 0.0f, 0);

	if(_useTransparent)
	{
		//inverted depth, so clear value is 0
		_blendedDepthBuffer->Clear(context, 0.0f, 0);
		SortTransparentMeshRenderQueue(_transparentMeshQueue, _owner->GetTransform(), renderManager);
	}

	// off alpha blending
	float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
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
		
	ID3D11SamplerState* samplerState = dx->GetSamplerStateAnisotropic();
	context->PSSetSamplers((uint)SamplerStateBindIndex::DefaultSamplerState, 1, &samplerState);

	//GBuffer
	{
		ID3D11RenderTargetView* renderTargetViews[] = {
			_albedo_occlusion->GetRenderTargetView(),
			_motionXY_metallic_specularity->GetRenderTargetView(),
			_normal_roughness->GetRenderTargetView(),
			_emission_materialFlag->GetRenderTargetView(),
			nullptr
		};

		ID3D11DepthStencilView* dsv = _opaqueDepthBuffer->GetDepthStencilView();
		context->OMSetRenderTargets(NumOfRenderTargets, renderTargetViews, dsv);
		context->OMSetDepthStencilState(dx->GetDepthStateGreater(), 0);

		SetCurrentViewport(context, _renderRect);

		//Opaque Mesh
		{
			const auto& meshes = renderManager->GetOpaqueMeshes();
			uint count = meshes.meshes.GetVector().size();

			context->RSSetState( nullptr );

			if(count > 0)
				MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(dx, renderManager, meshes, RenderType::GBuffer_Opaque, _camMatConstBuffer);
		}

		//Alpha Test Mesh
		{
			const auto& meshes = renderManager->GetAlphaTestMeshes();
			uint count = meshes.meshes.GetVector().size();

			if(count > 0)
			{
				bool useMSAA = dx->GetMSAADesc().Count > 1;

				if(useMSAA) //on alpha blending
					context->OMSetBlendState(dx->GetBlendStateAlphaToCoverage(), blendFactor, 0xffffffff);

				context->RSSetState( dx->GetRasterizerStateCWDisableCulling() );
		
				MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(dx, renderManager, meshes, RenderType::GBuffer_AlphaBlend, _camMatConstBuffer);

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
			//context->PSSetShader(nullptr, nullptr, 0);

			const std::vector<const Geometry::Mesh*>& meshes = _transparentMeshQueue.meshes;
			MeshCamera::RenderMeshesUsingMeshVector(dx, renderManager, meshes, RenderType::Forward_OnlyDepth, _camMatConstBuffer, nullptr);
		}
	}

	// Light Culling and Deferred Shading
	{
		//bool useShadow = shadowGlobalParamCB != nullptr;
		//if(useShadow)
		{
#if defined(USE_SHADOW_INVERTED_DEPTH)
			ID3D11SamplerState* shadowSamplerState = dx->GetShadowGreaterEqualSamplerComparisonState();
#else
			ID3D11SamplerState* shadowSamplerState = dx->GetShadowLessEqualSamplerComparisonState();
#endif
			context->CSSetSamplers((uint)SamplerStateBindIndex::ShadowComprisonSamplerState, 1, &shadowSamplerState);
			if(neverUseVSM == false)
			{
				ID3D11SamplerState* shadowSamplerState = dx->GetShadowSamplerState();
				context->CSSetSamplers((uint)SamplerStateBindIndex::VSMShadowSamplerState, 1, &shadowSamplerState);
			}
		}

		ID3D11RenderTargetView* nullRTVs[] = { nullptr, nullptr, nullptr, nullptr };
		ID3D11DepthStencilView* nullDSV = nullptr;
		context->OMSetRenderTargets(NumOfRenderTargets, nullRTVs, nullDSV);
		context->OMSetDepthStencilState(dx->GetDepthStateDisableDepthTest(), 0);

		context->VSSetShader(nullptr, nullptr, 0);
		context->PSSetShader(nullptr, nullptr, 0);
		ID3D11SamplerState* nullSampler = nullptr;
		context->PSSetSamplers((uint)SamplerStateBindIndex::DefaultSamplerState, 1, &nullSampler);

		_deferredShadingWithLightCulling->Dispatch(dx, _tbrParamConstBuffer, shadowGlobalParamCB);

		if(_useTransparent)
			_blendedMeshLightCulling->Dispatch(dx, _tbrParamConstBuffer);

		//if(useShadow)
		{
			context->CSSetSamplers((uint)SamplerStateBindIndex::ShadowComprisonSamplerState, 1, &nullSampler);

			if(neverUseVSM == false)
				context->CSSetSamplers((uint)SamplerStateBindIndex::VSMShadowSamplerState, 1, &nullSampler);
		}
	}

	const RenderTexture* indirectColorMap = (giPass != nullptr) ? giPass(this) : nullptr;

	// Main RT
	SetCurrentViewport(context, _renderRect); 
	_offScreen->Render(	dx, _renderTarget, indirectColorMap);

	if(sky)
	{
		if(sky->GetIsSkyOn())
			sky->Render(dx, this, _renderTarget, _opaqueDepthBuffer);
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

			lightManager->BindResources(dx, false, false, true);
			{
				// Light Culling Buffer
				context->PSSetShaderResources((uint)TextureBindIndex::LightIndexBuffer,
					1, _blendedMeshLightCulling->GetLightIndexSRBuffer()->GetShaderResourceView());
	
				ID3D11Buffer* tbrCB = _tbrParamConstBuffer->GetBuffer();
				context->VSSetConstantBuffers((uint)ConstBufferBindIndex::TBRParam, 1, &tbrCB);
				context->PSSetConstantBuffers((uint)ConstBufferBindIndex::TBRParam, 1, &tbrCB);
	
				MeshCamera::RenderMeshesUsingMeshVector(dx, renderManager, meshes, RenderType::Forward_Transparency, _camMatConstBuffer);
			}
			lightManager->UnbindResources(dx, false, false, true);		

			context->RSSetState(nullptr);
			context->OMSetBlendState(dx->GetBlendStateOpaque(), blendFactor, 0xffffffff);
			context->OMSetDepthStencilState(dx->GetDepthStateGreater(), 0);
		}
	}
}

void MeshCamera::EnableRenderTransparentMesh(bool enable)
{	
	if(enable)
	{
		const Size<unsigned int> backBufferSize = Director::SharedInstance()->GetBackBufferSize();

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

Core::Component* MeshCamera::Clone() const
{
	return nullptr;
}