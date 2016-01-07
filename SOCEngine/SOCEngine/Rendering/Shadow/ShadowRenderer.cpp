#include "ShadowRenderer.h"
#include "Utility.h"
#include "Object.h"

#define USE_RENDER_WITH_UPDATE_CB

using namespace Structure;
using namespace Math;
using namespace Core;
using namespace Device;
using namespace Intersection;
using namespace Rendering::Buffer;
using namespace Rendering::Camera;
using namespace Rendering::Shadow;
using namespace Rendering::Texture;
using namespace Rendering::Light;
using namespace Rendering::Manager;

ShadowRenderer::ShadowRenderer() :
	_pointLightShadowMapAtlas(nullptr), _spotLightShadowMapAtlas(nullptr), _directionalLightShadowMapAtlas(nullptr),
	_pointLightMomentShadowMapAtlas(nullptr), _spotLightMomentShadowMapAtlas(nullptr), _directionalLightMomentShadowMapAtlas(nullptr), _useVSM(false),
	_pointLightShadowMapResolution(512),
	_spotLightShadowMapResolution(512),
	_directionalLightShadowMapResolution(512),
	_numOfShadowCastingPointLightInAtlas(0),
	_numOfShadowCastingSpotLightInAtlas(0),
	_numOfShadowCastingDirectionalLightInAtlas(0),
	_shadowGlobalParamCB(nullptr),
	_directionalLightShadowIdxToLightIdxSRBuffer(nullptr), _pointLightShadowIdxToLightIdxSRBuffer(nullptr), _spotLightShadowIdxToLightIdxSRBuffer(nullptr),
	_updateConter(0), _prevUpdateCounter(0xffffffff)
{
}

ShadowRenderer::~ShadowRenderer()
{
	Destroy();
}

void ShadowRenderer::Initialize(bool useVSM,
								uint numOfShadowCastingPointLight,
								uint numOfShadowCastingSpotLight,
								uint numOfShadowCastingDirectionalLight)
{
	_useVSM = useVSM;

	ResizeShadowMapAtlas(	numOfShadowCastingPointLight,
							numOfShadowCastingSpotLight,
							numOfShadowCastingDirectionalLight,
							-1, -1, -1 );

	_shadowGlobalParamCB = new ConstBuffer;
	_shadowGlobalParamCB->Initialize(sizeof(ShadowGlobalParam));

	const __int32 dummyData[POINT_LIGHT_BUFFER_MAX_NUM * 4] = {0, };

	_pointLightShadowIdxToLightIdxSRBuffer = new ShaderResourceBuffer;
	_pointLightShadowIdxToLightIdxSRBuffer->Initialize(
		4, POINT_LIGHT_BUFFER_MAX_NUM,
		DXGI_FORMAT_R32_UINT,
		dummyData, true, 0, D3D11_USAGE_DYNAMIC);
	
	_directionalLightShadowIdxToLightIdxSRBuffer = new ShaderResourceBuffer;
	_directionalLightShadowIdxToLightIdxSRBuffer->Initialize(
		4, DIRECTIONAL_LIGHT_BUFFER_MAX_NUM,
		DXGI_FORMAT_R32_UINT,
		dummyData, true, 0, D3D11_USAGE_DYNAMIC);

	_spotLightShadowIdxToLightIdxSRBuffer = new ShaderResourceBuffer;
	_spotLightShadowIdxToLightIdxSRBuffer->Initialize(
		4, SPOT_LIGHT_BUFFER_MAX_NUM,
		DXGI_FORMAT_R32_UINT,
		dummyData, true, 0, D3D11_USAGE_DYNAMIC);
}

