#include "ShadowRenderer.h"
#include "Utility.h"
#include "Object.h"

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
using namespace Rendering;
using namespace Rendering::Shader;

ShadowRenderer::ShadowRenderer() :
	_pointLightShadowMapAtlas(nullptr), _spotLightShadowMapAtlas(nullptr), _directionalLightShadowMapAtlas(nullptr),
	_pointLightShadowMapResolution(512),
	_spotLightShadowMapResolution(512),
	_directionalLightShadowMapResolution(512),
	_numOfShadowCastingPointLightInAtlas(0),
	_numOfShadowCastingSpotLightInAtlas(0),
	_numOfShadowCastingDirectionalLightInAtlas(0),
	_shadowGlobalParamCB(nullptr),
	_directionalLightShadowParamSRBuffer(nullptr),	_spotLightShadowParamSRBuffer(nullptr),	_pointLightShadowParamSRBuffer(nullptr),
	_directionalLightViewProjMatSRBuffer(nullptr),	_spotLightViewProjMatSRBuffer(nullptr),	_pointLightViewProjMatSRBuffer(nullptr),
	_forceUpdateDL(true), _forceUpdatePL(true), _forceUpdateSL(false)
{
}

ShadowRenderer::~ShadowRenderer()
{
	SAFE_DELETE(_shadowGlobalParamCB);

	SAFE_DELETE(_pointLightShadowMapAtlas);
	SAFE_DELETE(_spotLightShadowMapAtlas);
	SAFE_DELETE(_directionalLightShadowMapAtlas);

	SAFE_DELETE(_pointLightShadowParamSRBuffer);
	SAFE_DELETE(_pointLightViewProjMatSRBuffer);
	SAFE_DELETE(_spotLightShadowParamSRBuffer);
	SAFE_DELETE(_spotLightViewProjMatSRBuffer);
	SAFE_DELETE(_directionalLightShadowParamSRBuffer);
	SAFE_DELETE(_directionalLightViewProjMatSRBuffer);

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

void ShadowRenderer::Initialize(uint numOfShadowCastingPointLight,
								uint numOfShadowCastingSpotLight,
								uint numOfShadowCastingDirectionalLight)
{
	ResizeShadowMapAtlas(	numOfShadowCastingPointLight,
							numOfShadowCastingSpotLight,
							numOfShadowCastingDirectionalLight,
							-1, -1, -1 );

	_shadowGlobalParamCB = new ConstBuffer;
	_shadowGlobalParamCB->Initialize(sizeof(ShadowGlobalParam));

	unsigned __int32 dummyData[POINT_LIGHT_BUFFER_MAX_NUM * 4] = {0, };
	memset(&dummyData, -1, sizeof(dummyData));

	_pointLightShadowParamSRBuffer = new ShaderResourceBuffer;
	_pointLightShadowParamSRBuffer->Initialize(
		sizeof(ShadowCommon::Param), POINT_LIGHT_BUFFER_MAX_NUM,
		DXGI_FORMAT_R32G32B32A32_UINT,
		dummyData, true, 0, D3D11_USAGE_DYNAMIC);

	_directionalLightShadowParamSRBuffer = new ShaderResourceBuffer;
	_directionalLightShadowParamSRBuffer->Initialize(
		sizeof(ShadowCommon::Param), DIRECTIONAL_LIGHT_BUFFER_MAX_NUM,
		DXGI_FORMAT_R32G32B32A32_UINT,
		dummyData, true, 0, D3D11_USAGE_DYNAMIC);

	_spotLightShadowParamSRBuffer = new ShaderResourceBuffer;
	_spotLightShadowParamSRBuffer->Initialize(
		sizeof(ShadowCommon::Param), SPOT_LIGHT_BUFFER_MAX_NUM,
		DXGI_FORMAT_R32G32B32A32_UINT,
		dummyData, true, 0, D3D11_USAGE_DYNAMIC);


	Math::Matrix dummyMat[POINT_LIGHT_BUFFER_MAX_NUM * 6];
	memset(dummyMat, 0, sizeof(dummyMat));

	_directionalLightViewProjMatSRBuffer = new ShaderResourceBuffer;
	_directionalLightViewProjMatSRBuffer->Initialize(
		sizeof(Math::Matrix), DIRECTIONAL_LIGHT_BUFFER_MAX_NUM,
		DXGI_FORMAT_UNKNOWN,
		dummyMat, true, 0, D3D11_USAGE_DYNAMIC);

	_spotLightViewProjMatSRBuffer = new ShaderResourceBuffer;
	_spotLightViewProjMatSRBuffer->Initialize(
		sizeof(Math::Matrix), SPOT_LIGHT_BUFFER_MAX_NUM,
		DXGI_FORMAT_UNKNOWN,
		dummyMat, true, 0, D3D11_USAGE_DYNAMIC);

	_pointLightViewProjMatSRBuffer = new ShaderResourceBuffer;
	_pointLightViewProjMatSRBuffer->Initialize(
		sizeof(Math::Matrix) * 6, POINT_LIGHT_BUFFER_MAX_NUM,
		DXGI_FORMAT_UNKNOWN,
		dummyMat, true, 0, D3D11_USAGE_DYNAMIC);
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
	_forceUpdateDL =
	_forceUpdatePL =
	_forceUpdateSL = true;

	_numOfShadowCastingPointLightInAtlas		=
	_numOfShadowCastingSpotLightInAtlas			=
	_numOfShadowCastingDirectionalLightInAtlas	=
	_pointLightShadowMapResolution				=
	_spotLightShadowMapResolution				=
	_directionalLightShadowMapResolution		= 0;

	_shadowCastingPointLights.DeleteAll();
	_shadowCastingSpotLights.DeleteAll();
	_shadowCastingDirectionalLights.DeleteAll();

	_pointLightShadowParamBuffer.DeleteAll();
	_pointLightViewProjMatBuffer.DeleteAll();

	_spotLightShadowParamBuffer.DeleteAll();
	_spotLightViewProjMatBuffer.DeleteAll();

	_directionalLightShadowParamBuffer.DeleteAll();
	_directionalLightViewProjMatBuffer.DeleteAll();
}

void ShadowRenderer::UpdateShadowCastingSpotLightCB(const Device::DirectX*& dx, uint index)
{
	ID3D11DeviceContext* context = dx->GetContext();

	auto& shadowCastingLight		= _shadowCastingSpotLights.Get(index);
	const LightForm* ownerLight		= shadowCastingLight.shadow->GetOwner();
	const SpotLight* light			= static_cast<const SpotLight*>(ownerLight);
	const SpotLightShadow* shadow	= shadowCastingLight.shadow;

	ShadowMapCB cbData = {shadow->GetViewProjectionMatrix()};
	bool isDifferent = memcmp(&shadowCastingLight.prevViewProjMat, &cbData.viewProjMat, sizeof(Matrix)) != 0;
	if(isDifferent)
	{
		shadowCastingLight.prevViewProjMat = cbData.viewProjMat;

		Matrix::Transpose(cbData.viewProjMat,	cbData.viewProjMat);

		ConstBuffer* camConstBuffer = shadowCastingLight.camConstBuffer;
		camConstBuffer->UpdateSubResource(context, &cbData);
	}
}

void ShadowRenderer::UpdateShadowCastingPointLightCB(const Device::DirectX*& dx, uint index)
{
	ID3D11DeviceContext* context = dx->GetContext();

	auto& shadowCastingLight		= _shadowCastingPointLights.Get(index);
	const PointLightShadow* shadow	= shadowCastingLight.shadow;
	const LightForm* ownerLight		= shadowCastingLight.shadow->GetOwner();
	const PointLight* light			= static_cast<const PointLight*>(ownerLight);

	std::array<Matrix, 6> viewProjMatrices;

	shadow->GetViewProjectionMatrices(viewProjMatrices);

	bool isDifferent = memcmp(&shadowCastingLight.prevViewProjMat, &viewProjMatrices[0], sizeof(Matrix)) != 0;
	if(isDifferent)
	{
		shadowCastingLight.prevViewProjMat = viewProjMatrices[0];

		for(uint i=0; i<6; ++i)
		{
			ShadowMapCB cbData = {viewProjMatrices[i]};
			Matrix::Transpose(cbData.viewProjMat, cbData.viewProjMat);

			ConstBuffer* constBuffer = shadowCastingLight.camConstBuffers[i];
			constBuffer->UpdateSubResource(context, &cbData);
		}
	}
}

void ShadowRenderer::UpdateShadowCastingDirectionalLightCB(const Device::DirectX*& dx, uint index)
{
	ID3D11DeviceContext* context = dx->GetContext();

	auto& shadowCastingLight		= _shadowCastingDirectionalLights.Get(index);
	const LightForm* ownerLight		= shadowCastingLight.shadow->GetOwner();
	const DirectionalLight* light	= static_cast<const DirectionalLight*>(ownerLight);
	
	ShadowMapCB cbData = {light->GetViewProjectionMatrix()};
	bool isDifferent = memcmp(&shadowCastingLight.prevViewProjMat, &cbData.viewProjMat, sizeof(Matrix)) != 0;
	if(isDifferent)
	{
		shadowCastingLight.prevViewProjMat = cbData.viewProjMat;
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

	_spotLightShadowMapAtlas->Clear(context, 0.0f, 0);

	ID3D11RenderTargetView* rtv		= nullptr;
	RenderType opaqueRenderType		= RenderType::Forward_OnlyDepth;
	RenderType alphaBlendRenderType	= RenderType::Forward_AlphaTestWithDiffuse;

	context->OMSetRenderTargets(1, &rtv, _spotLightShadowMapAtlas->GetDepthStencilView());

	const auto& opaqueMeshes = renderManager->GetOpaqueMeshes();
	const auto& alphaTestMeshes = renderManager->GetAlphaTestMeshes();

	uint count = _shadowCastingSpotLights.GetSize();
	for(uint index = 0; index < count; ++index)
	{		
		UpdateShadowCastingSpotLightCB(dx, index);

		const LightForm* ownerLight = _shadowCastingSpotLights.Get(index).shadow->GetOwner();
		const SpotLight* light = static_cast<const SpotLight*>(ownerLight);
		auto IntersectLight = [&](const Sphere& sphere)
		{
			return light->Intersect(sphere);
		};
		std::function<bool(const Sphere&)> intersectFunc = IntersectLight;

		viewport.TopLeftX = (float)index * _spotLightShadowMapResolution;
		context->RSSetViewports(1, &viewport);

		const ConstBuffer* shadowMapCB = _shadowCastingSpotLights.Get(index).camConstBuffer;

		VertexShader::BindConstBuffer(context, ConstBufferBindIndex::OnlyPass, shadowMapCB);
		{
			MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(
				dx, renderManager, opaqueMeshes,
				opaqueRenderType,
				nullptr, &intersectFunc);
	
			context->RSSetState( dx->GetRasterizerStateCWDisableCulling() );
	
			MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(
				dx, renderManager, alphaTestMeshes,
				alphaBlendRenderType,
				nullptr, &intersectFunc);
	
			context->RSSetState( nullptr );
		}
		VertexShader::BindConstBuffer(context, ConstBufferBindIndex::OnlyPass, nullptr);
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

	_pointLightShadowMapAtlas->Clear(context, 0.0f, 0);

	ID3D11RenderTargetView* rtv		= nullptr;
	context->OMSetRenderTargets(1, &rtv, _pointLightShadowMapAtlas->GetDepthStencilView());

	const auto& opaqueMeshes = renderManager->GetOpaqueMeshes();
	const auto& alphaTestMeshes = renderManager->GetAlphaTestMeshes();

	uint count = _shadowCastingPointLights.GetSize();
	for(uint index = 0; index < count; ++index)
	{		
		UpdateShadowCastingPointLightCB(dx, index);

		const LightForm* ownerLight = _shadowCastingPointLights.Get(index).shadow->GetOwner();
		const PointLight* light = static_cast<const PointLight*>(ownerLight);
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

			const ConstBuffer* shadowMapCB = _shadowCastingPointLights.Get(index).camConstBuffers[i];

			VertexShader::BindConstBuffer(context, ConstBufferBindIndex::OnlyPass, shadowMapCB);
			{
				MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(
					dx, renderManager, opaqueMeshes,
					RenderType::Forward_OnlyDepth,
					nullptr, &intersectFunc);

				context->RSSetState( dx->GetRasterizerStateCWDisableCulling() );
				MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(
					dx, renderManager, alphaTestMeshes,
					RenderType::Forward_AlphaTestWithDiffuse,
					nullptr, &intersectFunc);
				context->RSSetState( nullptr );
			}
			VertexShader::BindConstBuffer(context, ConstBufferBindIndex::OnlyPass, nullptr);
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

	_directionalLightShadowMapAtlas->Clear(context, 0.0f, 0);

	ID3D11RenderTargetView* rtv		= nullptr;

	context->OMSetRenderTargets(1, &rtv, _directionalLightShadowMapAtlas->GetDepthStencilView());

	const auto& opaqueMeshes = renderManager->GetOpaqueMeshes();
	const auto& alphaTestMeshes = renderManager->GetAlphaTestMeshes();

	uint count = _shadowCastingDirectionalLights.GetSize();
	for(uint index = 0; index < count; ++index)
	{
		UpdateShadowCastingDirectionalLightCB(dx, index);

		viewport.TopLeftX = (float)index * _directionalLightShadowMapResolution;
		context->RSSetViewports(1, &viewport);

		const LightForm* ownerLight = _shadowCastingDirectionalLights.Get(index).shadow->GetOwner();		
		const DirectionalLight* light = static_cast<const DirectionalLight*>(ownerLight);
		auto IntersectLight = [&](const Sphere& sphere)
		{
			return light->Intersect(sphere);
		};
		std::function<bool(const Sphere&)> intersectFunc = IntersectLight;

		const ConstBuffer* shadowMapCB = _shadowCastingDirectionalLights.Get(index).camConstBuffer;
		VertexShader::BindConstBuffer(context, ConstBufferBindIndex::OnlyPass, shadowMapCB);
		{
			MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(
				dx, renderManager, opaqueMeshes, 
				RenderType::Forward_OnlyDepth,
				nullptr, &intersectFunc);

			context->RSSetState( dx->GetRasterizerStateCWDisableCulling() );
			MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(
				dx, renderManager, alphaTestMeshes,
				RenderType::Forward_AlphaTestWithDiffuse,
				nullptr, &intersectFunc);
			context->RSSetState( nullptr );
		}
		VertexShader::BindConstBuffer(context, ConstBufferBindIndex::OnlyPass, nullptr);
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
		scl.shadow	= static_cast<const PointLight*>(light)->GetShadow();
		for(uint i=0; i<6; ++i)
		{
			scl.camConstBuffers[i] = new ConstBuffer;
			scl.camConstBuffers[i]->Initialize(sizeof(ShadowMapCB));
		}

		_shadowCastingPointLights.Add(lightAddress, scl);
	}
	else if(lightType == LightForm::LightType::Spot)
	{
		if(_shadowCastingSpotLights.Find(lightAddress))
			ASSERT_MSG("Error, Duplicated Light");

		ShadowCastingSpotLight scl;
		scl.shadow			= static_cast<const SpotLight*>(light)->GetShadow();;
		scl.camConstBuffer	= new ConstBuffer;
		scl.camConstBuffer->Initialize(sizeof(ShadowMapCB));

		_shadowCastingSpotLights.Add(lightAddress, scl);
	}
	else if(lightType == LightForm::LightType::Directional)
	{
		if(_shadowCastingDirectionalLights.Find(lightAddress))
			ASSERT_MSG("Error, Duplicated Light");

		ShadowCastingDirectionalLight scl;
		scl.shadow			= static_cast<const DirectionalLight*>(light)->GetShadow();;
		scl.camConstBuffer	= new ConstBuffer;
		scl.camConstBuffer->Initialize(sizeof(ShadowMapCB));

		_shadowCastingDirectionalLights.Add(lightAddress, scl);
	}
	else
		ASSERT_MSG("Unsupported light type.");

	uint plCount = _shadowCastingPointLights.GetSize();
	uint slCount = _shadowCastingSpotLights.GetSize();
	uint dlCount = _shadowCastingDirectionalLights.GetSize();

	ResizeShadowMapAtlas(plCount, slCount, dlCount, _pointLightShadowMapResolution, _spotLightShadowMapResolution, _directionalLightShadowMapResolution);
}

void ShadowRenderer::DeleteShadowCastingLight(const LightForm*& light)
{
#define DELETE_CB(x){if(x == nullptr){return;}SAFE_DELETE(x->camConstBuffer);}

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
		_pointLightShadowParamBuffer.Delete(lightAddress);
		_pointLightViewProjMatBuffer.Delete(lightAddress);

		_forceUpdatePL = true;
	}
	else if(lightType == LightForm::LightType::Spot)
	{
		ShadowCastingDirectionalLight* shadowLight = _shadowCastingDirectionalLights.Find(lightAddress);
		DELETE_CB(shadowLight);

		_shadowCastingDirectionalLights.Delete(lightAddress);
		_spotLightShadowParamBuffer.Delete(lightAddress);
		_spotLightViewProjMatBuffer.Delete(lightAddress);

		_forceUpdateSL = true;
	}
	else if(lightType == LightForm::LightType::Directional)
	{
		ShadowCastingSpotLight* shadowLight = _shadowCastingSpotLights.Find(lightAddress);
		DELETE_CB(shadowLight);

		_shadowCastingSpotLights.Delete(lightAddress);
		_directionalLightShadowParamBuffer.Delete(lightAddress);
		_directionalLightViewProjMatBuffer.Delete(lightAddress);

		_forceUpdateDL = true;
	}
	else
		ASSERT_MSG("Unsupported light type.");
}

bool ShadowRenderer::HasShadowCastingLight(const LightForm*& light)
{
	address lightAddress = reinterpret_cast<address>(light);

	bool has = false;

	LightForm::LightType lightType = light->GetType();

	if(lightType == LightForm::LightType::Point)			has = _shadowCastingPointLights.Find(lightAddress) != nullptr;
	else if(lightType == LightForm::LightType::Spot)		has = _shadowCastingSpotLights.Find(lightAddress) != nullptr;
	else if(lightType == LightForm::LightType::Directional)	has = _shadowCastingDirectionalLights.Find(lightAddress) != nullptr;
	else 
		ASSERT_MSG("Unsupported light type.");

	return has;
}

void ShadowRenderer::UpdateConstBuffer(const Device::DirectX*& dx)
{
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
	context->OMSetDepthStencilState(dx->GetDepthStateGreater(), 0);

	if(_shadowCastingSpotLights.GetSize() > 0)
		RenderSpotLightShadowMap(dx, renderManager);
	if(_shadowCastingPointLights.GetSize() > 0)
		RenderPointLightShadowMap(dx, renderManager);
	if(_shadowCastingDirectionalLights.GetSize() > 0)
		RenderDirectionalLightShadowMap(dx, renderManager);
}

uint ShadowRenderer::FetchShadowIndexInEachShadowLights(const LightForm* light)
{
	uint index = -1;

	if(light)
	{
		address lightAddress = reinterpret_cast<address>(light);
		LightForm::LightType type = light->GetType();

		if(type == LightForm::LightType::Point)
			_shadowCastingPointLights.Find(lightAddress, &index);
		else if(type == LightForm::LightType::Spot)
			_shadowCastingSpotLights.Find(lightAddress, &index);
		else if(type == LightForm::LightType::Directional)
			_shadowCastingDirectionalLights.Find(lightAddress, &index);
		else
		{
			ASSERT_MSG("Warning, Can not found shadow casting light index.");
		}
	}

	return index;
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

Math::Size<uint> ShadowRenderer::GetActivatedDLShadowMapSize() const
{
	return Size<uint>(_directionalLightShadowMapResolution * _shadowCastingDirectionalLights.GetSize(), _directionalLightShadowMapResolution);
}

Math::Size<uint> ShadowRenderer::GetActivatedPLShadowMapSize() const
{
	return Size<uint>(_pointLightShadowMapResolution * _shadowCastingPointLights.GetSize(), _pointLightShadowMapResolution * 6);
}

Math::Size<uint> ShadowRenderer::GetActivatedSLShadowMapSize() const
{
	return Size<uint>(_spotLightShadowMapResolution * _shadowCastingSpotLights.GetSize(), _spotLightShadowMapResolution);
}

void ShadowRenderer::ComputeAllLightViewProj()
{
	// Directional Light는 Light Manager에서 수행함.

	const auto& scsl = _shadowCastingSpotLights.GetVector();
	for(auto iter = scsl.begin(); iter != scsl.end(); ++iter)
		iter->shadow->ComputeViewProjMatrix();

	const auto& scpl = _shadowCastingPointLights.GetVector();
	for(auto iter = scpl.begin(); iter != scpl.end(); ++iter)
		iter->shadow->ComputeViewProjMatrix();
}



struct UpdateParamAndTransformReturn
{
	bool isUpdatedParam;
	bool isUpdatedTransform;
};

template<typename ShadowCastingLightType, typename LightShadow, typename LightShadowParam, typename ViewProjMatrixType>
UpdateParamAndTransformReturn UpdateParamAndTransformBuffer(VectorMap<address, ShadowCastingLightType>& shadowCastingLights,
															VectorMap<address, LightShadowParam>& sclParamBuffer,
															VectorHashMap<address, ViewProjMatrixType>& sclViewProjBuffer,
															std::function<uint(const LightForm*)> getLightIndexInEachLightsFunc)
//															,BufferUpdateType updateType)
{
	//auto CalcStartEndIdx = [](uint& start, uint& end, uint newIdx)
	//{
	//	start	= min(start, newIdx);
	//	end		= max(end, newIdx);
	//};
	//auto UpdateSRBuffer = [](ID3D11DeviceContext* context, ShaderResourceBuffer* srBuffer, const void* inputData, uint bufferElementSize, uint startIdx, uint endIdx)
	//{
	//	uint offset = startIdx * bufferElementSize;
	//	const void* data = (char*)inputData + offset;
	//	uint size = ((endIdx+1) - startIdx) * bufferElementSize;

	//	srBuffer->UpdateResourceUsingMapUnMap(context, data, startIdx * bufferElementSize, size, D3D11_MAP_WRITE_NO_OVERWRITE);
	//};

	//struct SelectiveInfo
	//{
	//	uint changedCount;
	//	uint startIndex;
	//	uint endIndex;
	//	
	//	ReplaceCount() : changedCount(0), startIndex(0), endIndex(0) {}
	//	~ReplaceCount(){}
	//}

	//SelectiveInfo paramIndex, viewProjIndex;

	UpdateParamAndTransformReturn ret;
	ret.isUpdatedParam		= false;
	ret.isUpdatedTransform	= false;

	uint vectorIndex	= 0;
	auto& vector		= shadowCastingLights.GetVector();
	for(auto iter = vector.begin(); iter != vector.end(); ++iter, ++vectorIndex)
	{
		LightShadow* shadow = iter->shadow;
		address key = reinterpret_cast<address>( shadow->GetOwner() );

		// Param
		{
			uint& prevUpdateCounter	= shadowCastingLights.Get(vectorIndex).prevParamUpateCounter;
			uint curUpdateCounter	= iter->shadow->GetParamUpdateCounter();

			if(prevUpdateCounter != curUpdateCounter)
			{
				LightShadow::Param param;
				shadow->MakeParam(param, getLightIndexInEachLightsFunc(shadow->GetOwner()));

				//uint curBufferIndex = 0;

				LightShadow::Param* existParam = sclParamBuffer.Find(key);
				if(existParam == nullptr)
				{
					sclParamBuffer.Add(key, param);
					//curBufferIndex = sclParamBuffer.GetSize() - 1;
				}
				else
				{
					(*existParam) = param;
				}

				//if(updateType == BufferUpdateType::Selective)
				//{
				//	const void* data = sclParamBuffer.GetVector().data() + curBufferIndex;
				//	UpdateSRBuffer(context, ?, data, sizeof(LightShadowParam), curBufferIndex, curBufferIndex);
				//}

				prevUpdateCounter	= curUpdateCounter;
				ret.isUpdatedParam	= true;
			}
		}

		// Transform
		{
			uint& prevUpdateCounter	= shadowCastingLights.Get(vectorIndex).prevTransformUpateCounter;
			uint curUpdateCounter	= iter->shadow->GetOwner()->GetOwner()->GetTransform()->GetUpdateCounter();

			if(prevUpdateCounter != curUpdateCounter)
			{
				ViewProjMatrixType viewProjMat;
				shadow->MakeMatrixParam(viewProjMat);

				//uint curBufferIndex = 0;

				ViewProjMatrixType* existMat = sclViewProjBuffer.Find(key);
				if(existMat == nullptr)
				{
					sclViewProjBuffer.Add(key,	viewProjMat);
					//curBufferIndex = sclViewProjBuffer.GetSize() - 1;
				}
				else
				{
					(*existMat)						= viewProjMat;
				}

				//if(updateType == BufferUpdateType::Selective)
				//{
				//	const void* data = sclViewProjBuffer.GetVector().data() + curBufferIndex;
				//	UpdateSRBuffer(context, ?, data, sizeof(ViewProjMatrixType), curBufferIndex, curBufferIndex);
				//}

				prevUpdateCounter		= curUpdateCounter;
				ret.isUpdatedTransform	= true;
			}
		}
	}

	return ret;
}

void ShadowRenderer::UpdateSRBufferUsingMapDiscard(	const Device::DirectX*& dx,
													std::function<uint(const LightForm*)> getLightIndexInEachLightsFunc)
{
	UpdateParamAndTransformReturn pl, dl, sl;

	pl = UpdateParamAndTransformBuffer<ShadowCastingPointLight, PointLightShadow, PointLightShadow::Param, std::array<Math::Matrix, 6>>
		(_shadowCastingPointLights,
		_pointLightShadowParamBuffer, _pointLightViewProjMatBuffer,
		getLightIndexInEachLightsFunc);

	sl = UpdateParamAndTransformBuffer<ShadowCastingSpotLight, SpotLightShadow, SpotLightShadow::Param, Math::Matrix>
		(_shadowCastingSpotLights,
		_spotLightShadowParamBuffer, _spotLightViewProjMatBuffer,
		getLightIndexInEachLightsFunc);

	dl = UpdateParamAndTransformBuffer<ShadowCastingDirectionalLight, DirectionalLightShadow, DirectionalLightShadow::Param, Math::Matrix>
		(_shadowCastingDirectionalLights,
		_directionalLightShadowParamBuffer, _directionalLightViewProjMatBuffer,
		getLightIndexInEachLightsFunc);

	ID3D11DeviceContext* context = dx->GetContext();

	// Update point light sr buffer
	{
		// 어디서 뭘 사용하던 count의 값은 같다.
		uint count = _pointLightShadowParamBuffer.GetSize();

		if(pl.isUpdatedParam || _forceUpdatePL)
		{
			const void* data = _pointLightShadowParamBuffer.GetVector().data();
			_pointLightShadowParamSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * sizeof(PointLightShadow::Param));
		}

		if(pl.isUpdatedTransform || _forceUpdatePL)
		{
			const void* data = _pointLightViewProjMatBuffer.GetVector().data();
			_pointLightViewProjMatSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * sizeof(Math::Matrix) * 6);
		}

		_forceUpdatePL = false;
	}

	// Update spot light sr buffer
	{
		// 어디서 뭘 사용하던 count의 값은 같다.
		uint count = _spotLightShadowParamBuffer.GetSize();

		if(sl.isUpdatedParam || _forceUpdateSL)
		{
			const void* data = _spotLightShadowParamBuffer.GetVector().data();
			_spotLightShadowParamSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * sizeof(SpotLightShadow::Param));
		}

		if(sl.isUpdatedTransform || _forceUpdateSL)
		{
			const void* data = _spotLightViewProjMatBuffer.GetVector().data();
			_spotLightViewProjMatSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * sizeof(Math::Matrix));
		}

		_forceUpdateSL = false;
	}

	// Update directional light sr buffer
	{
		// 어디서 뭘 사용하던 count의 값은 같다.
		uint count = _directionalLightShadowParamBuffer.GetSize();

		if(dl.isUpdatedParam || _forceUpdateDL)
		{
			const void* data = _directionalLightShadowParamBuffer.GetVector().data();
			_directionalLightShadowParamSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * sizeof(DirectionalLightShadow::Param));
		}

		if(dl.isUpdatedTransform || _forceUpdateDL)
		{
			const void* data = _directionalLightViewProjMatBuffer.GetVector().data();
			_directionalLightViewProjMatSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * sizeof(Math::Matrix));
		}

		_forceUpdateDL = false;
	}
}

