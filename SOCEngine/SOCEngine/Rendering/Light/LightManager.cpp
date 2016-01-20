#include "LightManager.h"
#include "Object.h"
#include "EngineMath.h"
#include "Utility.h"
#include "Director.h"

using namespace Device;
using namespace Math;
using namespace std;
using namespace Structure;
using namespace Rendering::Light;
using namespace Rendering::Manager;
using namespace Rendering::Shadow;
using namespace Rendering::Camera;
using namespace Rendering::Buffer;

LightManager::LightManager(void) 
:	_pointLightTransformSRBuffer(nullptr), _pointLightColorSRBuffer(nullptr),
	_directionalLightTransformSRBuffer(nullptr), _directionalLightParamSRBuffer(nullptr), _directionalLightColorSRBuffer(nullptr),
	_spotLightTransformSRBuffer(nullptr), _spotLightColorSRBuffer(nullptr), _spotLightParamSRBuffer(nullptr),
	_pointLightShadowIndexSRBuffer(nullptr), _directionalLightShadowIndexSRBuffer(nullptr), _spotLightShadowIndexSRBuffer(nullptr),
	_forceUpdateDL(true), _forceUpdatePL(true), _forceUpdateSL(true)
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

	// Point Light
	{
		_pointLightTransformSRBuffer	= new ShaderResourceBuffer;
		_pointLightTransformSRBuffer->Initialize(
			sizeof(LightForm::LightTransformBuffer), POINT_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			dummyData, true, 0, D3D11_USAGE_DYNAMIC);

		_pointLightColorSRBuffer		= new ShaderResourceBuffer;
		_pointLightColorSRBuffer->Initialize(
			4, POINT_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			dummyData, true, 0, D3D11_USAGE_DYNAMIC);

		_pointLightShadowIndexSRBuffer	= new ShaderResourceBuffer;
		_pointLightShadowIndexSRBuffer->Initialize(
			2, POINT_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R16_UINT,
			dummyData, true, 0, D3D11_USAGE_DYNAMIC);
	}

	// Directional Light Buffer
	{
		_directionalLightTransformSRBuffer		= new ShaderResourceBuffer;
		_directionalLightTransformSRBuffer->Initialize(
			sizeof(LightForm::LightTransformBuffer), DIRECTIONAL_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
		//	_directionalLightTransformBuffer.GetVector().data());
			dummyData, true, 0, D3D11_USAGE_DYNAMIC);

		_directionalLightParamSRBuffer			= new ShaderResourceBuffer;
		_directionalLightParamSRBuffer->Initialize(
			sizeof(DirectionalLight::Param), DIRECTIONAL_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R16G16_FLOAT,
			dummyData, true, 0, D3D11_USAGE_DYNAMIC);

		_directionalLightColorSRBuffer			= new ShaderResourceBuffer;
		_directionalLightColorSRBuffer->Initialize(
			4, DIRECTIONAL_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			dummyData, true, 0, D3D11_USAGE_DYNAMIC);

		_directionalLightShadowIndexSRBuffer	= new ShaderResourceBuffer;
		_directionalLightShadowIndexSRBuffer->Initialize(
			2, DIRECTIONAL_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R16_UINT,
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
			sizeof(SpotLight::Param), SPOT_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R16G16B16A16_FLOAT,
			dummyData, true, 0, D3D11_USAGE_DYNAMIC);

		_spotLightShadowIndexSRBuffer		= new ShaderResourceBuffer;
		_spotLightShadowIndexSRBuffer->Initialize(
			2, SPOT_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R16_UINT,
			dummyData, true, 0, D3D11_USAGE_DYNAMIC);
	}
}

void LightManager::DestroyAllShaderReourceBuffer()
{
	SAFE_DELETE(_pointLightTransformSRBuffer);
	SAFE_DELETE(_pointLightColorSRBuffer);
	SAFE_DELETE(_pointLightShadowIndexSRBuffer);

	SAFE_DELETE(_directionalLightTransformSRBuffer);
	SAFE_DELETE(_directionalLightParamSRBuffer);
	SAFE_DELETE(_directionalLightColorSRBuffer);
	SAFE_DELETE(_directionalLightShadowIndexSRBuffer);

	SAFE_DELETE(_spotLightTransformSRBuffer);
	SAFE_DELETE(_spotLightColorSRBuffer);
	SAFE_DELETE(_spotLightParamSRBuffer);
	SAFE_DELETE(_spotLightShadowIndexSRBuffer);
}

