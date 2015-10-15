#include "ShadowRenderer.h"
#include "Utility.h"
#include "CameraForm.h"
#include "Object.h"

using namespace Structure;
using namespace Math;
using namespace Core;
using namespace Device;
using namespace Rendering::Camera;
using namespace Rendering::Shadow;
using namespace Rendering::Texture;
using namespace Rendering::Light;

ShadowRenderer::ShadowRenderer()
	: _pointLightShadowMap(nullptr), _spotLightShadowMap(nullptr),
	_directionalLightShadowMap(nullptr), _numOfShadowCastingLight(),
	_shadowMapResolution(256)
{
}

ShadowRenderer::~ShadowRenderer()
{
	Destroy();
}

void ShadowRenderer::CreateOrResizeShadowMap(
	const NumOfShadowCastingLight& numOfShadowCastingLight
	)
{
	auto Next2Squre =[](uint value) -> uint
	{
		return 1 << (uint)(ceil(log((uint)value) / log(2.0f)));
	};

	bool changedPLShadowMap = false;
	bool changedSLShadowMap = false;
	bool changedDLShadowMap = false;

	if(_numOfShadowCastingLight.pointLight < numOfShadowCastingLight.pointLight)
	{
		_numOfShadowCastingLight.pointLight = Next2Squre(numOfShadowCastingLight.pointLight);
	}
	if(_numOfShadowCastingLight.spotLight < numOfShadowCastingLight.spotLight)
	{
		_numOfShadowCastingLight.spotLight = Next2Squre(numOfShadowCastingLight.spotLight);
	}
	if(_numOfShadowCastingLight.directionalLight < numOfShadowCastingLight.directionalLight)
	{
		_numOfShadowCastingLight.directionalLight = Next2Squre(numOfShadowCastingLight.directionalLight);
	}

	// Point Light Shadow Map ShadowMap
	{
		SAFE_DELETE(_pointLightShadowMap);

		Size<uint> mapSize;
		mapSize.w = _numOfShadowCastingLight.pointLight * _shadowMapResolution;
		mapSize.h = 6 * _shadowMapResolution; //point light´Â 6¸é ·»´õ¸µ

		_pointLightShadowMap = new DepthBuffer;
		_pointLightShadowMap->Initialize(mapSize, true, 1);
	}

	// Spot Light
	{
		SAFE_DELETE(_spotLightShadowMap);

		Size<uint> mapSize;
		mapSize.w = _numOfShadowCastingLight.spotLight * _shadowMapResolution;
		mapSize.h = _shadowMapResolution;

		_spotLightShadowMap = new DepthBuffer;
		_spotLightShadowMap->Initialize(mapSize, true, 1);
	}

	// dl
	{
		SAFE_DELETE(_directionalLightShadowMap);

		Size<uint> mapSize;
		mapSize.w = _numOfShadowCastingLight.spotLight * _shadowMapResolution;
		mapSize.h = _shadowMapResolution;

		_directionalLightShadowMap = new DepthBuffer;
		_directionalLightShadowMap->Initialize(mapSize, true, 1);
	}
}

void ShadowRenderer::Destroy()
{
	SAFE_DELETE(_pointLightShadowMap);
	SAFE_DELETE(_spotLightShadowMap);
	SAFE_DELETE(_directionalLightShadowMap);
}

void ShadowRenderer::Render(const Device::DirectX* dx)
{
}

void ShadowRenderer::RenderSpotLightShadowMap(const DirectX* dx)
{
	ID3D11DeviceContext* context = dx->GetContext();

	uint vpNum = 1;
	D3D11_VIEWPORT originViewport;
	context->RSGetViewports(&vpNum, &originViewport); 

	D3D11_VIEWPORT viewport;
	{
		viewport.Width		= (float)_shadowMapResolution;
		viewport.Height		= (float)_shadowMapResolution;
		viewport.MaxDepth	= 1.0f;
		viewport.MinDepth	= 0.0f;
		viewport.TopLeftY	= 0.0f;
	}

	_spotLightShadowMap->Clear(context, 1.0f, 0);

	ID3D11RenderTargetView* nullRTV = nullptr;
	context->OMSetRenderTargets(1, &nullRTV, _spotLightShadowMap->GetDepthStencilView());

	const auto& shadowCastingLights = _shadowCastingSpotLights.GetVector();
	uint index = 0;
	for(auto iter = shadowCastingLights.begin(); iter != shadowCastingLights.end(); ++iter, ++index)
	{
		viewport.TopLeftX = (float)(index * _shadowMapResolution);

		const SpotLight* light = reinterpret_cast<const SpotLight*>(iter->lightAddress);

		Matrix proj;
		Matrix::PerspectiveFovLH(proj, Common::Deg2Rad(light->GetSpotAngleDegree()), 1.0f, 1.0f, light->GetRadius());

		Matrix view;
		light->GetOwner()->GetTransform()->FetchWorldMatrix(view);
		CameraForm::GetViewMatrix(view, view);

		Matrix viewProj = view * proj;
	}
}