void ShadowRenderer::UpdateSRBufferUsingMapNoOverWrite(const Device::DirectX*& dx)
{
	ASSERT_MSG("Deprecated func");
}

void ShadowRenderer::UpdateSRBuffer(	const Device::DirectX*& dx,
					const std::function<uint(const LightForm*)>& getLightIndexInEachLightsFunc)
{
	//D3D_FEATURE_LEVEL level = dx->GetFeatureLevel();

	//if(level >= D3D_FEATURE_LEVEL_11_1)
	//	UpdateBufferUsingMapNoOverWrite(dx->GetContext());
	//else
	UpdateSRBufferUsingMapDiscard(dx, getLightIndexInEachLightsFunc);
}

void ShadowRenderer::BindResources(const Device::DirectX* dx, bool bindVS, bool bindGS, bool bindPS) const
{
	ID3D11DeviceContext* context = dx->GetContext();

	auto BindTextureToVGP = [](ID3D11DeviceContext* context, TextureBindIndex bind, const TextureForm* tex, bool bindVS, bool bindGS, bool bindPS)
	{
		if(bindVS)	VertexShader::BindTexture(context, bind, tex);
		if(bindGS)	GeometryShader::BindTexture(context, bind, tex);
		if(bindPS)	PixelShader::BindTexture(context, bind, tex);
	};
	auto BindSRBufferToVGP = [](ID3D11DeviceContext* context, TextureBindIndex bind, const ShaderResourceBuffer* srBuffer, bool bindVS, bool bindGS, bool bindPS)
	{
		if(bindVS)	VertexShader::BindShaderResourceBuffer(context, bind, srBuffer);
		if(bindGS)	GeometryShader::BindShaderResourceBuffer(context, bind, srBuffer);
		if(bindPS)	PixelShader::BindShaderResourceBuffer(context, bind, srBuffer);
	};

	BindTextureToVGP(context,	TextureBindIndex::PointLightShadowMapAtlas,				_pointLightShadowMapAtlas,				bindVS, bindGS, bindPS);
	BindSRBufferToVGP(context,	TextureBindIndex::PointLightShadowParam,				_pointLightShadowParamSRBuffer,			bindVS, bindGS, bindPS);
	BindSRBufferToVGP(context,	TextureBindIndex::PointLightShadowViewProjMatrix,		_pointLightViewProjMatSRBuffer,			bindVS, bindGS, bindPS);

	BindTextureToVGP(context,	TextureBindIndex::SpotLightShadowMapAtlas,				_spotLightShadowMapAtlas,				bindVS, bindGS, bindPS);
	BindSRBufferToVGP(context,	TextureBindIndex::SpotLightShadowParam,					_spotLightShadowParamSRBuffer,			bindVS, bindGS, bindPS);
	BindSRBufferToVGP(context,	TextureBindIndex::SpotLightShadowViewProjMatrix,		_spotLightViewProjMatSRBuffer,			bindVS, bindGS, bindPS);

	BindTextureToVGP(context,	TextureBindIndex::DirectionalLightShadowMapAtlas,		_directionalLightShadowMapAtlas,		bindVS, bindGS, bindPS);
	BindSRBufferToVGP(context,	TextureBindIndex::DirectionalLightShadowParam,			_directionalLightShadowParamSRBuffer,	bindVS, bindGS, bindPS);
	BindSRBufferToVGP(context,	TextureBindIndex::DirectionalLightShadowViewProjMatrix,	_directionalLightViewProjMatSRBuffer,	bindVS, bindGS, bindPS);

	if(bindVS) VertexShader::BindConstBuffer(context,	ConstBufferBindIndex::ShadowGlobalParam,	_shadowGlobalParamCB);
	if(bindGS) GeometryShader::BindConstBuffer(context,	ConstBufferBindIndex::ShadowGlobalParam,	_shadowGlobalParamCB);
	if(bindPS) PixelShader::BindConstBuffer(context,	ConstBufferBindIndex::ShadowGlobalParam,	_shadowGlobalParamCB);
}

