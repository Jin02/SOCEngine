#include "LightManager.h"
#include "Object.h"
#include "EngineMath.h"
#include "Utility.h"
#include "Director.h"

using namespace Device;
using namespace Math;
using namespace std;
using namespace Rendering::Light;
using namespace Rendering::Manager;
using namespace Rendering::Shadow;
using namespace Rendering::Camera;
using namespace Rendering::Buffer;

LightManager::LightManager(void) 
	:	
	_pointLightTransformSRBuffer(nullptr), _pointLightColorSRBuffer(nullptr),

	_directionalLightTransformSRBuffer(nullptr), _directionalLightParamSRBuffer(nullptr), 
	_directionalLightColorSRBuffer(nullptr),

	_spotLightTransformSRBuffer(nullptr), _spotLightColorSRBuffer(nullptr),
	_spotLightParamSRBuffer(nullptr),

	_directionalLightShadowParamSRBuffer(nullptr), _pointLightShadowParamSRBuffer(nullptr), _spotLightShadowParamSRBuffer(nullptr),
	_directionalLightShadowColorSRBuffer(nullptr), _pointLightShadowColorSRBuffer(nullptr), _spotLightShadowColorSRBuffer(nullptr)
{
	_pointLightBufferUpdateType			= BufferUpdateType::Overall;
	_spotLightBufferUpdateType			= BufferUpdateType::Overall;
	_directionalLightBufferUpdateType	= BufferUpdateType::Overall;
}

LightManager::~LightManager(void)
{
	DestroyAllShaderReourceBuffer();
}

void LightManager::InitializeAllShaderResourceBuffer()
{
	const __int32 dummyData[POINT_LIGHT_BUFFER_MAX_NUM * 4] = {0, };

	PointLightShadow::Param dummyShadowParam[POINT_LIGHT_BUFFER_MAX_NUM];
	memset(dummyShadowParam, 0, sizeof(dummyShadowParam));

	// Point Light
	{
		_pointLightTransformSRBuffer = new ShaderResourceBuffer;
		_pointLightTransformSRBuffer->Initialize(
			sizeof(LightForm::LightTransformBuffer), POINT_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			dummyData, true, 0, D3D11_USAGE_DYNAMIC);

		_pointLightColorSRBuffer = new ShaderResourceBuffer;
		_pointLightColorSRBuffer->Initialize(
			4, POINT_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			dummyData, true, 0, D3D11_USAGE_DYNAMIC);

		_pointLightShadowParamSRBuffer		= new ShaderResourceBuffer;
		_pointLightShadowParamSRBuffer->Initialize(
			sizeof(PointLightShadow::Param), POINT_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_UNKNOWN,
			dummyShadowParam, true, 0, D3D11_USAGE_DYNAMIC);

		_pointLightShadowColorSRBuffer = new ShaderResourceBuffer;
		_pointLightShadowColorSRBuffer->Initialize(
			4, POINT_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			dummyData, true, 0, D3D11_USAGE_DYNAMIC);
	}

	// Directional Light Buffer
	{
		_directionalLightTransformSRBuffer	= new ShaderResourceBuffer;
		_directionalLightTransformSRBuffer->Initialize(
			sizeof(LightForm::LightTransformBuffer), DIRECTIONAL_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
		//	_directionalLightTransformBuffer.GetVector().data());
			dummyData, true, 0, D3D11_USAGE_DYNAMIC);

		_directionalLightParamSRBuffer		= new ShaderResourceBuffer;
		_directionalLightParamSRBuffer->Initialize(
			sizeof(DirectionalLight::Params), DIRECTIONAL_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R16G16_FLOAT,
			dummyData, true, 0, D3D11_USAGE_DYNAMIC);

		_directionalLightColorSRBuffer		= new ShaderResourceBuffer;
		_directionalLightColorSRBuffer->Initialize(
			4, DIRECTIONAL_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			dummyData, true, 0, D3D11_USAGE_DYNAMIC);

		_directionalLightShadowParamSRBuffer		= new ShaderResourceBuffer;
		_directionalLightShadowParamSRBuffer->Initialize(
			sizeof(DirectionalLightShadow::Param), DIRECTIONAL_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_UNKNOWN,
			dummyShadowParam, true, 0, D3D11_USAGE_DYNAMIC);

		_directionalLightShadowColorSRBuffer = new ShaderResourceBuffer;
		_directionalLightShadowColorSRBuffer->Initialize(
			4, DIRECTIONAL_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			dummyData, true, 0, D3D11_USAGE_DYNAMIC);
	}

	// Spot Light
	{
		_spotLightTransformSRBuffer			= new ShaderResourceBuffer;
		_spotLightTransformSRBuffer->Initialize(
			sizeof(LightForm::LightTransformBuffer), SPOT_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			dummyData, true, 0, D3D11_USAGE_DYNAMIC);

		_spotLightColorSRBuffer				= new ShaderResourceBuffer;
		_spotLightColorSRBuffer->Initialize(
			4, SPOT_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			dummyData, true, 0, D3D11_USAGE_DYNAMIC);

		_spotLightParamSRBuffer				= new ShaderResourceBuffer;	
		_spotLightParamSRBuffer->Initialize(
			sizeof(SpotLight::Params), SPOT_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R16G16B16A16_FLOAT,
			dummyData, true, 0, D3D11_USAGE_DYNAMIC);

		_spotLightShadowParamSRBuffer		= new ShaderResourceBuffer;
		_spotLightShadowParamSRBuffer->Initialize(
			sizeof(SpotLightShadow::Param), SPOT_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_UNKNOWN,
			dummyShadowParam, true, 0, D3D11_USAGE_DYNAMIC);

		_spotLightShadowColorSRBuffer = new ShaderResourceBuffer;
		_spotLightShadowColorSRBuffer->Initialize(
			4, SPOT_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			dummyData, true, 0, D3D11_USAGE_DYNAMIC);
	}
}