void ShadowRenderer::ResizeShadowMapAtlas(
	uint numOfShadowCastingPointLight, uint numOfShadowCastingSpotLight, uint numOfShadowCastingDirectionalLight,
	uint pointLightShadowMapResolution, uint spotLightShadowMapResolution, uint directionalLightShadowMapResolution)
{
	auto Next2Squre =[](uint value) -> uint
	{
		return 1 << (uint)(ceil(log((uint)value) / log(2.0f)));
	};

	bool changedPLShadowMap = _pointLightShadowMapResolution != pointLightShadowMapResolution;
	bool changedSLShadowMap = _spotLightShadowMapResolution != spotLightShadowMapResolution;
	bool changedDLShadowMap = _directionalLightShadowMapResolution != directionalLightShadowMapResolution;

	if(_numOfShadowCastingPointLightInAtlas < numOfShadowCastingPointLight)
	{
		_numOfShadowCastingPointLightInAtlas = Next2Squre(numOfShadowCastingPointLight);
		changedPLShadowMap = true;
	}
	if(_numOfShadowCastingSpotLightInAtlas < numOfShadowCastingSpotLight)
	{
		_numOfShadowCastingSpotLightInAtlas = Next2Squre(numOfShadowCastingSpotLight);
		changedSLShadowMap = true;
	}
	if(_numOfShadowCastingDirectionalLightInAtlas < numOfShadowCastingDirectionalLight)
	{
		_numOfShadowCastingDirectionalLightInAtlas = Next2Squre(numOfShadowCastingDirectionalLight);
		changedDLShadowMap = true;
	}

	// Point Light Shadow Map ShadowMap
	if(changedPLShadowMap || (_pointLightShadowMapAtlas == nullptr))
	{
		SAFE_DELETE(_pointLightShadowMapAtlas);

		Size<uint> mapSize;
		mapSize.w = _numOfShadowCastingPointLightInAtlas * _pointLightShadowMapResolution;
		mapSize.h = 6 * _pointLightShadowMapResolution; //point light는 6면 렌더링

		_pointLightShadowMapAtlas = new DepthBuffer;
		_pointLightShadowMapAtlas->Initialize(mapSize, true, 1);

		if(_useVSM)
		{
			_pointLightMomentShadowMapAtlas = new RenderTexture;
			_pointLightMomentShadowMapAtlas->Initialize(mapSize, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN, 0, 1);
		}
	}

	// Spot Light
	if(changedSLShadowMap || (_spotLightShadowMapAtlas == nullptr))
	{
		SAFE_DELETE(_spotLightShadowMapAtlas);

		Size<uint> mapSize;
		mapSize.w = _numOfShadowCastingSpotLightInAtlas * _spotLightShadowMapResolution;
		mapSize.h = _spotLightShadowMapResolution;

		_spotLightShadowMapAtlas = new DepthBuffer;
		_spotLightShadowMapAtlas->Initialize(mapSize, true, 1);

		if(_useVSM)
		{
			_spotLightMomentShadowMapAtlas = new RenderTexture;
			_spotLightMomentShadowMapAtlas->Initialize(mapSize, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN, 0, 1);
		}
	}

	// dl
	if(changedDLShadowMap || (_directionalLightShadowMapAtlas == nullptr))
	{
		SAFE_DELETE(_directionalLightShadowMapAtlas);

		Size<uint> mapSize;
		mapSize.w = _numOfShadowCastingDirectionalLightInAtlas * _directionalLightShadowMapResolution;
		mapSize.h = _directionalLightShadowMapResolution;

		_directionalLightShadowMapAtlas = new DepthBuffer;
		_directionalLightShadowMapAtlas->Initialize(mapSize, true, 1);

		if(_useVSM)
		{
			_directionalLightMomentShadowMapAtlas = new RenderTexture;
			_directionalLightMomentShadowMapAtlas->Initialize(mapSize, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN, 0, 1);
		}
	}
}

void ShadowRenderer::Destroy()
{
	SAFE_DELETE(_shadowGlobalParamCB);

	SAFE_DELETE(_pointLightShadowMapAtlas);
	SAFE_DELETE(_spotLightShadowMapAtlas);
	SAFE_DELETE(_directionalLightShadowMapAtlas);

	SAFE_DELETE(_pointLightMomentShadowMapAtlas);
	SAFE_DELETE(_spotLightMomentShadowMapAtlas);
	SAFE_DELETE(_directionalLightMomentShadowMapAtlas);

	// DL
	{
		auto& lights = _shadowCastingDirectionalLights;
		uint count = lights.GetVector().size();
		for(uint i=0; i<count; ++i)
			SAFE_DELETE(lights.Get(i).camConstBuffer);
	}

	// SL
	{
		auto& lights = _shadowCastingSpotLights;
		uint count = lights.GetVector().size();
		for(uint i=0; i<count; ++i)
			SAFE_DELETE(lights.Get(i).camConstBuffer);
	}

	// PL
	{
		auto& lights = _shadowCastingPointLights;
		uint count = lights.GetVector().size();
		for(uint i=0; i<count; ++i)
		{
			for(uint j=0; j<6; ++j)
			{
				SAFE_DELETE(lights.Get(i).camConstBuffers[j]);
			}
		}
	}

	_directionalLightShadowIdxToLightIdxBuffer.DeleteAll();
	_pointLightShadowIdxToLightIdxBuffer.DeleteAll();
	_spotLightShadowIdxToLightIdxBuffer.DeleteAll();

	SAFE_DELETE(_directionalLightShadowIdxToLightIdxSRBuffer);
	SAFE_DELETE(_pointLightShadowIdxToLightIdxSRBuffer);
	SAFE_DELETE(_spotLightShadowIdxToLightIdxSRBuffer);
}

