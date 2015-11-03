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

ShadowRenderer::ShadowRenderer()
	: _pointLightShadowMapAtlas(nullptr), _spotLightShadowMapAtlas(nullptr),
	_directionalLightShadowMapAtlas(nullptr),
	_pointLightShadowMapResolution(256),
	_spotLightShadowMapResolution(256),
	_directionalLightShadowMapResolution(512),
	_numOfShadowCastingPointLightInAtlas(0),
	_numOfShadowCastingSpotLightInAtlas(0),
	_numOfShadowCastingDirectionalLightInAtlas(0),
	_pointLightShadowBlurSize(2.5f),
	_shadowGlobalParamCB(nullptr)
{
}

ShadowRenderer::~ShadowRenderer()
{
	Destroy();
}

void ShadowRenderer::Initialize(uint numOfShadowCastingPointLight,
								uint numOfShadowCastingSpotLight,
								uint numOfShadowCastingDirectionalLight)
{
	ResizeShadowMapAtlas(	numOfShadowCastingPointLight,
							numOfShadowCastingSpotLight,
							numOfShadowCastingDirectionalLight,
							256,
							256,
							512	);

	_shadowGlobalParamCB = new ConstBuffer;
	_shadowGlobalParamCB->Initialize(sizeof(ShadowGlobalParam));
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
		mapSize.h = 6 * _pointLightShadowMapResolution; //point light´Â 6¸é ·»´õ¸µ

		_pointLightShadowMapAtlas = new DepthBuffer;
		_pointLightShadowMapAtlas->Initialize(mapSize, true, 1);
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
	}
}

void ShadowRenderer::Destroy()
{
	SAFE_DELETE(_shadowGlobalParamCB);

	SAFE_DELETE(_pointLightShadowMapAtlas);
	SAFE_DELETE(_spotLightShadowMapAtlas);
	SAFE_DELETE(_directionalLightShadowMapAtlas);

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
}

void ShadowRenderer::UpdateShadowCastingSpotLightCB(const Device::DirectX*& dx, uint index)
{
	ID3D11DeviceContext* context = dx->GetContext();

	auto& shadowCastingLight = _shadowCastingSpotLights.Get(index);
	const SpotLight* light = reinterpret_cast<const SpotLight*>(shadowCastingLight.lightAddress);

	CameraForm::CamConstBufferData cbData;
	{
		cbData.viewMat = light->GetViewMatrix();
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

void ShadowRenderer::UpdateShadowCastingPointLightCB(const Device::DirectX*& dx, uint index)
{
	ID3D11DeviceContext* context = dx->GetContext();

	auto& shadowCastingLight = _shadowCastingPointLights.Get(index);
	const PointLight* light = reinterpret_cast<const PointLight*>(shadowCastingLight.lightAddress);

	std::array<Matrix, 6> viewMatrices;
	std::array<Matrix, 6> viewProjMatrices;

	light->GetViewMatrices(viewMatrices);
	light->GetViewProjectionMatrices(viewProjMatrices);

	//prevViewProjMat is viewMatrices[0]
	bool isDifferent = memcmp(&shadowCastingLight.prevViewProjMat, &viewProjMatrices[0], sizeof(Matrix)) != 0;
	if(isDifferent)
	{
		shadowCastingLight.prevViewProjMat = viewProjMatrices[0];

		for(uint i=0; i<6; ++i)
		{
			CameraForm::CamConstBufferData cb;
			{
				cb.viewMat = viewMatrices[i];
				cb.viewProjMat = viewProjMatrices[i];
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

	CameraForm::CamConstBufferData cbData;
	{
		cbData.viewMat = light->GetViewMatrix();
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

	ID3D11RenderTargetView* nullRTV = nullptr;
	context->OMSetRenderTargets(1, &nullRTV, _spotLightShadowMapAtlas->GetDepthStencilView());

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
			MeshCamera::RenderType::Forward_DepthOnly,
			camConstBuffer, &intersectFunc);


		context->RSSetState( dx->GetRasterizerStateCWDisableCulling() );

		MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(
			dx, renderManager, alphaTestMeshes,
			MeshCamera::RenderType::Forward_AlphaTest,
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

	ID3D11RenderTargetView* nullRTV = nullptr;
	context->OMSetRenderTargets(1, &nullRTV, _pointLightShadowMapAtlas->GetDepthStencilView());

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
				MeshCamera::RenderType::Forward_DepthOnly,
				camConstBuffer, &intersectFunc);

			context->RSSetState( dx->GetRasterizerStateCWDisableCulling() );
			MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(
				dx, renderManager, alphaTestMeshes,
				MeshCamera::RenderType::Forward_AlphaTest,
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

	ID3D11RenderTargetView* nullRTV = nullptr;
	context->OMSetRenderTargets(1, &nullRTV, _directionalLightShadowMapAtlas->GetDepthStencilView());

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
			MeshCamera::RenderType::Forward_DepthOnly,
			camConstBuffer, &intersectFunc);

		context->RSSetState( dx->GetRasterizerStateCWDisableCulling() );
		MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(
			dx, renderManager, alphaTestMeshes,
			MeshCamera::RenderType::Forward_AlphaTest,
			camConstBuffer, &intersectFunc);
		context->RSSetState( nullptr );
	}

	context->RSSetViewports(1, &originViewport);
}

void ShadowRenderer::AddShadowCastingLight(const LightForm*& light)
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
			scl.camConstBuffers[i]->Initialize(sizeof(CameraForm::CamConstBufferData));
		}

		_shadowCastingPointLights.Add(lightAddress, scl);
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
		scl.camConstBuffer->Initialize(sizeof(CameraForm::CamConstBufferData));

		shadowCastingLights->Add(lightAddress, scl);
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
	}
	else
		ASSERT_MSG("Unsupported light type.");
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

uint ShadowRenderer::GetPackedShadowCastingLightCount() const
{
	uint directionalLightCount	= _shadowCastingDirectionalLights.GetSize()	& 0x3FF;
	uint pointLightCount		= _shadowCastingPointLights.GetSize()		& 0x7FF;
	uint spotLightCount			= _shadowCastingSpotLights.GetSize()		& 0x7FF;

	return (pointLightCount << 21) | (spotLightCount << 10) | directionalLightCount;	
}

void ShadowRenderer::MakeShadowGlobalParam(ShadowGlobalParam& outParam) const
{
	outParam.packedNumOfShadowCastingLights = GetPackedShadowCastingLightCount();

	float plMapRes = (float)_pointLightShadowMapResolution;
	outParam.pointLightTexelOffset		= (plMapRes - (2.0f * _pointLightShadowBlurSize)) / plMapRes;
	outParam.pointLightUnderscanScale	= _pointLightShadowBlurSize / plMapRes;

	outParam.dummy = 0.0f;
}

bool ShadowRenderer::IsWorking() const
{
	bool has =	_shadowCastingPointLights.GetSize() > 0 ||
				_shadowCastingSpotLights.GetSize() > 0 ||
				_shadowCastingDirectionalLights.GetSize() > 0;

	return has;
}