void LightManager::ComputeAllLightViewProj(const Intersection::BoundBox& sceneBoundBox)
{
	const auto& lights = _lights.GetVector();
	for(auto iter = lights.begin(); iter != lights.end(); ++iter)
	{
		LightForm* light = iter->light;
		light->ComputeViewProjMatrix(sceneBoundBox);
	}
}

void LightManager::DestroyAllShaderReourceBuffer()
{
	SAFE_DELETE(_pointLightTransformSRBuffer);
	SAFE_DELETE(_pointLightColorSRBuffer);

	SAFE_DELETE(_directionalLightTransformSRBuffer);
	SAFE_DELETE(_directionalLightParamSRBuffer);
	SAFE_DELETE(_directionalLightColorSRBuffer);

	SAFE_DELETE(_spotLightTransformSRBuffer);
	SAFE_DELETE(_spotLightColorSRBuffer);
	SAFE_DELETE(_spotLightParamSRBuffer);

	SAFE_DELETE(_directionalLightShadowParamSRBuffer);
	SAFE_DELETE(_pointLightShadowParamSRBuffer);
	SAFE_DELETE(_spotLightShadowParamSRBuffer);

	SAFE_DELETE(_directionalLightShadowColorSRBuffer);
	SAFE_DELETE(_pointLightShadowColorSRBuffer);
	SAFE_DELETE(_spotLightShadowColorSRBuffer);
}

void LightManager::Add(LightForm*& light)
{
	address searchKey = reinterpret_cast<address>(light);

	bool found = Has(light);
	ASSERT_COND_MSG(found == false, "Already has Key");

	uint counter = -1;
	_lights.Add(searchKey, Lights(light, counter));
}

void LightManager::UpdateBuffer(const DirectX* dx)
{
	D3D_FEATURE_LEVEL level = dx->GetFeatureLevel();

	if(level >= D3D_FEATURE_LEVEL_11_1)
		UpdateBufferUsingMapNoOverWrite(dx->GetContext());
	else
		UpdateBufferUsingMapDiscard(dx->GetContext());
}