void ShadowRenderer::UpdateShadowCastingSpotLightCB(const Device::DirectX*& dx, uint index)
{
	ID3D11DeviceContext* context = dx->GetContext();

	auto& shadowCastingLight = _shadowCastingSpotLights.Get(index);
	const SpotLight* light = reinterpret_cast<const SpotLight*>(shadowCastingLight.lightAddress);

	CameraForm::CamMatCBData cbData;
	{
		cbData.viewMat		= light->GetInvViewProjectionMatrix();	// 사용하지 않는 viewMat대신 invViewProj 사용
		cbData.viewProjMat	= light->GetViewProjectionMatrix();
	}

	bool isDifferent = memcmp(&shadowCastingLight.prevViewProjMat, &cbData.viewProjMat, sizeof(Matrix)) != 0;
	if(isDifferent)
	{
		shadowCastingLight.prevViewProjMat = cbData.viewProjMat;

		Matrix::Transpose(cbData.viewMat,		cbData.viewMat);
		Matrix::Transpose(cbData.viewProjMat,	cbData.viewProjMat);

		ConstBuffer* camConstBuffer = shadowCastingLight.camConstBuffer;
		camConstBuffer->UpdateSubResource(context, &cbData);
	}
}

void ShadowRenderer::UpdateShadowCastingPointLightCB(const Device::DirectX*& dx, uint index)
{
	ID3D11DeviceContext* context = dx->GetContext();

	auto& shadowCastingLight = _shadowCastingPointLights.Get(index);
	const PointLight* light = reinterpret_cast<const PointLight*>(shadowCastingLight.lightAddress);

	std::array<Matrix, 6> invViewProjMatrices;
	std::array<Matrix, 6> viewProjMatrices;

	light->GetInvViewProjMatrices(invViewProjMatrices);
	light->GetViewProjectionMatrices(viewProjMatrices);

	bool isDifferent = memcmp(&shadowCastingLight.prevViewProjMat, &viewProjMatrices[0], sizeof(Matrix)) != 0;
	if(isDifferent)
	{
		shadowCastingLight.prevViewProjMat = viewProjMatrices[0];

		for(uint i=0; i<6; ++i)
		{
			CameraForm::CamMatCBData cb;
			{
				cb.viewMat		= invViewProjMatrices[i]; // 사용하지 않는 viewMat대신 invViewProj 사용
				cb.viewProjMat	= viewProjMatrices[i];
			}

			Matrix::Transpose(cb.viewMat,		cb.viewMat);
			Matrix::Transpose(cb.viewProjMat,	cb.viewProjMat);

			ConstBuffer* constBuffer = shadowCastingLight.camConstBuffers[i];
			constBuffer->UpdateSubResource(context, &cb);
		}
	}
}

void ShadowRenderer::UpdateShadowCastingDirectionalLightCB(const Device::DirectX*& dx, uint index)
{
	ID3D11DeviceContext* context = dx->GetContext();

	auto& shadowCastingLight = _shadowCastingDirectionalLights.Get(index);
	DirectionalLight* light = reinterpret_cast<DirectionalLight*>(shadowCastingLight.lightAddress);

	CameraForm::CamMatCBData cbData;
	{
		cbData.viewMat = light->GetInvViewProjectionMatrix(); // 사용하지 않는 viewMat대신 invViewProj 사용
		cbData.viewProjMat = light->GetViewProjectionMatrix();
	}

	bool isDifferent = memcmp(&shadowCastingLight.prevViewProjMat, &cbData.viewProjMat, sizeof(Matrix)) != 0;
	if(isDifferent)
	{
		shadowCastingLight.prevViewProjMat = cbData.viewProjMat;

		Matrix::Transpose(cbData.viewMat,		cbData.viewMat);
		Matrix::Transpose(cbData.viewProjMat,	cbData.viewProjMat);

		ConstBuffer* camConstBuffer = shadowCastingLight.camConstBuffer;
		camConstBuffer->UpdateSubResource(context, &cbData);
	}
}