void ShadowRenderer::RenderPointLightShadowMap(const DirectX* dx)
{
	ID3D11DeviceContext* context = dx->GetContext();

	uint vpNum = 1;
	D3D11_VIEWPORT originViewport;
	context->RSGetViewports(&vpNum, &originViewport); 

	D3D11_VIEWPORT viewport;
	{
		viewport.Width		= (float)_shadowMapResolution;
		viewport.Height		= (float)_shadowMapResolution;
		viewport.MaxDepth	= 1.0f;
		viewport.MinDepth	= 0.0f;
		viewport.TopLeftY	= 0.0f;
	}

	_pointLightShadowMap->Clear(context, 1.0f, 0);

	ID3D11RenderTargetView* nullRTV = nullptr;
	context->OMSetRenderTargets(1, &nullRTV, _pointLightShadowMap->GetDepthStencilView());

	const auto& shadowCastingLights = _shadowCastingPointLights.GetVector();
	uint index = 0;
	for(auto iter = shadowCastingLights.begin(); iter != shadowCastingLights.end(); ++iter, ++index)
	{
		//viewport.TopLeftX = (float)(index * _shadowMapResolution);

		//const PointLight* light = reinterpret_cast<const PointLight*>(iter->lightAddress);

		//Matrix proj;
		//Matrix::PerspectiveFovLH(proj, Common::Deg2Rad(90.0f), 1.0f, 1.0f, light->GetRadius());

		//Matrix view;
		//light->GetOwner()->GetTransform()->FetchWorldMatrix(view);
		//CameraForm::GetViewMatrix(view, view);

		//Matrix viewProj = view * proj;
	}
}

void ShadowRenderer::RenderDirectionalLightShadowMap(const DirectX* dx)
{
}

VectorMap<address, ShadowRenderer::ShadowCastingLight>* 
	ShadowRenderer::GetShadowCastingLights(const LightForm* light)
{
	Structure::VectorMap<address, ShadowCastingLight>* shadowCastingLights = nullptr;

	LightForm::LightType type = light->GetType();
	if(type == LightForm::LightType::Point)
		shadowCastingLights = &_shadowCastingPointLights;
	else if(type == LightForm::LightType::Spot)
		shadowCastingLights = &_shadowCastingSpotLights;
	else if(type == LightForm::LightType::Directional)
		shadowCastingLights = &_shadowCastingDirectionalLights;

	return shadowCastingLights;
}

void ShadowRenderer::AddShadowCastingLight(const LightForm* light)
{
	VectorMap<address, ShadowCastingLight>* shadowCastingLights = GetShadowCastingLights(light);
	ASSERT_COND_MSG(shadowCastingLights, "Unsupported light type.");

	uint lightAddress = reinterpret_cast<address>(light);
	uint currentUpdateCounter = light->GetOwner()->GetTransform()->GetUpdateCounter();

	ShadowCastingLight scl;
	scl.lightAddress = lightAddress;
	scl.updateCounter = currentUpdateCounter;

	shadowCastingLights->Add(lightAddress, scl);
}

void ShadowRenderer::DeleteShadowCastingLight(const LightForm* light)
{
	VectorMap<address, ShadowCastingLight>* shadowCastingLights = GetShadowCastingLights(light);
	ASSERT_COND_MSG(shadowCastingLights, "Unsupported light type.");

	uint lightAddress = reinterpret_cast<address>(light);
	shadowCastingLights->Delete(lightAddress);
}

bool ShadowRenderer::HasShadowCastingLight(const LightForm* light)
{
	VectorMap<address, ShadowCastingLight>* shadowCastingLights = GetShadowCastingLights(light);
	ASSERT_COND_MSG(shadowCastingLights, "Unsupported light type.");

	uint lightAddress = reinterpret_cast<address>(light);
	return shadowCastingLights->Find(lightAddress) != nullptr;
}