void LightManager::UpdateBufferUsingMapDiscard(ID3D11DeviceContext* context)
{
	bool isUpdatedDL = false;
	bool isUpdatedPL = false;
	bool isUpdatedSL = false;

	uint allLightBufferIdx = 0;
	auto& lights = _lights.GetVector();
	for(auto iter = lights.begin(); iter != lights.end(); ++iter, ++allLightBufferIdx)
	{
		const Light::LightForm* light = iter->light;

		uint& prevTransformUpdateCounter = _lights.Get(allLightBufferIdx).prevTransformUpdateCounter;
		uint currentUpdateCounter = light->GetOwner()->GetTransform()->GetUpdateCounter();

		if(currentUpdateCounter == prevTransformUpdateCounter)
			continue;
		
		prevTransformUpdateCounter = currentUpdateCounter;

		address key = reinterpret_cast<address>(light);

		LightForm::LightType lightType = light->GetType();
		uint uintColor = light->Get32BitMainColor();

		bool useShadow = light->GetUseShadow();
		uint uintShadowColor = useShadow ? light->GetShadowColor().Get32BitUintColor() : 0;

		if(lightType == LightForm::LightType::Directional)
		{			
			const DirectionalLight* dl = dynamic_cast<const DirectionalLight*>(light);

			LightForm::LightTransformBuffer transformElem;
			DirectionalLight::Params param;
			dl->MakeLightBufferElement(transformElem, param);

			DirectionalLightShadow::Param shadowParam;
			if( useShadow )
				dl->GetShadow()->MakeParam(shadowParam);

			LightForm::LightTransformBuffer* transform = _directionalLightTransformBuffer.Find(key);
			if( transform == nullptr ) //하나만 검색해도 됨
			{
				_directionalLightTransformBuffer.Add(key, transformElem);
				_directionalLightParamBuffer.Add(key, param);
				_directionalLightColorBuffer.Add(key, uintColor);

				_directionalLightShadowParamBuffer.Add(key, shadowParam);
				_directionalLightShadowColorBuffer.Add(key, uintShadowColor);
			}
			else
			{
				(*transform) = transformElem;
				(*_directionalLightParamBuffer.Find(key))			= param;
				(*_directionalLightColorBuffer.Find(key))			= uintColor;

				(*_directionalLightShadowParamBuffer.Find(key))		= shadowParam;
				(*_directionalLightShadowColorBuffer.Find(key))		= uintShadowColor;
			}

			isUpdatedDL = true;
		}
		else if(lightType == LightForm::LightType::Point)
		{	
			const PointLight* pl = dynamic_cast<const PointLight*>(light);

			LightForm::LightTransformBuffer transformElem;
			pl->MakeLightBufferElement(transformElem);

			PointLightShadow::Param shadowParam;
			if( useShadow )
				pl->GetShadow()->MakeParam(shadowParam);

			LightForm::LightTransformBuffer* transform = _pointLightTransformBuffer.Find(key);
			if( transform == nullptr)
			{
				_pointLightTransformBuffer.Add(key, transformElem);
				_pointLightColorBuffer.Add(key, uintColor);

				_pointLightShadowParamBuffer.Add(key, shadowParam);
				_pointLightShadowColorBuffer.Add(key, uintShadowColor);
			}
			else
			{
				(*transform) = transformElem;
				(*_pointLightColorBuffer.Find(key))			= uintColor;

				(*_pointLightShadowParamBuffer.Find(key))	= shadowParam;
				(*_pointLightShadowColorBuffer.Find(key))	= uintShadowColor;
			}

			isUpdatedPL = true;
		}
		else if(lightType == LightForm::LightType::Spot)
		{
			const SpotLight* sl = dynamic_cast<const SpotLight*>(light);

			LightForm::LightTransformBuffer transformElem;
			SpotLight::Params param;
			sl->MakeLightBufferElement(transformElem, param);

			SpotLightShadow::Param shadowParam;
			if( useShadow )
				sl->GetShadow()->MakeParam(shadowParam);

			LightForm::LightTransformBuffer* transform = _spotLightTransformBuffer.Find(key);
			if( transform == nullptr )
			{
				_spotLightTransformBuffer.Add(key, transformElem);
				_spotLightParamBuffer.Add(key, param);
				_spotLightColorBuffer.Add(key, uintColor);

				_spotLightShadowParamBuffer.Add(key, shadowParam);
				_spotLightShadowColorBuffer.Add(key, uintShadowColor);
			}
			else
			{
				(*transform) = transformElem;
				(*_spotLightParamBuffer.Find(key))			= param;
				(*_spotLightColorBuffer.Find(key))			= uintColor;

				(*_spotLightShadowParamBuffer.Find(key))	= shadowParam;
				(*_spotLightShadowColorBuffer.Find(key))	= uintShadowColor;
			}

			isUpdatedSL = true;
		}
	}

	if(isUpdatedDL)
	{
		uint count = _directionalLightTransformBuffer.GetVector().size();

		// Transform
		{
			const void* data = _directionalLightTransformBuffer.GetVector().data();
			_directionalLightTransformSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * sizeof(LightForm::LightTransformBuffer));
		}

		// Color
		{
			const void* data = _directionalLightColorBuffer.GetVector().data();
			_directionalLightColorSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * 4);
		}

		// Param
		{
			const void* data = _directionalLightParamBuffer.GetVector().data();
			_directionalLightParamSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * sizeof(DirectionalLight::Params));
		}

		// Shadow
		{
			const void* data = _directionalLightShadowParamBuffer.GetVector().data();
			_directionalLightShadowParamSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * sizeof(DirectionalLightShadow::Param));

			data = _directionalLightShadowColorBuffer.GetVector().data();
			_directionalLightShadowColorSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * 4);
		}
	}

	if(isUpdatedPL)
	{
		uint count = _pointLightTransformBuffer.GetVector().size();

		// Transform
		{
			const void* data = _pointLightTransformBuffer.GetVector().data();
			_pointLightTransformSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * sizeof(LightForm::LightTransformBuffer));
		}

		// Color
		{
			const void* data = _pointLightColorBuffer.GetVector().data();
			_pointLightColorSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * 4);
		}

		// Shadow
		{
			const void* data = _pointLightShadowParamBuffer.GetVector().data();
			_pointLightShadowParamSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * sizeof(PointLightShadow::Param));

			data = _pointLightShadowColorBuffer.GetVector().data();
			_pointLightShadowColorSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * 4);
		}
	}

	if(isUpdatedSL)
	{
		uint count = _spotLightTransformBuffer.GetVector().size();

		// Transform
		{
			const void* data = _spotLightTransformBuffer.GetVector().data();
			_spotLightTransformSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * sizeof(LightForm::LightTransformBuffer));
		}

		// Color
		{
			const void* data = _spotLightColorBuffer.GetVector().data();
			_spotLightColorSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * 4);
		}
		
		// Param
		{
			const void* data = _spotLightParamBuffer.GetVector().data();
			_spotLightParamSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * sizeof(SpotLight::Params));
		}

		// Shadow
		{
			const void* data = _spotLightShadowParamBuffer.GetVector().data();
			_spotLightShadowParamSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * sizeof(SpotLightShadow::Param));

			data = _spotLightShadowColorBuffer.GetVector().data();
			_spotLightShadowColorSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * 4);
		}
	}
}