void ShadowRenderer::RenderSpotLightShadowMap(const DirectX*& dx, const RenderManager*& renderManager)
{
	ID3D11DeviceContext* context = dx->GetContext();

	uint vpNum = 1;
	D3D11_VIEWPORT originViewport;
	context->RSGetViewports(&vpNum, &originViewport); 

	D3D11_VIEWPORT viewport;
	{
		viewport.Width		= (float)_spotLightShadowMapResolution;
		viewport.Height		= (float)_spotLightShadowMapResolution;
		viewport.MaxDepth	= 1.0f;
		viewport.MinDepth	= 0.0f;
		viewport.TopLeftY	= 0.0f;
	}

#if defined(USE_SHADOW_INVERTED_DEPTH)
	_spotLightShadowMapAtlas->Clear(context, 0.0f, 0);
#else
	_spotLightShadowMapAtlas->Clear(context, 1.0f, 0);
#endif

	ID3D11RenderTargetView* rtv		= nullptr;
	RenderType opaqueRenderType		= RenderType::Forward_OnlyDepth;
	RenderType alphaBlendRenderType	= RenderType::Forward_AlphaTestWithDiffuse;

	if(_useVSM)
	{
		Color clearColor(1.f, 1.f, 1.f, 1.f);
		_spotLightMomentShadowMapAtlas->Clear(context, clearColor);
		rtv = _spotLightMomentShadowMapAtlas->GetRenderTargetView();

		opaqueRenderType		= RenderType::Forward_MomentDepth;
		alphaBlendRenderType	= RenderType::Forward_MomentDepthWithAlphaTest;
	}

	context->OMSetRenderTargets(1, &rtv, _spotLightShadowMapAtlas->GetDepthStencilView());

	const auto& opaqueMeshes = renderManager->GetOpaqueMeshes();
	const auto& alphaTestMeshes = renderManager->GetAlphaTestMeshes();

	uint count = _shadowCastingSpotLights.GetSize();
	for(uint index = 0; index < count; ++index)
	{		
#ifdef USE_RENDER_WITH_UPDATE_CB
		UpdateShadowCastingSpotLightCB(dx, index);
#endif
		address lightAddress = _shadowCastingSpotLights.Get(index).lightAddress;		
		const SpotLight* light = reinterpret_cast<const SpotLight*>(lightAddress);
		auto IntersectLight = [&](const Sphere& sphere)
		{
			return light->Intersect(sphere);
		};
		std::function<bool(const Sphere&)> intersectFunc = IntersectLight;

		viewport.TopLeftX = (float)index * _spotLightShadowMapResolution;
		context->RSSetViewports(1, &viewport);

		const ConstBuffer* camConstBuffer = _shadowCastingSpotLights.Get(index).camConstBuffer;
		MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(
			dx, renderManager, opaqueMeshes,
			opaqueRenderType,
			camConstBuffer, &intersectFunc);


		context->RSSetState( dx->GetRasterizerStateCWDisableCulling() );

		MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(
			dx, renderManager, alphaTestMeshes,
			alphaBlendRenderType,
			camConstBuffer, &intersectFunc);

		context->RSSetState( nullptr );
	}

	context->RSSetViewports(1, &originViewport);
}

