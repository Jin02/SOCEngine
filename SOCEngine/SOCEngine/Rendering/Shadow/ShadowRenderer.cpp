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
	_numOfShadowCastingDirectionalLightInAtlas(0)
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
		Matrix& view = cbData.viewMat;
		light->GetOwner()->GetTransform()->FetchWorldMatrix(view);
		CameraForm::GetViewMatrix(view, view);

		Matrix proj;
		Matrix::PerspectiveFovLH(proj, 1.0f, Common::Deg2Rad(light->GetSpotAngleDegree()), light->GetRadius(), 1.0f);

		Matrix& viewProj = cbData.viewProjMat;
		viewProj = view * proj;
	}

	bool isDifferent = memcmp(&shadowCastingLight.prevConstBufferData, &cbData, sizeof(CameraForm::CamConstBufferData)) != 0;
	if(isDifferent)
	{
		shadowCastingLight.prevConstBufferData = cbData;

		Matrix::Transpose(cbData.viewMat,		cbData.viewMat);
		Matrix::Transpose(cbData.viewProjMat,	cbData.viewProjMat);

		ConstBuffer* camConstBuffer = shadowCastingLight.camConstBuffer;
		camConstBuffer->UpdateSubResource(context, &cbData);
	}
}

void ShadowRenderer::UpdateShadowCastingPointLightCB(const Device::DirectX*& dx, uint index)
{
	Vector3 forwards[6] = 
	{
		Vector3( 0.0f,  0.0f,  1.0f),
		Vector3( 0.0f,  0.0f, -1.0f),
		Vector3( 1.0f,  0.0f,  0.0f),
		Vector3(-1.0f,  0.0f,  0.0f),
		Vector3( 0.0f,  1.0f,  0.0f),
		Vector3( 0.0f, -1.0f,  0.0f)
	};
	Vector3 ups[6] = 
	{
		Vector3( 0.0f,  1.0f,  0.0f),
		Vector3( 0.0f,  1.0f,  0.0f),
		Vector3( 0.0f,  1.0f,  0.0f),
		Vector3( 0.0f,  1.0f,  0.0f),
		Vector3( 0.0f,  0.0f, -1.0f),
		Vector3( 0.0f,  0.0f,  1.0f),
	};

	ID3D11DeviceContext* context = dx->GetContext();

	auto& shadowCastingLight = _shadowCastingPointLights.Get(index);
	const PointLight* light = reinterpret_cast<const PointLight*>(shadowCastingLight.lightAddress);

	Matrix proj;
	Matrix::PerspectiveFovLH(proj, 1.0f, Common::Deg2Rad(90.0f), light->GetRadius(), 1.0f);

	auto ComputeCameraConstBufferData = [](CameraForm::CamConstBufferData& out,
		const Vector3& eyePos, const Vector3& forward, const Vector3& up, const Matrix& projMat)
	{
		Matrix& view = out.viewMat;
		{
			Transform tf0(nullptr);
			tf0.UpdatePosition(eyePos);
			tf0.LookAtWorld(eyePos + forward, &up);

			tf0.FetchWorldMatrix(view);
			CameraForm::GetViewMatrix(view, view);
		}

		out.viewProjMat = view * projMat;
	};

	Vector3 worldPos;
	light->GetOwner()->GetTransform()->FetchWorldPosition(worldPos);

	CameraForm::CamConstBufferData  cbData0;
	ComputeCameraConstBufferData(cbData0, worldPos, forwards[0], ups[0], proj);

	//prevConstBuffer is camConstBuffers[0]
	bool isDifferent = memcmp(&shadowCastingLight.prevConstBufferData, &cbData0, sizeof(CameraForm::CamConstBufferData)) != 0;
	if(isDifferent)
	{
		shadowCastingLight.prevConstBufferData = cbData0;

		Matrix::Transpose(cbData0.viewMat,		cbData0.viewMat);
		Matrix::Transpose(cbData0.viewProjMat,	cbData0.viewProjMat);

		ConstBuffer* camConstBuffer = shadowCastingLight.camConstBuffers[0];
		camConstBuffer->UpdateSubResource(context, &cbData0);

		for(uint i=1; i<6; ++i)
		{
			CameraForm::CamConstBufferData cb;
			ComputeCameraConstBufferData(cb, worldPos, forwards[i], ups[i], proj);

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
	const DirectionalLight* light = reinterpret_cast<const DirectionalLight*>(shadowCastingLight.lightAddress);

	CameraForm::CamConstBufferData cbData;
	{
		Matrix& view = cbData.viewMat;
		light->GetOwner()->GetTransform()->FetchWorldMatrix(view);
		Vector3 forward = Vector3(view._13, view._23, view._33);
		view._41 -= forward.x * 50000.0f;
		view._42 -= forward.y * 50000.0f;
		view._43 -= forward.z * 50000.0f;
		CameraForm::GetViewMatrix(view, view);

		Matrix proj;
		Matrix::OrthoLH(proj, (float)_directionalLightShadowMapResolution, (float)_directionalLightShadowMapResolution, 10000.0f, 1.0f);

		Matrix& viewProj = cbData.viewProjMat;
		viewProj = view * proj;
	}

	bool isDifferent = memcmp(&shadowCastingLight.prevConstBufferData, &cbData, sizeof(CameraForm::CamConstBufferData)) != 0;
	if(isDifferent)
	{
		shadowCastingLight.prevConstBufferData = cbData;

		Matrix::Transpose(cbData.viewMat,		cbData.viewMat);
		Matrix::Transpose(cbData.viewProjMat,	cbData.viewProjMat);

		ConstBuffer* camConstBuffer = shadowCastingLight.camConstBuffer;
		camConstBuffer->UpdateSubResource(context, &cbData);
	}
}

void ShadowRenderer::RenderSpotLightShadowMap(const DirectX*& dx, const RenderManager* renderManager)
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

	_spotLightShadowMapAtlas->Clear(context, 0.0f, 0);

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
			MeshCamera::RenderType::DepthOnly,
			camConstBuffer, &intersectFunc);

		context->RSSetState( dx->GetRasterizerStateCWDisableCulling() );

		MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(
			dx, renderManager, alphaTestMeshes,
			MeshCamera::RenderType::AlphaMesh,
			camConstBuffer, &intersectFunc);
		context->RSSetState( nullptr );
	}

	context->RSSetViewports(1, &originViewport);
}

