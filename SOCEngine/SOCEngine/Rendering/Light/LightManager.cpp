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
using namespace Rendering::Camera;
using namespace Rendering::Buffer;

LightManager::LightManager(void) 
	: _pointLightTransformBufferSR(nullptr), _pointLightColorBufferSR(nullptr),
	_directionalLightTransformBufferSR(nullptr), _directionalLightParamBufferSR(nullptr),
	_directionalLightColorBufferSR(nullptr), _spotLightTransformBufferSR(nullptr),
	_spotLightColorBufferSR(nullptr), _spotLightParamBufferSR(nullptr)
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
	const int dummyData[POINT_LIGHT_BUFFER_MAX_NUM] = {0, };

	// Point Light
	{
		_pointLightTransformBufferSR = new ShaderResourceBuffer;
		_pointLightTransformBufferSR->Initialize(
			sizeof(LightForm::LightTransformBuffer), POINT_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			dummyData, true, D3D11_USAGE_DYNAMIC);

		_pointLightColorBufferSR = new ShaderResourceBuffer;
		_pointLightColorBufferSR->Initialize(
			4, POINT_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			dummyData, true, D3D11_USAGE_DYNAMIC);
	}

	// Directional Light Buffer
	{
		_directionalLightTransformBufferSR	= new ShaderResourceBuffer;
		_directionalLightTransformBufferSR->Initialize(
			sizeof(LightForm::LightTransformBuffer), DIRECTIONAL_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
		//	_directionalLightTransformBuffer.GetVector().data());
			dummyData, true, D3D11_USAGE_DYNAMIC);

		_directionalLightParamBufferSR		= new ShaderResourceBuffer;
		_directionalLightParamBufferSR->Initialize(
			sizeof(DirectionalLight::Params), DIRECTIONAL_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R16G16_FLOAT,
			dummyData, true, D3D11_USAGE_DYNAMIC);

		_directionalLightColorBufferSR		= new ShaderResourceBuffer;
		_directionalLightColorBufferSR->Initialize(
			4, DIRECTIONAL_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			dummyData, true, D3D11_USAGE_DYNAMIC);
	}

	// Spot Light
	{
		_spotLightTransformBufferSR			= new ShaderResourceBuffer;
		_spotLightTransformBufferSR->Initialize(
			sizeof(LightForm::LightTransformBuffer), SPOT_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			dummyData, true, D3D11_USAGE_DYNAMIC);

		_spotLightColorBufferSR				= new ShaderResourceBuffer;
		_spotLightColorBufferSR->Initialize(
			4, SPOT_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			dummyData, true, D3D11_USAGE_DYNAMIC);

		_spotLightParamBufferSR				= new ShaderResourceBuffer;	
		_spotLightParamBufferSR->Initialize(
			sizeof(SpotLight::Params), SPOT_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R16G16B16A16_FLOAT,
			dummyData, true, D3D11_USAGE_DYNAMIC);
	}
}

void LightManager::DestroyAllShaderReourceBuffer()
{
	SAFE_DELETE(_pointLightTransformBufferSR);
	SAFE_DELETE(_pointLightColorBufferSR);

	SAFE_DELETE(_directionalLightTransformBufferSR);
	SAFE_DELETE(_directionalLightParamBufferSR);
	SAFE_DELETE(_directionalLightColorBufferSR);

	SAFE_DELETE(_spotLightTransformBufferSR);
	SAFE_DELETE(_spotLightColorBufferSR);
	SAFE_DELETE(_spotLightParamBufferSR);
}