void ShadowRenderer::RenderPointLightShadowMap(const DirectX*& dx, const RenderManager*& renderManager)
{
	ID3D11DeviceContext* context = dx->GetContext();

	uint vpNum = 1;
	D3D11_VIEWPORT originViewport;
	context->RSGetViewports(&vpNum, &originViewport); 

	D3D11_VIEWPORT viewport;
	{
		viewport.Width		= (float)_pointLightShadowMapResolution;
		viewport.Height		= (float)_pointLightShadowMapResolution;
		viewport.MaxDepth	= 1.0f;
		viewport.MinDepth	= 0.0f;
		viewport.TopLeftY	= 0.0f;
	}

#if defined(USE_SHADOW_INVERTED_DEPTH)
	_pointLightShadowMapAtlas->Clear(context, 0.0f, 0);
#else
	_pointLightShadowMapAtlas->Clear(context, 1.0f, 0);
#endif

	ID3D11RenderTargetView* rtv		= nullptr;
	RenderType opaqueRenderType		= RenderType::Forward_OnlyDepth;
	RenderType alphaBlendRenderType	= RenderType::Forward_AlphaTestWithDiffuse;

	if(_useVSM)
	{
		Color clearColor(1.f, 1.f, 1.f, 1.f);
		_pointLightMomentShadowMapAtlas->Clear(context, clearColor);

		rtv = _pointLightMomentShadowMapAtlas->GetRenderTargetView();
		
		opaqueRenderType		= RenderType::Forward_MomentDepth;
		alphaBlendRenderType	= RenderType::Forward_MomentDepthWithAlphaTest;
	}

	context->OMSetRenderTargets(1, &rtv, _pointLightShadowMapAtlas->GetDepthStencilView());

	const auto& opaqueMeshes = renderManager->GetOpaqueMeshes();
	const auto& alphaTestMeshes = renderManager->GetAlphaTestMeshes();

	uint count = _shadowCastingPointLights.GetSize();
	for(uint index = 0; index < count; ++index)
	{
#ifdef USE_RENDER_WITH_UPDATE_CB
		UpdateShadowCastingPointLightCB(dx, index);
#endif
		address lightAddress = _shadowCastingPointLights.Get(index).lightAddress;		
		const PointLight* light = reinterpret_cast<const PointLight*>(lightAddress);
		auto IntersectLight = [&](const Sphere& sphere)
		{
			return light->Intersect(sphere);
		};
		std::function<bool(const Sphere&)> intersectFunc = IntersectLight;

		viewport.TopLeftX = (float)index * _pointLightShadowMapResolution;

		for(uint i=0; i<6; ++i)
		{
			viewport.TopLeftY = (float)i * _pointLightShadowMapResolution;
			context->RSSetViewports(1, &viewport);

			const ConstBuffer* camConstBuffer = _shadowCastingPointLights.Get(index).camConstBuffers[i];
			MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(
				dx, renderManager, opaqueMeshes,
				opaqueRenderType,
				camConstBuffer, &intersectFunc);

			context->RSSetState( dx->GetRasterizerStateCWDisableCulling() );
			MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(
				dx, renderManager, alphaTestMeshes,
				alphaBlendRenderType,
				camConstBuffer, &intersectFunc);
			context->RSSetState( nullptr );
		}
	}

	context->RSSetViewports(1, &originViewport);
}

