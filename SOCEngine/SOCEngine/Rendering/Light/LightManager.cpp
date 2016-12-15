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
using namespace Rendering;
using namespace Rendering::Shader;

LightManager::LightManager(void) 
:	_pointLightTransformSRBuffer(nullptr), _pointLightColorSRBuffer(nullptr),
	_directionalLightDirSRBuffer(nullptr), _directionalLightColorSRBuffer(nullptr),
	_spotLightTransformSRBuffer(nullptr), _spotLightColorSRBuffer(nullptr), _spotLightParamSRBuffer(nullptr),
	_pointLightOptionalParamIndexSRBuffer(nullptr), _directionalLightOptionalParamIndexSRBuffer(nullptr), _spotLightOptionalParamIndexSRBuffer(nullptr),
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

		_pointLightOptionalParamIndexSRBuffer	= new ShaderResourceBuffer;
		_pointLightOptionalParamIndexSRBuffer->Initialize(
			4, POINT_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R32_UINT,
			dummyData, true, 0, D3D11_USAGE_DYNAMIC);
	}

	// Directional Light Buffer
	{
		_directionalLightDirSRBuffer		= new ShaderResourceBuffer;
		_directionalLightDirSRBuffer->Initialize(
			sizeof(std::pair<ushort, ushort>), DIRECTIONAL_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R16G16_FLOAT,
			dummyData, true, 0, D3D11_USAGE_DYNAMIC);

		_directionalLightColorSRBuffer			= new ShaderResourceBuffer;
		_directionalLightColorSRBuffer->Initialize(
			4, DIRECTIONAL_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			dummyData, true, 0, D3D11_USAGE_DYNAMIC);

		_directionalLightOptionalParamIndexSRBuffer	= new ShaderResourceBuffer;
		_directionalLightOptionalParamIndexSRBuffer->Initialize(
			4, DIRECTIONAL_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R32_UINT,
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

		_spotLightOptionalParamIndexSRBuffer		= new ShaderResourceBuffer;
		_spotLightOptionalParamIndexSRBuffer->Initialize(
			4, SPOT_LIGHT_BUFFER_MAX_NUM,
			DXGI_FORMAT_R32_UINT,
			dummyData, true, 0, D3D11_USAGE_DYNAMIC);
	}
}

void LightManager::DestroyAllShaderReourceBuffer()
{
	SAFE_DELETE(_pointLightTransformSRBuffer);
	SAFE_DELETE(_pointLightColorSRBuffer);
	SAFE_DELETE(_pointLightOptionalParamIndexSRBuffer);

	SAFE_DELETE(_directionalLightDirSRBuffer);
	SAFE_DELETE(_directionalLightColorSRBuffer);
	SAFE_DELETE(_directionalLightOptionalParamIndexSRBuffer);

	SAFE_DELETE(_spotLightTransformSRBuffer);
	SAFE_DELETE(_spotLightColorSRBuffer);
	SAFE_DELETE(_spotLightParamSRBuffer);
	SAFE_DELETE(_spotLightOptionalParamIndexSRBuffer);
}

uint LightManager::Add(LightForm*& light)
{
	address searchKey = reinterpret_cast<address>(light);

	bool found = Has(light);
	ASSERT_MSG_IF(found == false, "Already has Key");

	uint counter = -1;
	_lights.Add(searchKey, Lights(light, counter));

	if(light->GetType() == LightForm::LightType::Directional)
	{
		DirectionalLight* l = static_cast<DirectionalLight*>(light);
		_directionalLights.Add(searchKey, l);
	}
	else if(light->GetType() == LightForm::LightType::Point)
	{
		PointLight* l = static_cast<PointLight*>(light);
		_pointLights.Add(searchKey, l);
	}
	else if(light->GetType() == LightForm::LightType::Spot)
	{
		SpotLight* l = static_cast<SpotLight*>(light);
		_spotLights.Add(searchKey, l);
	}

	return FetchLightIndexInEachLights(light);
}