void LightManager::UpdateBufferUsingMapNoOverWrite(ID3D11DeviceContext* context)
{
	auto CalcStartEndIdx = [](uint& start, uint& end, uint newIdx)
	{
		start = min(start, newIdx);
		end = max(end, newIdx);
	};
	auto UpdateSRBuffer = [](ID3D11DeviceContext* context, ShaderResourceBuffer* srBuffer, const void* inputData, uint bufferElementSize, uint startIdx, uint endIdx)
	{
		uint offset = startIdx * bufferElementSize;
		const void* data = (char*)inputData + offset;
		uint size = ((endIdx+1) - startIdx) * bufferElementSize;

		srBuffer->UpdateResourceUsingMapUnMap(context, data, startIdx * bufferElementSize, size, D3D11_MAP_WRITE_NO_OVERWRITE);
	};

	uint dlChangeCount		= 0;
	uint plChangeCount		= 0;
	uint slChangeCount		= 0;

	uint dlChangeStartIdx	= 0;
	uint plChangeStartIdx	= 0;
	uint slChangeStartIdx	= 0;

	uint dlChangeEndIdx		= 0;
	uint plChangeEndIdx		= 0;
	uint slChangeEndIdx		= 0;

	uint allLightBufferIdx	= 0;

	auto& lights = _lights.GetVector();
	for(auto iter = lights.begin(); iter != lights.end(); ++iter, ++allLightBufferIdx)
	{
		const Light::LightForm* light = iter->light;

		uint& prevTransformUpdateCounter = _lights.Get(allLightBufferIdx).prevTransformUpdateCounter;
		uint currentUpdateCounter = light->GetOwner()->GetTransform()->GetUpdateCounter();

		if(currentUpdateCounter == prevTransformUpdateCounter)
			continue;
		prevTransformUpdateCounter = currentUpdateCounter;

		address key = reinterpret_cast<address>(light);

		LightForm::LightType lightType = light->GetType();
		uint uintColor = light->Get32BitMainColor();

		bool useShadow = light->GetUseShadow();
		uint uintShadowColor = useShadow ? light->GetShadowColor().Get32BitUintColor() : 0;

		if(lightType == LightForm::LightType::Directional)
		{			
			const DirectionalLight* dl = dynamic_cast<const DirectionalLight*>(light);

			LightForm::LightTransformBuffer transformElem;
			DirectionalLight::Params param;
			dl->MakeLightBufferElement(transformElem, param);

			DirectionalLightShadow::Param shadowParam;
			if(useShadow)
				dl->GetShadow()->MakeParam(shadowParam);

			uint lightIdx = 0;
			LightForm::LightTransformBuffer* transform = _directionalLightTransformBuffer.Find(key, &lightIdx);
			if( transform == nullptr ) //하나만 검색해도 됨
			{
				_directionalLightTransformBuffer.Add(key, transformElem);
				_directionalLightParamBuffer.Add(key, param);
				_directionalLightColorBuffer.Add(key, uintColor);

				_directionalLightShadowParamBuffer.Add(key, shadowParam);
				_directionalLightShadowColorBuffer.Add(key, uintShadowColor);

				lightIdx = _directionalLightColorBuffer.GetSize() - 1;
			}
			else
			{
				(*transform) = transformElem;
				(*_directionalLightParamBuffer.Find(key))			= param;
				(*_directionalLightColorBuffer.Find(key))			= uintColor;

				(*_directionalLightShadowParamBuffer.Find(key))		= shadowParam;
				(*_directionalLightShadowColorBuffer.Find(key))		= uintShadowColor;
			}

			if(_directionalLightBufferUpdateType == BufferUpdateType::Selective)
			{
				const void* data = _directionalLightColorBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _directionalLightColorSRBuffer, data, 4, lightIdx, lightIdx);

				data = _directionalLightParamBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _directionalLightParamSRBuffer, data, sizeof(DirectionalLight::Params), lightIdx, lightIdx);

				data = _directionalLightTransformBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _directionalLightTransformSRBuffer, data, sizeof(LightForm::LightTransformBuffer), lightIdx, lightIdx);

				data = _directionalLightShadowParamBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _directionalLightShadowParamSRBuffer, data, sizeof(DirectionalLightShadow::Param), lightIdx, lightIdx);

				data = _directionalLightShadowColorBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _directionalLightShadowColorSRBuffer, data, 4, lightIdx, lightIdx);
			}

			CalcStartEndIdx(dlChangeStartIdx, dlChangeEndIdx, lightIdx);
			++dlChangeCount;
		}
		else if(lightType == LightForm::LightType::Point)
		{
			const PointLight* pl = dynamic_cast<const PointLight*>(light);

			LightForm::LightTransformBuffer transformElem;
			pl->MakeLightBufferElement(transformElem);

			PointLightShadow::Param shadowParam;
			if( useShadow )
				pl->GetShadow()->MakeParam(shadowParam);

			uint lightIdx = 0;
			LightForm::LightTransformBuffer* transform = _pointLightTransformBuffer.Find(key, &lightIdx);
			if( transform == nullptr)
			{
				_pointLightTransformBuffer.Add(key, transformElem);
				_pointLightColorBuffer.Add(key, uintColor);

				_pointLightShadowParamBuffer.Add(key, shadowParam);
				_pointLightShadowColorBuffer.Add(key, uintShadowColor);

				lightIdx = _pointLightColorBuffer.GetSize() - 1;
			}
			else
			{
				(*transform) = transformElem;
				(*_pointLightColorBuffer.Find(key))			= uintColor;
				(*_pointLightShadowParamBuffer.Find(key))	= shadowParam;
				(*_pointLightShadowColorBuffer.Find(key))	= uintShadowColor;
			}

			if(_pointLightBufferUpdateType == BufferUpdateType::Selective)
			{
				const void* data = _pointLightColorBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _pointLightColorSRBuffer, data, 4, lightIdx, lightIdx);

				data = _pointLightTransformBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _pointLightTransformSRBuffer, data, sizeof(LightForm::LightTransformBuffer), lightIdx, lightIdx);

				data = _pointLightShadowParamBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _pointLightShadowParamSRBuffer, data, sizeof(PointLightShadow::Param), lightIdx, lightIdx);

				data = _pointLightShadowColorBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _pointLightShadowColorSRBuffer, data, 4, lightIdx, lightIdx);
			}

			CalcStartEndIdx(plChangeStartIdx, plChangeEndIdx, lightIdx);
			++plChangeCount;
		}
		else if(lightType == LightForm::LightType::Spot)
		{
			const SpotLight* sl = dynamic_cast<const SpotLight*>(light);

			LightForm::LightTransformBuffer transformElem;
			SpotLight::Params param;
			sl->MakeLightBufferElement(transformElem, param);

			SpotLightShadow::Param shadowParam;
			if( useShadow )
				sl->GetShadow()->MakeParam(shadowParam);

			uint lightIdx = 0;
			LightForm::LightTransformBuffer* transform = _spotLightTransformBuffer.Find(key, &lightIdx);
			if( transform == nullptr)
			{
				_spotLightTransformBuffer.Add(key, transformElem);
				_spotLightParamBuffer.Add(key, param);
				_spotLightColorBuffer.Add(key, uintColor);
				_spotLightShadowParamBuffer.Add(key, shadowParam);
				_spotLightShadowColorBuffer.Add(key, uintShadowColor);

				lightIdx = _spotLightColorBuffer.GetSize() - 1;
			}
			else
			{
				(*transform) = transformElem;
				(*_spotLightParamBuffer.Find(key))			= param;
				(*_spotLightColorBuffer.Find(key))			= uintColor;
				(*_spotLightShadowParamBuffer.Find(key))	= shadowParam;
				(*_spotLightShadowColorBuffer.Find(key))	= uintShadowColor;
			}

			if(_spotLightBufferUpdateType == BufferUpdateType::Selective)
			{
				const void* data = _spotLightColorBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _spotLightColorSRBuffer, data, 4, lightIdx, lightIdx);

				data = _spotLightParamBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _spotLightParamSRBuffer, data, sizeof(SpotLight::Params), lightIdx, lightIdx);

				data = _spotLightTransformBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _spotLightTransformSRBuffer, data, sizeof(LightForm::LightTransformBuffer), lightIdx, lightIdx);

				data = _spotLightShadowParamBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _spotLightShadowParamSRBuffer, data, sizeof(SpotLightShadow::Param), lightIdx, lightIdx);

				data = _spotLightShadowColorBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _spotLightShadowColorSRBuffer, data, 4, lightIdx, lightIdx);
			}

			CalcStartEndIdx(slChangeStartIdx, slChangeEndIdx, lightIdx);
			++slChangeCount;
		}

		prevTransformUpdateCounter = currentUpdateCounter;
	}

	if(_directionalLightBufferUpdateType == BufferUpdateType::Overall &&
		dlChangeCount > 0)
	{
			UpdateSRBuffer(context, _directionalLightColorSRBuffer,
				_directionalLightColorBuffer.GetVector().data(),
				4, 
				dlChangeStartIdx, dlChangeEndIdx);

			UpdateSRBuffer(context, _directionalLightParamSRBuffer,
				_directionalLightParamBuffer.GetVector().data(),
				sizeof(DirectionalLight::Params),
				dlChangeStartIdx, dlChangeEndIdx);

			UpdateSRBuffer(context, _directionalLightTransformSRBuffer, 
				_directionalLightTransformBuffer.GetVector().data(),
				sizeof(LightForm::LightTransformBuffer),
				dlChangeStartIdx, dlChangeEndIdx);

			UpdateSRBuffer(context, _directionalLightShadowParamSRBuffer, 
				_directionalLightShadowParamBuffer.GetVector().data(),
				sizeof(DirectionalLightShadow::Param),
				dlChangeStartIdx, dlChangeEndIdx);

			UpdateSRBuffer(context, _directionalLightShadowColorSRBuffer, 
				_directionalLightShadowColorBuffer.GetVector().data(),
				4,
				dlChangeStartIdx, dlChangeEndIdx);
	}

	if(_pointLightBufferUpdateType == BufferUpdateType::Overall &&
		plChangeCount > 0)
	{
		UpdateSRBuffer(context, _pointLightTransformSRBuffer, 
			_pointLightTransformBuffer.GetVector().data(),
			sizeof(LightForm::LightTransformBuffer),
			plChangeStartIdx, plChangeEndIdx);

		UpdateSRBuffer(context, _pointLightColorSRBuffer,
			_pointLightColorBuffer.GetVector().data(),
			4, 
			plChangeStartIdx, plChangeEndIdx);

		UpdateSRBuffer(context, _pointLightShadowParamSRBuffer, 
			_pointLightShadowParamBuffer.GetVector().data(),
			sizeof(PointLightShadow::Param),
			plChangeStartIdx, plChangeEndIdx);

		UpdateSRBuffer(context, _pointLightShadowColorSRBuffer, 
			_pointLightShadowColorBuffer.GetVector().data(),
			4,
			plChangeStartIdx, plChangeEndIdx);
	}

	if(_spotLightBufferUpdateType == BufferUpdateType::Overall &&
		slChangeCount > 0)
	{
		UpdateSRBuffer(context, _spotLightColorSRBuffer,
			_spotLightColorBuffer.GetVector().data(),
			4, 
			slChangeStartIdx, slChangeEndIdx);

		UpdateSRBuffer(context, _spotLightParamSRBuffer,
			_spotLightParamBuffer.GetVector().data(),
			sizeof(SpotLight::Params),
			slChangeStartIdx, slChangeEndIdx);

		UpdateSRBuffer(context, _spotLightTransformSRBuffer, 
			_spotLightTransformBuffer.GetVector().data(),
			sizeof(LightForm::LightTransformBuffer),
			slChangeStartIdx, slChangeEndIdx);

		UpdateSRBuffer(context, _spotLightShadowParamSRBuffer, 
			_spotLightShadowParamBuffer.GetVector().data(),
			sizeof(SpotLightShadow::Param),
			slChangeStartIdx, slChangeEndIdx);

		UpdateSRBuffer(context, _spotLightShadowColorSRBuffer, 
			_spotLightShadowColorBuffer.GetVector().data(),
			4,
			slChangeStartIdx, slChangeEndIdx);
	}


	// transform, color, param 중,
	// 뭘 사용하든 갯수는 같다.

	// 다음 버퍼 업데이트 타입 계산
	// 지정한 갯수보다 변경된 횟수가 많으면 MapUnMap을 한번만 사용하는 대신 마지막에 버퍼를 업데이트한다.
	// 그게 아니라면, CPU에 버퍼 내용을 업데이트 하면서 MapUnMap을 통해 버퍼를 업데이트 한다. (MapUnMap 여러번)
	{
		uint standard = (uint)((float)_directionalLightColorBuffer.GetSize() * LIGHT_BUFFER_OVERALL_UPDATE_RATE);
		_directionalLightBufferUpdateType = dlChangeCount > standard ? BufferUpdateType::Overall : BufferUpdateType::Selective;
	
		standard = (uint)((float)_pointLightColorBuffer.GetSize() * LIGHT_BUFFER_OVERALL_UPDATE_RATE);
		_pointLightBufferUpdateType = plChangeCount > standard ? BufferUpdateType::Overall : BufferUpdateType::Selective;

		standard = (uint)((float)_spotLightColorBuffer.GetSize() * LIGHT_BUFFER_OVERALL_UPDATE_RATE);
		_spotLightBufferUpdateType = slChangeCount > standard ? BufferUpdateType::Overall : BufferUpdateType::Selective;
	}
}