uint LightManager::Add(LightForm*& light)
{
	address searchKey = reinterpret_cast<address>(light);

	bool found = Has(light);
	ASSERT_COND_MSG(found == false, "Already has Key");

	uint counter = -1;
	_lights.Add(searchKey, Lights(light, counter));

	if(light->GetType() == LightForm::LightType::Directional)
	{
		DirectionalLight* dl = static_cast<DirectionalLight*>(light);
		_directionalLights.Add(searchKey, dl);
	}

	return FetchLightIndexInEachLights(light);
}

template<typename LightType, typename LightTypeParam>
bool UpdateBuffer(ID3D11DeviceContext* context,
				  VectorHashMap<address, LightManager::Lights>& lights, uint lightIndexInAllLights,
				  VectorHashMap<address, LightForm::LightTransformBuffer>& transformBuffer,
				  VectorHashMap<address, LightTypeParam>& paramBuffer,
				  VectorHashMap<address, uint>& colorBuffer,
				  VectorHashMap<address, ushort>& shadowIndexBuffer,
				  const LightType* light,
				  std::function<uint(const LightForm*)> getShadowIndexInEachShadowLights)
{
	uint& prevCounter	= lights.Get(lightIndexInAllLights).prevTransformUpdateCounter;
	uint curCounter		= light->GetOwner()->GetTransform()->GetUpdateCounter();

	if(curCounter == prevCounter)
		return false;

	prevCounter = curCounter;
	address key = reinterpret_cast<address>(light);

	LightForm::LightTransformBuffer transformElem;
	LightType::Param param;
	light->MakeLightBufferElement(transformElem, param);

	ushort shadowIndex	= getShadowIndexInEachShadowLights(light);
	uint uintColor		= light->Get32BitMainColor();

	LightForm::LightTransformBuffer* existTransform = transformBuffer.Find(key);
	if(existTransform == nullptr) // 하나만 검색해도 됨
	{
		transformBuffer.Add(key, transformElem);
		paramBuffer.Add(key, param);
		colorBuffer.Add(key, uintColor);
		shadowIndexBuffer.Add(key, shadowIndex);
	}
	else
	{
		(*existTransform)				= transformElem;
		(*colorBuffer.Find(key))		= uintColor;
		(*shadowIndexBuffer.Find(key))	= shadowIndex;

		// Point Light는 Param을 실질적으로 가지고 있지 않다.
		// 그래서 이 부분만 예외로 이런식으로 처리한다.
		LightType::Param* existParam = paramBuffer.Find(key);
		if(existParam)
			(*existParam) = param;
	}

	return true;
}

void LightManager::UpdateSRBufferUsingMapDiscard(ID3D11DeviceContext* context,
												 const std::function<uint(const LightForm*)>& getShadowIndexInEachShadowLights)
{
	bool isUpdatedDL = false;
	bool isUpdatedPL = false;
	bool isUpdatedSL = false;

	uint allLightBufferIdx = 0;
	auto& lights = _lights.GetVector();
	for(auto iter = lights.begin(); iter != lights.end(); ++iter, ++allLightBufferIdx)
	{
		const Light::LightForm* light	= iter->light;
		LightForm::LightType lightType	= light->GetType();

		if(lightType == LightForm::LightType::Directional)
		{
			isUpdatedDL = UpdateBuffer<DirectionalLight, DirectionalLight::Param>(
				context, _lights, allLightBufferIdx,
				_directionalLightTransformBuffer,
				_directionalLightParamBuffer,
				_directionalLightColorBuffer,
				_directionalLightShadowIndexBuffer,
				dynamic_cast<const DirectionalLight*>(light),
				getShadowIndexInEachShadowLights);
		}
		else if(lightType == LightForm::LightType::Point)
		{	
			VectorHashMap<address, PointLight::Param> dummy;

			isUpdatedPL = UpdateBuffer<PointLight, PointLight::Param>(
				context, _lights, allLightBufferIdx,
				_pointLightTransformBuffer,
				dummy,
				_pointLightColorBuffer,
				_pointLightShadowIndexBuffer,
				dynamic_cast<const PointLight*>(light),
				getShadowIndexInEachShadowLights);
		}
		else if(lightType == LightForm::LightType::Spot)
		{
			isUpdatedSL = UpdateBuffer<SpotLight, SpotLight::Param>(
				context, _lights, allLightBufferIdx,
				_spotLightTransformBuffer,
				_spotLightParamBuffer,
				_spotLightColorBuffer,
				_spotLightShadowIndexBuffer,
				dynamic_cast<const SpotLight*>(light),
				getShadowIndexInEachShadowLights);
		}
	}

	if(isUpdatedDL || _forceUpdateDL)
	{
		uint count = _directionalLightTransformBuffer.GetVector().size();

		// Transform
		const void* data = _directionalLightTransformBuffer.GetVector().data();
		_directionalLightTransformSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * sizeof(LightForm::LightTransformBuffer));

		// Color
		data = _directionalLightColorBuffer.GetVector().data();
		_directionalLightColorSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * 4);

		// Param
		data = _directionalLightParamBuffer.GetVector().data();
		_directionalLightParamSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * sizeof(DirectionalLight::Param));

		// Shadow Index
		data = _directionalLightShadowIndexBuffer.GetVector().data();
		_directionalLightShadowIndexSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * 2);

		_forceUpdateDL = false;
	}

	if(isUpdatedPL || _forceUpdatePL)
	{
		uint count = _pointLightTransformBuffer.GetVector().size();

		// Transform
		const void* data = _pointLightTransformBuffer.GetVector().data();
		_pointLightTransformSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * sizeof(LightForm::LightTransformBuffer));

		// Color
		data = _pointLightColorBuffer.GetVector().data();
		_pointLightColorSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * 4);

		// Shadow Index
		data = _pointLightShadowIndexBuffer.GetVector().data();
		_pointLightShadowIndexSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * 2);

		_forceUpdatePL = false;
	}

	if(isUpdatedSL || _forceUpdateSL)
	{
		uint count = _spotLightTransformBuffer.GetVector().size();

		// Transform
		const void* data = _spotLightTransformBuffer.GetVector().data();
		_spotLightTransformSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * sizeof(LightForm::LightTransformBuffer));

		// Color
		data = _spotLightColorBuffer.GetVector().data();
		_spotLightColorSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * 4);
		
		// Param
		data = _spotLightParamBuffer.GetVector().data();
		_spotLightParamSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * sizeof(SpotLight::Param));

		// Shadow Index
		data = _spotLightShadowIndexBuffer.GetVector().data();
		_spotLightShadowIndexSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * 2);

		_forceUpdateSL = false;
	}
}