template<typename LightType, typename LightTypeParam, typename LightTransformParam>
bool UpdateBuffer(ID3D11DeviceContext* context,
				  VectorHashMap<address, LightManager::Lights>& lights, uint lightIndexInAllLights,
				  VectorHashMap<address, LightTransformParam>& transformBuffer,
				  VectorHashMap<address, LightTypeParam>* paramBuffer,
				  VectorHashMap<address, uint>& colorBuffer,
				  VectorHashMap<address, uint>& optionalParamIndexBuffer,
				  const LightType* light,
				  std::function<ushort(const LightForm*)> getShadowIndexInEachShadowLights,
				  const std::function<uchar(const Light::LightForm*)>& getLightShaftIndex)
{
	uint& prevCounter	= lights.Get(lightIndexInAllLights).prevTransformUpdateCounter;
	uint curCounter		= light->GetOwner()->GetTransform()->GetUpdateCounter();

	if(curCounter == prevCounter)
		return false;

	prevCounter = curCounter;
	address key = reinterpret_cast<address>(light);

	LightTransformParam transformElem;
	LightType::Param param;
	light->MakeLightBufferElement(transformElem, param);

	ushort	shadowIndex		= getShadowIndexInEachShadowLights(light);
	uchar	lightShaftIndex	= getLightShaftIndex(light) & 0x7f;
	uchar	lightFlag		= light->GetFlag();
	uint	paramIndex		= (shadowIndex << 16) | (lightFlag << 8) | lightShaftIndex;

	uint uintColor		= light->Get32BitMainColor();

	LightTransformParam* existTransform = transformBuffer.Find(key);
	if(existTransform == nullptr) // 하나만 검색해도 됨
	{
		transformBuffer.Add(key, transformElem);
		if(paramBuffer)
			paramBuffer->Add(key, param);
		colorBuffer.Add(key, uintColor);
		optionalParamIndexBuffer.Add(key, paramIndex);
	}
	else
	{
		(*existTransform)						= transformElem;
		(*colorBuffer.Find(key))				= uintColor;
		(*optionalParamIndexBuffer.Find(key))	= paramIndex;

		LightType::Param* existParam = paramBuffer ? paramBuffer->Find(key) : nullptr;
		if(existParam)
			(*existParam) = param;
	}

	return true;
}