void LightManager::Delete(const LightForm*& inputLight)
{
	address key = reinterpret_cast<address>(inputLight);

	const Light::LightForm* light = _lights.Find(key)->light;
	Light::LightForm::LightType type = light->GetType();

	if(type == LightForm::LightType::Directional)
	{
		_directionalLightTransformBuffer.Delete(key);
		_directionalLightParamBuffer.Delete(key);
		_directionalLightColorBuffer.Delete(key);

		_directionalLightShadowParamBuffer.Delete(key);
		_directionalLightShadowColorBuffer.Delete(key);
	}
	else if(type == LightForm::LightType::Point)
	{
		_pointLightTransformBuffer.Delete(key);
		_pointLightColorBuffer.Delete(key);

		_pointLightShadowParamBuffer.Delete(key);
		_pointLightShadowColorBuffer.Delete(key);
	}
	else if(type == LightForm::LightType::Spot)
	{
		_spotLightTransformBuffer.Delete(key);
		_spotLightColorBuffer.Delete(key);
		_spotLightParamBuffer.Delete(key);

		_spotLightShadowParamBuffer.Delete(key);
		_spotLightShadowColorBuffer.Delete(key);
	}

	_lights.Delete(key);
}

void LightManager::DeleteAll()
{
	_spotLightTransformBuffer.DeleteAll();
	_spotLightColorBuffer.DeleteAll();
	_spotLightParamBuffer.DeleteAll();
	_spotLightShadowColorBuffer.DeleteAll();
	_spotLightShadowParamBuffer.DeleteAll();

	_pointLightTransformBuffer.DeleteAll();
	_pointLightColorBuffer.DeleteAll();
	_pointLightShadowColorBuffer.DeleteAll();
	_pointLightShadowParamBuffer.DeleteAll();
	
	_directionalLightColorBuffer.DeleteAll();
	_directionalLightParamBuffer.DeleteAll();
	_directionalLightTransformBuffer.DeleteAll();
	_directionalLightShadowColorBuffer.DeleteAll();
	_directionalLightShadowParamBuffer.DeleteAll();

	_lights.DeleteAll();
}

bool LightManager::Has(LightForm*& light) const
{
	address key = reinterpret_cast<address>(light);
	return _lights.Has(key);
}

uint LightManager::GetLightIndexInEachLights(const LightForm*& inputLight) const
{
	address key = reinterpret_cast<address>(inputLight);

	const Light::LightForm* light		= _lights.Find(key)->light;
	Light::LightForm::LightType type	= light->GetType();

	uint res = -1;

	if(type == LightForm::LightType::Directional)	_directionalLightColorBuffer.Find(key, &res);
	else if(type == LightForm::LightType::Point)	_pointLightColorBuffer.Find(key, &res);
	else if(type == LightForm::LightType::Spot)		_spotLightColorBuffer.Find(key, &res);

	return res;
}

uint LightManager::GetPackedLightCount() const
{
	// transform, color, param 중,
	// 뭘 사용하든 크기는 같다.
	uint directionalLightCount	= _directionalLightColorBuffer.GetSize() & 0x3FF;
	uint pointLightCount		= _pointLightColorBuffer.GetSize() & 0x7FF;
	uint spotLightCount			= _spotLightColorBuffer.GetSize() & 0x7FF;

	return (pointLightCount << 21) | (spotLightCount << 10) | directionalLightCount;	
}