void ShadowRenderer::RenderPointLightShadowMap(const DirectX*& dx, const RenderManager* renderManager)
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

	_pointLightShadowMapAtlas->Clear(context, 0.0f, 0);

	ID3D11RenderTargetView* nullRTV = nullptr;
	context->OMSetRenderTargets(1, &nullRTV, _pointLightShadowMapAtlas->GetDepthStencilView());

	const auto& opaqueMeshes = renderManager->GetOpaqueMeshes();
	const auto& alphaTestMeshes = renderManager->GetOpaqueMeshes();

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
				MeshCamera::RenderType::DepthOnly,
				camConstBuffer, &intersectFunc);

			context->RSSetState( dx->GetRasterizerStateCWDisableCulling() );
			MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(
				dx, renderManager, alphaTestMeshes,
				MeshCamera::RenderType::AlphaMesh,
				camConstBuffer, &intersectFunc);
			context->RSSetState( nullptr );
		}
	}

	context->RSSetViewports(1, &originViewport);
}

void ShadowRenderer::RenderDirectionalLightShadowMap(const DirectX*& dx, const RenderManager* renderManager)
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

	_directionalLightShadowMapAtlas->Clear(context, 0.0f, 0);

	ID3D11RenderTargetView* nullRTV = nullptr;
	context->OMSetRenderTargets(1, &nullRTV, _directionalLightShadowMapAtlas->GetDepthStencilView());

	const auto& opaqueMeshes = renderManager->GetOpaqueMeshes();
	const auto& alphaTestMeshes = renderManager->GetAlphaTestMeshes();

	uint count = _shadowCastingDirectionalLights.GetSize();
	for(uint index = 0; index < count; ++index)
	{
#ifdef USE_RENDER_WITH_UPDATE_CB
		UpdateShadowCastingSpotLightCB(dx, index);
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
			MeshCamera::RenderType::DepthOnly,
			camConstBuffer, &intersectFunc);

		context->RSSetState( dx->GetRasterizerStateCWDisableCulling() );
		MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(
			dx, renderManager, alphaTestMeshes,
			MeshCamera::RenderType::AlphaMesh,
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

void ShadowRenderer::UpdateShadowCastingLightCB(const Device::DirectX*& dx)
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
}

void ShadowRenderer::RenderShadowMap(const Device::DirectX*& dx, const RenderManager* renderManager)
{
	ID3D11DeviceContext* context = dx->GetContext();
	context->OMSetDepthStencilState(dx->GetDepthStateGreater(), 0);

	if(_shadowCastingSpotLights.GetSize() > 0)
		RenderSpotLightShadowMap(dx, renderManager);
	if(_shadowCastingPointLights.GetSize() > 0)
		RenderPointLightShadowMap(dx, renderManager);
	if(_shadowCastingDirectionalLights.GetSize() > 0)
		RenderDirectionalLightShadowMap(dx, renderManager);
}