void LightManager::UpdateSRBufferUsingMapDiscard(ID3D11DeviceContext* context,
												 const std::function<ushort(const LightForm*)>& getShadowIndexInEachShadowLights,
												 const std::function<uchar(const Light::LightForm*)>& getLightShaftIndex)
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
			isUpdatedDL = UpdateBuffer<DirectionalLight, DirectionalLight::Param, std::pair<ushort, ushort>>(
				context, _lights, allLightBufferIdx,
				_directionalLightDirBuffer,
				nullptr,
				_directionalLightColorBuffer,
				_directionalLightOptionalParamIndexBuffer,
				dynamic_cast<const DirectionalLight*>(light),
				getShadowIndexInEachShadowLights, getLightShaftIndex);
		}
		else if(lightType == LightForm::LightType::Point)
		{	
			isUpdatedPL = UpdateBuffer<PointLight, PointLight::Param, LightForm::LightTransformBuffer>(
				context, _lights, allLightBufferIdx,
				_pointLightTransformBuffer,
				nullptr,
				_pointLightColorBuffer,
				_pointLightOptionalParamIndexBuffer,
				dynamic_cast<const PointLight*>(light),
				getShadowIndexInEachShadowLights, getLightShaftIndex);
		}
		else if(lightType == LightForm::LightType::Spot)
		{
			isUpdatedSL = UpdateBuffer<SpotLight, SpotLight::Param, LightForm::LightTransformBuffer>(
				context, _lights, allLightBufferIdx,
				_spotLightTransformBuffer,
				&_spotLightParamBuffer,
				_spotLightColorBuffer,
				_spotLightOptionalParamIndexBuffer,
				dynamic_cast<const SpotLight*>(light),
				getShadowIndexInEachShadowLights, getLightShaftIndex);
		}
	}

	if(isUpdatedDL || _forceUpdateDL)
	{
		uint count = _directionalLightDirBuffer.GetVector().size();

		// Transform
		const void* data = _directionalLightDirBuffer.GetVector().data();
		_directionalLightDirSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * sizeof(std::pair<ushort, ushort>));

		// Color
		data = _directionalLightColorBuffer.GetVector().data();
		_directionalLightColorSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * 4);

		// Optional Packed Param
		data = _directionalLightOptionalParamIndexBuffer.GetVector().data();
		_directionalLightOptionalParamIndexSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * 4);

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

		// Optional Packed Param
		data = _pointLightOptionalParamIndexBuffer.GetVector().data();
		_pointLightOptionalParamIndexSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * 4);

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

		// Optional Packed Param
		data = _spotLightOptionalParamIndexBuffer.GetVector().data();
		_spotLightOptionalParamIndexSRBuffer->UpdateResourceUsingMapUnMap(context, data, count * 4);

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
			LightForm::LightTransformBuffer* transform = _directionalLightDirBuffer.Find(key, &lightIdx);
			if( transform == nullptr ) //하나만 검색해도 됨
			{
				_directionalLightDirBuffer.Add(key, transformElem);
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

				data = _directionalLightDirBuffer.GetVector().data() + lightIdx;
				UpdateSRBuffer(context, _directionalLightDirSRBuffer, data, sizeof(std::pair<ushort, ushort>), lightIdx, lightIdx);
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

			UpdateSRBuffer(context, _directionalLightDirSRBuffer, 
				_directionalLightDirBuffer.GetVector().data(),
				sizeof(std::pair<ushort, ushort>),
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
								  const std::function<ushort(const LightForm*)>& getShadowIndexInEachShadowLights,
								  const std::function<uchar(const Light::LightForm*)>& getLightShaftIndex)
{
	//D3D_FEATURE_LEVEL level = dx->GetFeatureLevel();

	//if(level >= D3D_FEATURE_LEVEL_11_1)
	//	UpdateSRBufferUsingMapNoOverWrite(dx->GetContext());
	//else
	UpdateSRBufferUsingMapDiscard(dx->GetContext(), getShadowIndexInEachShadowLights, getLightShaftIndex);
}

void LightManager::Delete(const LightForm*& inputLight)
{
	address key = reinterpret_cast<address>(inputLight);

	const Light::LightForm* light = _lights.Find(key)->light;
	Light::LightForm::LightType type = light->GetType();

	if(type == LightForm::LightType::Directional)
	{
		_directionalLightDirBuffer.Delete(key);
		_directionalLightColorBuffer.Delete(key);
		_directionalLightOptionalParamIndexBuffer.Delete(key);
		_directionalLights.Delete(key);

		_forceUpdateDL = true;
	}
	else if(type == LightForm::LightType::Point)
	{
		_pointLightOptionalParamIndexBuffer.Delete(key);
		_pointLightTransformBuffer.Delete(key);
		_pointLightColorBuffer.Delete(key);
		_pointLights.Delete(key);

		_forceUpdatePL = true;
	}
	else if(type == LightForm::LightType::Spot)
	{
		_spotLightOptionalParamIndexBuffer.Delete(key);
		_spotLightTransformBuffer.Delete(key);
		_spotLightColorBuffer.Delete(key);
		_spotLightParamBuffer.Delete(key);
		_spotLights.Delete(key);

		_forceUpdateSL = true;
	}

	_lights.Delete(key);
}

void LightManager::DeleteAll()
{
	_spotLightTransformBuffer.DeleteAll();
	_spotLightColorBuffer.DeleteAll();
	_spotLightParamBuffer.DeleteAll();
	_spotLightOptionalParamIndexBuffer.DeleteAll();
	_spotLights.DeleteAll();
	
	_pointLightTransformBuffer.DeleteAll();
	_pointLightColorBuffer.DeleteAll();
	_pointLightOptionalParamIndexBuffer.DeleteAll();
	_pointLights.DeleteAll();

	_directionalLightColorBuffer.DeleteAll();
	_directionalLightDirBuffer.DeleteAll();
	_directionalLights.DeleteAll();
	_directionalLightOptionalParamIndexBuffer.DeleteAll();

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

		if(type == LightForm::LightType::Directional)	_directionalLights.Find(key,	&index);
		else if(type == LightForm::LightType::Point)	_pointLights.Find(key,			&index);
		else if(type == LightForm::LightType::Spot)		_spotLights.Find(key,			&index);
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

void LightManager::BindResources(const Device::DirectX* dx, bool bindVS, bool bindGS, bool bindPS) const
{
	ID3D11DeviceContext* context = dx->GetContext();

	auto BindSRBufferToVGP = [](ID3D11DeviceContext* context, TextureBindIndex bind, const ShaderResourceBuffer* srBuffer, bool bindVS, bool bindGS, bool bindPS)
	{
		if(bindVS)	VertexShader::BindShaderResourceBuffer(context, bind, srBuffer);
		if(bindGS)	GeometryShader::BindShaderResourceBuffer(context, bind, srBuffer);
		if(bindPS)	PixelShader::BindShaderResourceBuffer(context, bind, srBuffer);
	};

	BindSRBufferToVGP(context, TextureBindIndex::PointLightRadiusWithCenter,			_pointLightTransformSRBuffer,					bindVS, bindGS, bindPS);
	BindSRBufferToVGP(context, TextureBindIndex::PointLightColor,						_pointLightColorSRBuffer,						bindVS, bindGS, bindPS);
	BindSRBufferToVGP(context, TextureBindIndex::PointLightOptionalParamIndex,			_pointLightOptionalParamIndexSRBuffer,			bindVS, bindGS, bindPS);
	BindSRBufferToVGP(context, TextureBindIndex::DirectionalLightDirXY,					_directionalLightDirSRBuffer,					bindVS, bindGS, bindPS);
	BindSRBufferToVGP(context, TextureBindIndex::DirectionalLightColor,					_directionalLightColorSRBuffer,					bindVS, bindGS, bindPS);
	BindSRBufferToVGP(context, TextureBindIndex::DirectionalLightOptionalParamIndex,	_directionalLightOptionalParamIndexSRBuffer,	bindVS, bindGS, bindPS);
	BindSRBufferToVGP(context, TextureBindIndex::SpotLightRadiusWithCenter,				_spotLightTransformSRBuffer,					bindVS, bindGS, bindPS);
	BindSRBufferToVGP(context, TextureBindIndex::SpotLightParam,						_spotLightParamSRBuffer,						bindVS, bindGS, bindPS);
	BindSRBufferToVGP(context, TextureBindIndex::SpotLightColor,						_spotLightColorSRBuffer,						bindVS, bindGS, bindPS);
	BindSRBufferToVGP(context, TextureBindIndex::SpotLightOptionalParamIndex,			_spotLightOptionalParamIndexSRBuffer,			bindVS, bindGS, bindPS);
}

void LightManager::UnbindResources(const Device::DirectX* dx, bool bindVS, bool bindGS, bool bindPS) const
{
	ID3D11DeviceContext* context = dx->GetContext();

	auto UnbindSRBufferToVGP = [](ID3D11DeviceContext* context, TextureBindIndex bind, bool bindVS, bool bindGS, bool bindPS)
	{
		ID3D11ShaderResourceView* srv = nullptr;

		if(bindVS)	VertexShader::BindShaderResourceBuffer(context, bind, nullptr);
		if(bindGS)	GeometryShader::BindShaderResourceBuffer(context, bind, nullptr);
		if(bindPS)	PixelShader::BindShaderResourceBuffer(context, bind, nullptr);
	};

	UnbindSRBufferToVGP(context, TextureBindIndex::PointLightRadiusWithCenter,					bindVS, bindGS, bindPS);
	UnbindSRBufferToVGP(context, TextureBindIndex::PointLightColor,								bindVS, bindGS, bindPS);
	UnbindSRBufferToVGP(context, TextureBindIndex::PointLightOptionalParamIndex,				bindVS, bindGS, bindPS);
	UnbindSRBufferToVGP(context, TextureBindIndex::DirectionalLightDirXY,						bindVS, bindGS, bindPS);
	UnbindSRBufferToVGP(context, TextureBindIndex::DirectionalLightColor,						bindVS, bindGS, bindPS);
	UnbindSRBufferToVGP(context, TextureBindIndex::DirectionalLightOptionalParamIndex,			bindVS, bindGS, bindPS);
	UnbindSRBufferToVGP(context, TextureBindIndex::SpotLightRadiusWithCenter,					bindVS, bindGS, bindPS);
	UnbindSRBufferToVGP(context, TextureBindIndex::SpotLightParam,								bindVS, bindGS, bindPS);
	UnbindSRBufferToVGP(context, TextureBindIndex::SpotLightColor,								bindVS, bindGS, bindPS);
	UnbindSRBufferToVGP(context, TextureBindIndex::SpotLightOptionalParamIndex,					bindVS, bindGS, bindPS);
}