void ShadowRenderer::RenderDirectionalLightShadowMap(const DirectX*& dx, const RenderManager*& renderManager)
{
	ID3D11DeviceContext* context = dx->GetContext();

	uint vpNum = 1;
	D3D11_VIEWPORT originViewport;
	context->RSGetViewports(&vpNum, &originViewport); 

	D3D11_VIEWPORT viewport;
	{
		viewport.Width		= (float)_directionalLightShadowMapResolution;
		viewport.Height		= (float)_directionalLightShadowMapResolution;
		viewport.MaxDepth	= 1.0f;
		viewport.MinDepth	= 0.0f;
		viewport.TopLeftY	= 0.0f;
	}

#if defined(USE_SHADOW_INVERTED_DEPTH)
	_directionalLightShadowMapAtlas->Clear(context, 0.0f, 0);
#else
	_directionalLightShadowMapAtlas->Clear(context, 1.0f, 0);
#endif

	ID3D11RenderTargetView* rtv		= nullptr;
	RenderType opaqueRenderType		= RenderType::Forward_OnlyDepth;
	RenderType alphaBlendRenderType	= RenderType::Forward_AlphaTestWithDiffuse;

	if(_useVSM)
	{
		Color clearColor(1.f, 1.f, 1.f, 1.f);
		_directionalLightMomentShadowMapAtlas->Clear(context, clearColor);

		rtv = _directionalLightMomentShadowMapAtlas->GetRenderTargetView();

		opaqueRenderType		= RenderType::Forward_MomentDepth;
		alphaBlendRenderType	= RenderType::Forward_MomentDepthWithAlphaTest;
	}

	context->OMSetRenderTargets(1, &rtv, _directionalLightShadowMapAtlas->GetDepthStencilView());

	const auto& opaqueMeshes = renderManager->GetOpaqueMeshes();
	const auto& alphaTestMeshes = renderManager->GetAlphaTestMeshes();

	uint count = _shadowCastingDirectionalLights.GetSize();
	for(uint index = 0; index < count; ++index)
	{
#ifdef USE_RENDER_WITH_UPDATE_CB
		UpdateShadowCastingDirectionalLightCB(dx, index);
#endif

		viewport.TopLeftX = (float)index * _directionalLightShadowMapResolution;
		context->RSSetViewports(1, &viewport);

		address lightAddress = _shadowCastingDirectionalLights.Get(index).lightAddress;		
		const DirectionalLight* light = reinterpret_cast<const DirectionalLight*>(lightAddress);
		auto IntersectLight = [&](const Sphere& sphere)
		{
			return light->Intersect(sphere);
		};
		std::function<bool(const Sphere&)> intersectFunc = IntersectLight;

		const ConstBuffer* camConstBuffer = _shadowCastingDirectionalLights.Get(index).camConstBuffer;
		MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(
			dx, renderManager, opaqueMeshes, 
			opaqueRenderType,
			camConstBuffer, &intersectFunc);

		context->RSSetState( dx->GetRasterizerStateCWDisableCulling() );
		MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(
			dx, renderManager, alphaTestMeshes,
			alphaBlendRenderType,
			camConstBuffer, &intersectFunc);
		context->RSSetState( nullptr );
	}

	context->RSSetViewports(1, &originViewport);
}

void ShadowRenderer::AddShadowCastingLight(const LightForm*& light, uint lightIndexInEachLights)
{
	address lightAddress = reinterpret_cast<address>(light);

	LightForm::LightType lightType = light->GetType();
	if(lightType == LightForm::LightType::Point)
	{
		if(_shadowCastingPointLights.Find(lightAddress))
			ASSERT_MSG("Error, Duplicated Light");

		ShadowCastingPointLight scl;
		scl.lightAddress	= lightAddress;
		for(uint i=0; i<6; ++i)
		{
			scl.camConstBuffers[i] = new ConstBuffer;
			scl.camConstBuffers[i]->Initialize(sizeof(CameraForm::CamMatCBData));
		}

		_shadowCastingPointLights.Add(lightAddress, scl);
		_pointLightShadowIdxToLightIdxBuffer.Add(lightAddress, lightIndexInEachLights);
	}
	else if( (lightType == LightForm::LightType::Spot) ||
			 (lightType == LightForm::LightType::Directional) )
	{
		Structure::VectorMap<address, ShadowCastingSpotDirectionalLight>* shadowCastingLights =
			(lightType == LightForm::LightType::Spot) ? &_shadowCastingSpotLights : &_shadowCastingDirectionalLights;

		if(shadowCastingLights->Find(lightAddress))
			ASSERT_MSG("Error, Duplicated Light");

		ShadowCastingSpotDirectionalLight scl;
		scl.lightAddress	= lightAddress;
		scl.camConstBuffer	= new ConstBuffer;
		scl.camConstBuffer->Initialize(sizeof(CameraForm::CamMatCBData));

		shadowCastingLights->Add(lightAddress, scl);

		Structure::VectorMap<address, uint>* shadowIdxTolightIdxBuffer =
			(lightType == LightForm::LightType::Spot) ? &_spotLightShadowIdxToLightIdxBuffer : &_directionalLightShadowIdxToLightIdxBuffer;

		shadowIdxTolightIdxBuffer->Add(lightAddress, lightIndexInEachLights);
	}
	else
		ASSERT_MSG("Unsupported light type.");

	uint plCount = _shadowCastingPointLights.GetSize();
	uint slCount = _shadowCastingSpotLights.GetSize();
	uint dlCount = _shadowCastingDirectionalLights.GetSize();
	ResizeShadowMapAtlas(plCount, slCount, dlCount,
		_pointLightShadowMapResolution,
		_spotLightShadowMapResolution,
		_directionalLightShadowMapResolution);

	++_updateConter;
}