void LightManager::UpdateSRBufferUsingMapNoOverWrite(ID3D11DeviceContext* context)
{
#if 0
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

		if(lightType == LightForm::LightType::Directional)
		{			
			const DirectionalLight* dl = dynamic_cast<const DirectionalLight*>(light);

			LightForm::LightTransformBuffer transformElem;
			DirectionalLight::Param param;
			dl->MakeLightBufferElement(transformElem, param);

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
				(*transform)								= transformElem;
				(*_directionalLightParamBuffer.Find(key))	= param;
				(*_directionalLightColorBuffer.Find(key))	= uintColor;
			}

			if(_directionalLightBufferUpdateType == BufferUpdateType::Selective)
			{
				const void* data = _directionalLightColorBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _directionalLightColorSRBuffer, data, 4, lightIdx, lightIdx);

				data = _directionalLightParamBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _directionalLightParamSRBuffer, data, sizeof(DirectionalLight::Param), lightIdx, lightIdx);

				data = _directionalLightTransformBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _directionalLightTransformSRBuffer, data, sizeof(LightForm::LightTransformBuffer), lightIdx, lightIdx);
			}

			CalcStartEndIdx(dlChangeStartIdx, dlChangeEndIdx, lightIdx);
			++dlChangeCount;
		}
		else if(lightType == LightForm::LightType::Point)
		{
			const PointLight* pl = dynamic_cast<const PointLight*>(light);

			LightForm::LightTransformBuffer transformElem;
			pl->MakeLightBufferElement(transformElem);

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
				(*transform)						= transformElem;
				(*_pointLightColorBuffer.Find(key))	= uintColor;
			}

			if(_pointLightBufferUpdateType == BufferUpdateType::Selective)
			{
				const void* data = _pointLightColorBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _pointLightColorSRBuffer, data, 4, lightIdx, lightIdx);

				data = _pointLightTransformBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _pointLightTransformSRBuffer, data, sizeof(LightForm::LightTransformBuffer), lightIdx, lightIdx);
			}

			CalcStartEndIdx(plChangeStartIdx, plChangeEndIdx, lightIdx);
			++plChangeCount;
		}
		else if(lightType == LightForm::LightType::Spot)
		{
			const SpotLight* sl = dynamic_cast<const SpotLight*>(light);

			LightForm::LightTransformBuffer transformElem;
			SpotLight::Param param;
			sl->MakeLightBufferElement(transformElem, param);

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
				(*transform)								= transformElem;
				(*_spotLightParamBuffer.Find(key))			= param;
				(*_spotLightColorBuffer.Find(key))			= uintColor;
			}

			if(_spotLightBufferUpdateType == BufferUpdateType::Selective)
			{
				const void* data = _spotLightColorBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _spotLightColorSRBuffer, data, 4, lightIdx, lightIdx);

				data = _spotLightParamBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _spotLightParamSRBuffer, data, sizeof(SpotLight::Param), lightIdx, lightIdx);

				data = _spotLightTransformBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _spotLightTransformSRBuffer, data, sizeof(LightForm::LightTransformBuffer), lightIdx, lightIdx);
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
				sizeof(DirectionalLight::Param),
				dlChangeStartIdx, dlChangeEndIdx);

			UpdateSRBuffer(context, _directionalLightTransformSRBuffer, 
				_directionalLightTransformBuffer.GetVector().data(),
				sizeof(LightForm::LightTransformBuffer),
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
			sizeof(SpotLight::Param),
			slChangeStartIdx, slChangeEndIdx);

		UpdateSRBuffer(context, _spotLightTransformSRBuffer, 
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
#endif
	ASSERT_MSG("Deprecated func");
}

