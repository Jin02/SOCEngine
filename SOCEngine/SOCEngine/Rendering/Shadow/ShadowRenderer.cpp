#include "ShadowRenderer.h"
#include "Utility.h"
#include "Object.h"

using namespace Structure;
using namespace Math;
using namespace Core;
using namespace Device;
using namespace Rendering::Buffer;
using namespace Rendering::Camera;
using namespace Rendering::Shadow;
using namespace Rendering::Texture;
using namespace Rendering::Light;

ShadowRenderer::ShadowRenderer()
	: _pointLightShadowMap(nullptr), _spotLightShadowMap(nullptr),
	_directionalLightShadowMap(nullptr), _numOfShadowCastingLight(),
	_pointLightShadowMapResolution(256),
	_spotLightShadowMapResolution(256),
	_directionalLightShadowMapResolution(512)
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
		mapSize.w = _numOfShadowCastingLight.pointLight * _pointLightShadowMapResolution;
		mapSize.h = 6 * _pointLightShadowMapResolution; //point light´Â 6¸é ·»´õ¸µ

		_pointLightShadowMap = new DepthBuffer;
		_pointLightShadowMap->Initialize(mapSize, true, 1);
	}

	// Spot Light
	{
		SAFE_DELETE(_spotLightShadowMap);

		Size<uint> mapSize;
		mapSize.w = _numOfShadowCastingLight.spotLight * _spotLightShadowMapResolution;
		mapSize.h = _spotLightShadowMapResolution;

		_spotLightShadowMap = new DepthBuffer;
		_spotLightShadowMap->Initialize(mapSize, true, 1);
	}

	// dl
	{
		SAFE_DELETE(_directionalLightShadowMap);

		Size<uint> mapSize;
		mapSize.w = _numOfShadowCastingLight.spotLight * _directionalLightShadowMapResolution;
		mapSize.h = _directionalLightShadowMapResolution;

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

void ShadowRenderer::UpdateConstBuffer(const Device::DirectX*& dx)
{

}

void ShadowRenderer::Render(const Device::DirectX*& dx)
{
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
		Matrix::PerspectiveFovLH(proj, Common::Deg2Rad(light->GetSpotAngleDegree()), 1.0f, 1.0f, light->GetRadius());

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
	Matrix::PerspectiveFovLH(proj, Common::Deg2Rad(90.0f), 1.0f, 1.0f, light->GetRadius());

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
		CameraForm::GetViewMatrix(view, view);

		Matrix proj;
		Matrix::OrthoLH(proj, (float)_directionalLightShadowMapResolution, (float)_directionalLightShadowMapResolution, 1.0f, 10000.0f);

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

void ShadowRenderer::RenderSpotLightShadowMap(const DirectX*& dx)
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

	_spotLightShadowMap->Clear(context, 1.0f, 0);

	ID3D11RenderTargetView* nullRTV = nullptr;
	context->OMSetRenderTargets(1, &nullRTV, _spotLightShadowMap->GetDepthStencilView());

	uint count = _shadowCastingSpotLights.GetSize();
	for(uint index = 0; index < count; ++index)
	{		
		UpdateShadowCastingSpotLightCB(dx, index);
		const auto& shadowCastingLight = _shadowCastingSpotLights.Get(index);
	
		viewport.TopLeftX = (float)index * _spotLightShadowMapResolution;
		context->RSSetViewports(1, &viewport);
	}

	context->RSSetViewports(1, &originViewport);
}

void ShadowRenderer::RenderPointLightShadowMap(const DirectX*& dx)
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

	_pointLightShadowMap->Clear(context, 1.0f, 0);

	ID3D11RenderTargetView* nullRTV = nullptr;
	context->OMSetRenderTargets(1, &nullRTV, _pointLightShadowMap->GetDepthStencilView());

	uint count = _shadowCastingDirectionalLights.GetSize();
	for(uint index = 0; index < count; ++index)
	{
		viewport.TopLeftX = (float)index * _pointLightShadowMapResolution;
		UpdateShadowCastingPointLightCB(dx, index);

		for(uint i=0; i<6; ++i)
		{
			viewport.TopLeftY = (float)i * _pointLightShadowMapResolution;
			context->RSSetViewports(1, &viewport);

		}
	}

	context->RSSetViewports(1, &originViewport);
}

void ShadowRenderer::RenderDirectionalLightShadowMap(const DirectX*& dx)
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

	_directionalLightShadowMap->Clear(context, 1.0f, 0);

	ID3D11RenderTargetView* nullRTV = nullptr;
	context->OMSetRenderTargets(1, &nullRTV, _directionalLightShadowMap->GetDepthStencilView());

	uint count = _shadowCastingDirectionalLights.GetSize();
	for(uint index = 0; index < count; ++index)
	{
		UpdateShadowCastingSpotLightCB(dx, index);
		const auto& shadowCastingLight = _shadowCastingDirectionalLights.Get(index);

		viewport.TopLeftX = (float)index * _directionalLightShadowMapResolution;
		context->RSSetViewports(1, &viewport);
	}

	context->RSSetViewports(1, &originViewport);
}

void ShadowRenderer::AddShadowCastingLight(const LightForm*& light)
{
	uint lightAddress = reinterpret_cast<address>(light);

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
}

void ShadowRenderer::DeleteShadowCastingLight(const LightForm*& light)
{
	uint lightAddress = reinterpret_cast<address>(light);

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
	uint lightAddress = reinterpret_cast<address>(light);

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