void ShadowRenderer::UnbindResources(const Device::DirectX* dx, bool bindVS, bool bindGS, bool bindPS) const
{
	ID3D11DeviceContext* context = dx->GetContext();

	auto BindTextureToVGP = [](ID3D11DeviceContext* context, TextureBindIndex bind, bool bindVS, bool bindGS, bool bindPS)
	{
		if(bindVS)	VertexShader::BindTexture(context, bind, nullptr);
		if(bindGS)	GeometryShader::BindTexture(context, bind, nullptr);
		if(bindPS)	PixelShader::BindTexture(context, bind, nullptr);
	};
	auto BindSRBufferToVGP = [](ID3D11DeviceContext* context, TextureBindIndex bind, bool bindVS, bool bindGS, bool bindPS)
	{
		if(bindVS)	VertexShader::BindShaderResourceBuffer(context, bind, nullptr);
		if(bindGS)	GeometryShader::BindShaderResourceBuffer(context, bind, nullptr);
		if(bindPS)	PixelShader::BindShaderResourceBuffer(context, bind, nullptr);
	};

	BindTextureToVGP(context,	TextureBindIndex::PointLightShadowMapAtlas,				bindVS, bindGS, bindPS);
	BindSRBufferToVGP(context,	TextureBindIndex::PointLightShadowParam,				bindVS, bindGS, bindPS);
	BindSRBufferToVGP(context,	TextureBindIndex::PointLightShadowViewProjMatrix,		bindVS, bindGS, bindPS);

	BindTextureToVGP(context,	TextureBindIndex::SpotLightShadowMapAtlas,				bindVS, bindGS, bindPS);
	BindSRBufferToVGP(context,	TextureBindIndex::SpotLightShadowParam,					bindVS, bindGS, bindPS);
	BindSRBufferToVGP(context,	TextureBindIndex::SpotLightShadowViewProjMatrix,		bindVS, bindGS, bindPS);

	BindTextureToVGP(context,	TextureBindIndex::DirectionalLightShadowMapAtlas,		bindVS, bindGS, bindPS);
	BindSRBufferToVGP(context,	TextureBindIndex::DirectionalLightShadowParam,			bindVS, bindGS, bindPS);
	BindSRBufferToVGP(context,	TextureBindIndex::DirectionalLightShadowViewProjMatrix,	bindVS, bindGS, bindPS);

	if(bindVS) VertexShader::BindConstBuffer(context,	ConstBufferBindIndex::ShadowGlobalParam,	nullptr);
	if(bindGS) GeometryShader::BindConstBuffer(context,	ConstBufferBindIndex::ShadowGlobalParam,	nullptr);
	if(bindPS) PixelShader::BindConstBuffer(context,	ConstBufferBindIndex::ShadowGlobalParam,	nullptr);
}