void LightManager::UpdateSRBuffer(const DirectX* dx,
								  const std::function<uint(const LightForm*)>& getShadowIndexInEachShadowLights)
{
	//D3D_FEATURE_LEVEL level = dx->GetFeatureLevel();

	//if(level >= D3D_FEATURE_LEVEL_11_1)
	//	UpdateSRBufferUsingMapNoOverWrite(dx->GetContext());
	//else
	UpdateSRBufferUsingMapDiscard(dx->GetContext(), getShadowIndexInEachShadowLights);
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
		_directionalLightShadowIndexBuffer.Delete(key);
		_directionalLights.Delete(key);

		_forceUpdateDL = true;
	}
	else if(type == LightForm::LightType::Point)
	{
		_pointLightShadowIndexBuffer.Delete(key);
		_pointLightTransformBuffer.Delete(key);
		_pointLightColorBuffer.Delete(key);

		_forceUpdatePL = true;
	}
	else if(type == LightForm::LightType::Spot)
	{
		_spotLightShadowIndexBuffer.Delete(key);
		_spotLightTransformBuffer.Delete(key);
		_spotLightColorBuffer.Delete(key);
		_spotLightParamBuffer.Delete(key);

		_forceUpdateSL = true;
	}

	_lights.Delete(key);
}

void LightManager::DeleteAll()
{
	_spotLightTransformBuffer.DeleteAll();
	_spotLightColorBuffer.DeleteAll();
	_spotLightParamBuffer.DeleteAll();
	_spotLightShadowIndexBuffer.DeleteAll();
	
	_pointLightTransformBuffer.DeleteAll();
	_pointLightColorBuffer.DeleteAll();
	_pointLightShadowIndexBuffer.DeleteAll();

	_directionalLightColorBuffer.DeleteAll();
	_directionalLightParamBuffer.DeleteAll();
	_directionalLightTransformBuffer.DeleteAll();
	_directionalLights.DeleteAll();
	_directionalLightShadowIndexBuffer.DeleteAll();

	_lights.DeleteAll();

	_forceUpdateDL = 
	_forceUpdatePL =
	_forceUpdateSL = true;
}

bool LightManager::Has(LightForm*& light) const
{
	address key = reinterpret_cast<address>(light);
	return _lights.Has(key);
}

uint LightManager::FetchLightIndexInEachLights(const LightForm* inputLight) const
{
	uint index = -1;

	if(inputLight)
	{
		address key = reinterpret_cast<address>(inputLight);

		const Light::LightForm* light		= _lights.Find(key)->light;
		Light::LightForm::LightType type	= light->GetType();

		if(type == LightForm::LightType::Directional)	_directionalLightColorBuffer.Find(key,	&index);
		else if(type == LightForm::LightType::Point)	_pointLightColorBuffer.Find(key,		&index);
		else if(type == LightForm::LightType::Spot)		_spotLightColorBuffer.Find(key,			&index);
	}

	return index;
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

void LightManager::Destroy()
{
	DeleteAll();
	DestroyAllShaderReourceBuffer();
}

void LightManager::ComputeDirectionalLightViewProj(const Intersection::BoundBox& sceneBoundBox, float directionalLightShadowMapResolution)
{
	Matrix invViewport;
	CameraForm::GetInvViewportMatrix(invViewport, Rect<float>(0.0f, 0.0f, directionalLightShadowMapResolution, directionalLightShadowMapResolution));

	const auto& lights = _directionalLights.GetVector();
	for(auto iter = lights.begin(); iter != lights.end(); ++iter)
		(*iter)->ComputeViewProjMatrix(sceneBoundBox, invViewport);
}
