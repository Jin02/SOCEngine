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

	ASSERT_COND_MSG(HasKey(searchKey), "Already has Key");

	//1�� ���ϴ°�, UpdateBuffer���� ���Ҷ� ���� �Ȼ���� �Ϸ��� -��-..
	uint counter = light->GetOwner()->GetTransform()->GetUpdateCounter() + 1;
	_lights.Add(searchKey, Lights(light, counter));
}

void LightManager::UpdateBuffer()
{
	auto CalcStartEndIdx = [](uint& start, uint& end, uint newIdx)
	{
		start = min(start, newIdx);
		end = max(end, newIdx);
	};

	ID3D11DeviceContext* context = Director::GetInstance()->GetDirectX()->GetContext();

	auto UpdateSRBuffer = [](ID3D11DeviceContext* context, ShaderResourceBuffer* srBuffer, const void* inputData, uint bufferElementSize, uint startIdx, uint endIdx)
	{
		const void* data = (char*)inputData + (startIdx * bufferElementSize);
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

		if(currentUpdateCounter != prevTransformUpdateCounter)
		{
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
				if( _directionalLightTransformBuffer.Find(key, &lightIdx) == nullptr ) //�ϳ��� �˻��ص� ��
				{
					_directionalLightTransformBuffer.Add(key, transformElem);
					_directionalLightParamBuffer.Add(key, param);
					_directionalLightColorBuffer.Add(key, uintColor);

					lightIdx = _directionalLightColorBuffer.GetSize() - 1;
				}
				else
				{
					(*_directionalLightTransformBuffer.Find(key)) = transformElem;
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
				if(_pointLightTransformBuffer.Find(key, &lightIdx) == nullptr)
				{
					_pointLightTransformBuffer.Add(key, transformElem);
					_pointLightColorBuffer.Add(key, uintColor);

					lightIdx = _pointLightColorBuffer.GetSize() - 1;
				}
				else
				{
					(*_pointLightTransformBuffer.Find(key)) = transformElem;
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
				if(_spotLightTransformBuffer.Find(key, &lightIdx) == nullptr)
				{
					_spotLightTransformBuffer.Add(key, transformElem);
					_spotLightParamBuffer.Add(key, param);
					_spotLightColorBuffer.Add(key, uintColor);

					lightIdx = _spotLightColorBuffer.GetSize() - 1;
				}
				else
				{
					(*_spotLightTransformBuffer.Find(key)) = transformElem;
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


	// transform, color, param ��,
	// �� ����ϵ� ũ��� ����.

	// ���� ���� ������Ʈ Ÿ�� ���
	// ������ �������� ����� Ƚ���� ������ MapUnMap�� �ѹ��� ����ϴ� ��� �������� ���۸� ������Ʈ�Ѵ�.
	// �װ� �ƴ϶��, CPU�� ���� ������ ������Ʈ �ϸ鼭 MapUnMap�� ���� ���۸� ������Ʈ �Ѵ�. (MapUnMap ������)
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
	// transform, color, param ��,
	// �� ����ϵ� ũ��� ����.
	uint directionalLightCount	= _directionalLightColorBuffer.GetSize() & 0x3FF;
	uint pointLightCount		= _pointLightColorBuffer.GetSize() & 0x7FF;
	uint spotLightCount			= _spotLightColorBuffer.GetSize() & 0x7FF;

	return (pointLightCount << 21) | (spotLightCount << 10) | directionalLightCount;	
}