void ShadowRenderer::DeleteShadowCastingLight(const LightForm*& light)
{
	address lightAddress = reinterpret_cast<address>(light);

	LightForm::LightType lightType = light->GetType();
	if(lightType == LightForm::LightType::Point)
	{
		ShadowCastingPointLight* scpl = _shadowCastingPointLights.Find(lightAddress);
		if(scpl == nullptr)
			return;

		for(uint i=0; i<6; ++i)
			SAFE_DELETE(scpl->camConstBuffers[i]);

		_shadowCastingPointLights.Delete(lightAddress);
		_pointLightShadowIdxToLightIdxBuffer.Delete(lightAddress);
	}
	else if( (lightType == LightForm::LightType::Spot) ||
			 (lightType == LightForm::LightType::Directional) )
	{
		Structure::VectorMap<address, ShadowCastingSpotDirectionalLight>* shadowCastingLights =
			(lightType == LightForm::LightType::Spot) ? &_shadowCastingSpotLights : &_shadowCastingDirectionalLights;

		ShadowCastingSpotDirectionalLight* scsdl = shadowCastingLights->Find(lightAddress);
		if(scsdl == nullptr)
			return;

		SAFE_DELETE(scsdl->camConstBuffer);
		shadowCastingLights->Delete(lightAddress);

		auto shadowIdxToLightIdxBuffer = (lightType == LightForm::LightType::Spot) ? &_spotLightShadowIdxToLightIdxBuffer : &_directionalLightShadowIdxToLightIdxBuffer;
		shadowIdxToLightIdxBuffer->Delete(lightAddress);
	}
	else
		ASSERT_MSG("Unsupported light type.");

	++_updateConter;
}

bool ShadowRenderer::HasShadowCastingLight(const LightForm*& light)
{
	address lightAddress = reinterpret_cast<address>(light);

	LightForm::LightType lightType = light->GetType();
	if(lightType == LightForm::LightType::Point)
	{
		return _shadowCastingPointLights.Find(lightAddress) != nullptr;
	}
	else if( (lightType == LightForm::LightType::Spot) ||
			 (lightType == LightForm::LightType::Directional) )
	{
		Structure::VectorMap<address, ShadowCastingSpotDirectionalLight>* shadowCastingLights =
			(lightType == LightForm::LightType::Spot) ? &_shadowCastingSpotLights : &_shadowCastingDirectionalLights;

		return shadowCastingLights->Find(lightAddress) != nullptr;
	}

	ASSERT_MSG("Unsupported light type.");
	return false;
}

void ShadowRenderer::UpdateConstBuffer(const Device::DirectX*& dx)
{
#ifndef USE_RENDER_WITH_UPDATE_CB
	// Spot Light
	{
		uint count = _shadowCastingSpotLights.GetSize();
		for(uint index = 0; index < count; ++index)
			UpdateShadowCastingSpotLightCB(dx, index);
	}

	// Point Light
	{
		uint count = _shadowCastingPointLights.GetSize();
		for(uint index = 0; index < count; ++index)
			UpdateShadowCastingPointLightCB(dx, index);
	}

	// Directional Light
	{
		uint count = _shadowCastingDirectionalLights.GetSize();
		for(uint index = 0; index < count; ++index)
			UpdateShadowCastingDirectionalLightCB(dx, index);
	}
#endif

	ShadowGlobalParam param;
	MakeShadowGlobalParam(param);
	bool isDifferent = memcmp(&param, &_prevShadowGlobalParam, sizeof(ShadowGlobalParam)) != 0;

	if(isDifferent)
	{
		_prevShadowGlobalParam = param;
		_shadowGlobalParamCB->UpdateSubResource(dx->GetContext(), &param);
	}
}