void LightManager::Add(const LightForm* light, const char* key)
{
	std::string searchKey = key ? key : light->GetOwner()->GetName();

	bool found = HasKey(searchKey);
	ASSERT_COND_MSG(found == false, "Already has Key");

	//1을 더하는건, UpdateBuffer에서 비교할때 버그 안생기게 하려고 -ㅠ-..
	uint counter = light->GetOwner()->GetTransform()->GetUpdateCounter() + 1;
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
		
		std::string key = light->GetOwner()->GetName();

		LightForm::LightType lightType = light->GetType();
		uint uintColor = light->GetColor().Get32BitUintColor();

		if(lightType == LightForm::LightType::Directional)
		{			
			const DirectionalLight* l = dynamic_cast<const DirectionalLight*>(light);

			LightForm::LightTransformBuffer transformElem;
			DirectionalLight::Params param;

			l->MakeLightBufferElement(transformElem, param);

			LightForm::LightTransformBuffer* transform = _directionalLightTransformBuffer.Find(key);
			if( transform == nullptr ) //하나만 검색해도 됨
			{
				_directionalLightTransformBuffer.Add(key, transformElem);
				_directionalLightParamBuffer.Add(key, param);
				_directionalLightColorBuffer.Add(key, uintColor);
			}
			else
			{
				(*transform) = transformElem;
				(*_directionalLightParamBuffer.Find(key)) = param;
				(*_directionalLightColorBuffer.Find(key)) = uintColor;
			}

			isUpdatedDL = true;
		}
		else if(lightType == LightForm::LightType::Point)
		{
			const PointLight* l = dynamic_cast<const PointLight*>(light);

			LightForm::LightTransformBuffer transformElem;
			l->MakeLightBufferElement(transformElem);

			LightForm::LightTransformBuffer* transform = _pointLightTransformBuffer.Find(key);
			if( transform == nullptr)
			{
				_pointLightTransformBuffer.Add(key, transformElem);
				_pointLightColorBuffer.Add(key, uintColor);
			}
			else
			{
				(*transform) = transformElem;
				(*_pointLightColorBuffer.Find(key)) = uintColor;
			}

			isUpdatedPL = true;
		}
		else if(lightType == LightForm::LightType::Spot)
		{
			const SpotLight* l = dynamic_cast<const SpotLight*>(light);

			LightForm::LightTransformBuffer transformElem;
			SpotLight::Params param;

			l->MakeLightBufferElement(transformElem, param);

			LightForm::LightTransformBuffer* transform = _spotLightTransformBuffer.Find(key);
			if( transform == nullptr )
			{
				_spotLightTransformBuffer.Add(key, transformElem);
				_spotLightParamBuffer.Add(key, param);
				_spotLightColorBuffer.Add(key, uintColor);
			}
			else
			{
				(*transform) = transformElem;
				(*_spotLightParamBuffer.Find(key)) = param;
				(*_spotLightColorBuffer.Find(key)) = uintColor;
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
			_directionalLightColorBufferSR->UpdateResourceUsingMapUnMap(context, data, count * sizeof(LightForm::LightTransformBuffer));
		}

		// Color
		{
			const void* data = _directionalLightColorBuffer.GetVector().data();
			_directionalLightColorBufferSR->UpdateResourceUsingMapUnMap(context, data, count * 4);
		}

		// Param
		{
			const void* data = _directionalLightParamBuffer.GetVector().data();
			_directionalLightParamBufferSR->UpdateResourceUsingMapUnMap(context, data, count * sizeof(DirectionalLight::Params));
		}		
	}

	if(isUpdatedPL)
	{
		uint count = _pointLightTransformBuffer.GetVector().size();

		// Transform
		{
			const void* data = _pointLightTransformBuffer.GetVector().data();
			_pointLightColorBufferSR->UpdateResourceUsingMapUnMap(context, data, count * sizeof(LightForm::LightTransformBuffer));
		}

		// Color
		{
			const void* data = _pointLightColorBuffer.GetVector().data();
			_pointLightColorBufferSR->UpdateResourceUsingMapUnMap(context, data, count * 4);
		}
	}

	if(isUpdatedSL)
	{
		uint count = _spotLightTransformBuffer.GetVector().size();

		// Transform
		{
			const void* data = _spotLightTransformBuffer.GetVector().data();
			_spotLightColorBufferSR->UpdateResourceUsingMapUnMap(context, data, count * sizeof(LightForm::LightTransformBuffer));
		}

		// Color
		{
			const void* data = _spotLightColorBuffer.GetVector().data();
			_spotLightColorBufferSR->UpdateResourceUsingMapUnMap(context, data, count * 4);
		}

		// Param
		{
			const void* data = _spotLightParamBuffer.GetVector().data();
			_spotLightParamBufferSR->UpdateResourceUsingMapUnMap(context, data, count * sizeof(SpotLight::Params));
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

		std::string key = light->GetOwner()->GetName();

		LightForm::LightType lightType = light->GetType();
		uint uintColor = light->GetColor().Get32BitUintColor();

		if(lightType == LightForm::LightType::Directional)
		{			
			const DirectionalLight* l = dynamic_cast<const DirectionalLight*>(light);

			LightForm::LightTransformBuffer transformElem;
			DirectionalLight::Params param;

			l->MakeLightBufferElement(transformElem, param);

			uint lightIdx = 0;
			LightForm::LightTransformBuffer* transform = _directionalLightTransformBuffer.Find(key, &lightIdx);
			if( transform == nullptr ) //하나만 검색해도 됨
			{
				_directionalLightTransformBuffer.Add(key, transformElem);
				_directionalLightParamBuffer.Add(key, param);
				_directionalLightColorBuffer.Add(key, uintColor);

				lightIdx = _directionalLightColorBuffer.GetSize() - 1;
			}
			else
			{
				(*transform) = transformElem;
				(*_directionalLightParamBuffer.Find(key)) = param;
				(*_directionalLightColorBuffer.Find(key)) = uintColor;
			}

			if(_directionalLightBufferUpdateType == BufferUpdateType::Selective)
			{
				const void* data = _directionalLightColorBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _directionalLightColorBufferSR, data, 4, lightIdx, lightIdx);

				data = _directionalLightParamBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _directionalLightParamBufferSR, data, sizeof(DirectionalLight::Params), lightIdx, lightIdx);

				data = _directionalLightColorBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _directionalLightTransformBufferSR, data, sizeof(LightForm::LightTransformBuffer), lightIdx, lightIdx);
			}

			CalcStartEndIdx(dlChangeStartIdx, dlChangeEndIdx, lightIdx);
			++dlChangeCount;
		}
		else if(lightType == LightForm::LightType::Point)
		{
			const PointLight* l = dynamic_cast<const PointLight*>(light);

			LightForm::LightTransformBuffer transformElem;
			l->MakeLightBufferElement(transformElem);

			uint lightIdx = 0;
			LightForm::LightTransformBuffer* transform = _pointLightTransformBuffer.Find(key, &lightIdx);
			if( transform == nullptr)
			{
				_pointLightTransformBuffer.Add(key, transformElem);
				_pointLightColorBuffer.Add(key, uintColor);

				lightIdx = _pointLightColorBuffer.GetSize() - 1;
			}
			else
			{
				(*transform) = transformElem;
				(*_pointLightColorBuffer.Find(key)) = uintColor;
			}

			if(_pointLightBufferUpdateType == BufferUpdateType::Selective)
			{
				const void* data = _pointLightColorBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _pointLightColorBufferSR, data, 4, lightIdx, lightIdx);

				data = _pointLightTransformBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _pointLightTransformBufferSR, data, sizeof(LightForm::LightTransformBuffer), lightIdx, lightIdx);
			}

			CalcStartEndIdx(plChangeStartIdx, plChangeEndIdx, lightIdx);
			++plChangeCount;
		}
		else if(lightType == LightForm::LightType::Spot)
		{
			const SpotLight* l = dynamic_cast<const SpotLight*>(light);

			LightForm::LightTransformBuffer transformElem;
			SpotLight::Params param;

			l->MakeLightBufferElement(transformElem, param);

			uint lightIdx = 0;
			LightForm::LightTransformBuffer* transform = _spotLightTransformBuffer.Find(key, &lightIdx);
			if( transform == nullptr)
			{
				_spotLightTransformBuffer.Add(key, transformElem);
				_spotLightParamBuffer.Add(key, param);
				_spotLightColorBuffer.Add(key, uintColor);

				lightIdx = _spotLightColorBuffer.GetSize() - 1;
			}
			else
			{
				(*transform) = transformElem;
				(*_spotLightParamBuffer.Find(key)) = param;
				(*_spotLightColorBuffer.Find(key)) = uintColor;
			}

			if(_spotLightBufferUpdateType == BufferUpdateType::Selective)
			{
				const void* data = _spotLightColorBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _spotLightColorBufferSR, data, 4, lightIdx, lightIdx);

				data = _spotLightParamBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _spotLightParamBufferSR, data, sizeof(SpotLight::Params), lightIdx, lightIdx);

				data = _spotLightTransformBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _spotLightTransformBufferSR, data, sizeof(LightForm::LightTransformBuffer), lightIdx, lightIdx);
			}

			CalcStartEndIdx(slChangeStartIdx, slChangeEndIdx, lightIdx);
			++slChangeCount;
		}

		prevTransformUpdateCounter = currentUpdateCounter;
	}

	if(_directionalLightBufferUpdateType == BufferUpdateType::Overall &&
		dlChangeCount > 0)
	{
			UpdateSRBuffer(context, _directionalLightColorBufferSR,
				_directionalLightColorBuffer.GetVector().data(),
				4, 
				dlChangeStartIdx, dlChangeEndIdx);

			UpdateSRBuffer(context, _directionalLightParamBufferSR,
				_directionalLightParamBuffer.GetVector().data(),
				sizeof(DirectionalLight::Params),
				dlChangeStartIdx, dlChangeEndIdx);

			UpdateSRBuffer(context, _directionalLightTransformBufferSR, 
				_directionalLightTransformBuffer.GetVector().data(),
				sizeof(LightForm::LightTransformBuffer),
				dlChangeStartIdx, dlChangeEndIdx);
	}

	if(_pointLightBufferUpdateType == BufferUpdateType::Overall &&
		plChangeCount > 0)
	{
		UpdateSRBuffer(context, _pointLightTransformBufferSR, 
			_pointLightTransformBuffer.GetVector().data(),
			sizeof(LightForm::LightTransformBuffer),
			plChangeStartIdx, plChangeEndIdx);

		UpdateSRBuffer(context, _pointLightColorBufferSR,
			_pointLightColorBuffer.GetVector().data(),
			4, 
			plChangeStartIdx, plChangeEndIdx);
	}

	if(_spotLightBufferUpdateType == BufferUpdateType::Overall &&
		slChangeCount > 0)
	{
		UpdateSRBuffer(context, _spotLightColorBufferSR,
			_spotLightColorBuffer.GetVector().data(),
			4, 
			slChangeStartIdx, slChangeEndIdx);

		UpdateSRBuffer(context, _spotLightParamBufferSR,
			_spotLightParamBuffer.GetVector().data(),
			sizeof(SpotLight::Params),
			slChangeStartIdx, slChangeEndIdx);

		UpdateSRBuffer(context, _spotLightTransformBufferSR, 
			_spotLightTransformBuffer.GetVector().data(),
			sizeof(LightForm::LightTransformBuffer),
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

void LightManager::Delete(const std::string& key)
{
	const Light::LightForm* light = _lights.Find(key)->light;
	Light::LightForm::LightType type = light->GetType();

	if(type == LightForm::LightType::Directional)
	{
		_directionalLightColorBuffer.Delete(key);
		_directionalLightTransformBuffer.Delete(key);
		_directionalLightParamBuffer.Delete(key);
	}
	else if(type == LightForm::LightType::Point)
	{
		_pointLightTransformBuffer.Delete(key);
		_pointLightColorBuffer.Delete(key);
	}
	else if(type == LightForm::LightType::Spot)
	{
		_spotLightTransformBuffer.Delete(key);
		_spotLightColorBuffer.Delete(key);
		_spotLightParamBuffer.Delete(key);
	}

	_lights.Delete(key);
}

void LightManager::DeleteAll()
{
	_spotLightTransformBuffer.DeleteAll();
	_spotLightColorBuffer.DeleteAll();
	_spotLightParamBuffer.DeleteAll();

	_pointLightTransformBuffer.DeleteAll();
	_pointLightColorBuffer.DeleteAll();
	
	_directionalLightColorBuffer.DeleteAll();
	_directionalLightParamBuffer.DeleteAll();
	_directionalLightTransformBuffer.DeleteAll();

	_lights.DeleteAll();
}

bool LightManager::HasKey(const std::string& key)
{
	return _lights.Find(key) != nullptr;
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