void ShadowRenderer::RenderShadowMap(const Device::DirectX*& dx, const RenderManager*& renderManager)
{
	ID3D11DeviceContext* context = dx->GetContext();
#if defined(USE_SHADOW_INVERTED_DEPTH)
	context->OMSetDepthStencilState(dx->GetDepthStateGreater(), 0);
#else
	context->OMSetDepthStencilState(dx->GetDepthStateLess(), 0);
#endif

	if(_shadowCastingSpotLights.GetSize() > 0)
		RenderSpotLightShadowMap(dx, renderManager);
	if(_shadowCastingPointLights.GetSize() > 0)
		RenderPointLightShadowMap(dx, renderManager);
	if(_shadowCastingDirectionalLights.GetSize() > 0)
		RenderDirectionalLightShadowMap(dx, renderManager);
}

ushort ShadowRenderer::FetchShadowCastingLightIndex(const LightForm*& light)
{
	address lightAddress = reinterpret_cast<address>(light);
	LightForm::LightType type = light->GetType();

	uint index = -1;

	if(type == LightForm::LightType::Point)
		_shadowCastingPointLights.Find(lightAddress, &index);
	else if(type == LightForm::LightType::Spot)
		_shadowCastingSpotLights.Find(lightAddress, &index);
	else if(type == LightForm::LightType::Directional)
		_shadowCastingDirectionalLights.Find(lightAddress, &index);
	else
	{
		DEBUG_LOG("Warning, Can not found shadow casting light index.");
	}

	return (ushort)index;
}

uint ShadowRenderer::GetPackedShadowAtlasCapacity() const
{
	uint d	= _numOfShadowCastingDirectionalLightInAtlas	& 0x3FF;
	uint p	= _numOfShadowCastingPointLightInAtlas			& 0x7FF;
	uint s	= _numOfShadowCastingSpotLightInAtlas			& 0x7FF;

	return (p << 21) | (s << 10) | d;
}

void ShadowRenderer::MakeShadowGlobalParam(ShadowGlobalParam& outParam) const
{
	outParam.dummy = 0;
	outParam.packedNumOfShadowAtlasCapacity		= GetPackedShadowAtlasCapacity();

	auto Log2 = [](float f) -> float
	{
		return log(f) / log(2.0f);
	};

	// Compute packedPowerOfTwoShadowResolution
	{
		uint plShadowMapPower = (uint)Log2((float)_pointLightShadowMapResolution);
		plShadowMapPower &= 0x7FF;

		uint slShadowMapPower = (uint)Log2((float)_spotLightShadowMapResolution);
		slShadowMapPower &= 0x7FF;

		uint dlShadowMapPower = (uint)Log2((float)_directionalLightShadowMapResolution);
		dlShadowMapPower &= 0x3FF;

		outParam.packedPowerOfTwoShadowResolution = (plShadowMapPower << 21) | (slShadowMapPower << 10) | dlShadowMapPower;
	}

	// Compute packedNumOfShadows
	{
		uint plCount = _shadowCastingPointLights.GetSize()			& 0x7ff;
		uint slCount = _shadowCastingSpotLights.GetSize()			& 0x7ff;
		uint dlCount = _shadowCastingDirectionalLights.GetSize()	& 0x3ff;

		outParam.packedNumOfShadows = (plCount << 21) | (slCount << 10) | dlCount;
	}
}

bool ShadowRenderer::IsWorking() const
{
	bool has =	_shadowCastingPointLights.GetSize() > 0 ||
				_shadowCastingSpotLights.GetSize() > 0 ||
				_shadowCastingDirectionalLights.GetSize() > 0;

	return has;
}

void ShadowRenderer::UpdateShadowIndexToLightIndexBuffer(const Device::DirectX*& dx)
{
	if( _prevUpdateCounter == _updateConter )
		return;
	
	_prevUpdateCounter = _updateConter;


	ID3D11DeviceContext* context = dx->GetContext();

	uint count = _pointLightShadowIdxToLightIdxBuffer.GetSize();
	const void* data = _pointLightShadowIdxToLightIdxBuffer.GetVector().data();
	_pointLightShadowIdxToLightIdxSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * 4);

	count = _spotLightShadowIdxToLightIdxBuffer.GetSize();
	data = _spotLightShadowIdxToLightIdxBuffer.GetVector().data();
	_spotLightShadowIdxToLightIdxSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * 4);

	count = _directionalLightShadowIdxToLightIdxBuffer.GetSize();
	data = _directionalLightShadowIdxToLightIdxBuffer.GetVector().data();
	_directionalLightShadowIdxToLightIdxSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * 4);
}