#include "ShadowManager.h"

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

#include "BindIndexInfo.h"

#include "VertexShader.h"
#include "PixelShader.h"
#include "GeometryShader.h"

#include <type_traits>

using namespace Core;
using namespace Rendering;
using namespace Rendering::Manager;
using namespace Rendering::Shadow;
using namespace Rendering::Shadow::Buffer;
using namespace Rendering::Light;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;

void ShadowManager::Initialize(Device::DirectX & dx)
{
	_globalCB.Initialize(dx);

	GetBuffer<DirectionalLightShadow>().GetBuffer().Initialize(dx);
	GetBuffer<SpotLightShadow>().GetBuffer().Initialize(dx, SPOT_LIGHT_BUFFER_MAX_NUM);
	GetBuffer<PointLightShadow>().GetBuffer().Initialize(dx, POINT_LIGHT_BUFFER_MAX_NUM);
}

void ShadowManager::UpdateGlobalCB(Device::DirectX & dx)
{
	if(_dirtyGlobalParam == false)
		return;

	ShadowGlobalParam param;
	{
		auto& dlsBufferObj = GetBuffer<DirectionalLightShadow>();
		auto& plsBufferObj = GetBuffer<PointLightShadow>();
		auto& slsBufferObj = GetBuffer<SpotLightShadow>();

		auto Pack = [](uint d, uint s, uint p) -> uint
		{
			return ((p & 0x7ff) << 21) | ((s & 0x7ff) << 10) | (d & 0x3ff);
		};
		auto Log2Uint = [](uint i) -> uint
		{
			return static_cast<uint>(log(static_cast<float>(i)) / log(2.0f));
		};

		param.packedNumOfShadowAtlasCapacity = Pack(dlsBufferObj.GetLightCapacityInAtlas(),
													slsBufferObj.GetLightCapacityInAtlas(),
													plsBufferObj.GetLightCapacityInAtlas());

		param.packedPowerOfTwoShadowResolution = Pack(	Log2Uint(dlsBufferObj.GetMapResolution()),
														Log2Uint(slsBufferObj.GetMapResolution()),
														Log2Uint(plsBufferObj.GetMapResolution())	);

		param.packedNumOfShadows = Pack(GetPool<DirectionalLightShadow>().GetSize(),
										GetPool<SpotLightShadow>().GetSize(),
										GetPool<PointLightShadow>().GetSize()	);
		param.dummy = 0;

		dlsBufferObj.SetDirty(false);
		plsBufferObj.SetDirty(false);
		slsBufferObj.SetDirty(false);
	}

	_globalCB.UpdateSubResource(dx, param);
	_dirtyGlobalParam = false;
}

void ShadowManager::CheckDirtyShadows(const LightManager& lightMgr, const TransformPool& tfPool)
{
	auto UpdateDirtyShadow = [&lightMgr, &tfPool, this](auto& shadowDatas)
	{
		auto& pool		= shadowDatas.pool;
		auto& dirtys	= shadowDatas.dirtyShadows;

		uint size = pool.GetSize();
		for (uint i = 0; i < size; ++i)
		{
			auto& shadow = pool.Get(i);
			
			Core::ObjectID objID = shadow.GetObjectID();
			using ShadowPoolType = std::decay_t<decltype(pool)>;

			auto light = lightMgr.GetPool<ShadowPoolType::LightType>().Find(objID.Literal()); assert(light);
			auto transform = tfPool.Find(objID.Literal()); assert(transform);

			bool dirty = light->GetBase().GetDirty() | shadow.GetDirty() | transform->GetDirty();
			if (dirty)
				dirtys.push_back(&shadow);
		}
	};

	UpdateDirtyShadow(GetShadowDatas<PointLightShadow>());
	UpdateDirtyShadow(GetShadowDatas<SpotLightShadow>());
	UpdateDirtyShadow(GetShadowDatas<DirectionalLightShadow>());

	_dirtyGlobalParam |=	GetBuffer<DirectionalLightShadow>().GetDirty() |
							GetBuffer<PointLightShadow>().GetDirty()		 |
							GetBuffer<SpotLightShadow>().GetDirty();
}

void ShadowManager::ClearDirty()
{	
	auto Clear = [](auto& datas)
	{
		auto& dirtys = datas.dirtyShadows;

		for(auto shadow : dirtys)
			shadow->SetDirty(false);
	};

	Clear(GetShadowDatas<DirectionalLightShadow>());
	Clear(GetShadowDatas<PointLightShadow>());
	Clear(GetShadowDatas<SpotLightShadow>());
}

void ShadowManager::UpdateBuffer(const LightManager& lightMgr, const TransformPool& tfPool, const Intersection::BoundBox& sceneBoundBox)
{
	auto Update = [&lightMgr, &tfPool, &sceneBoundBox](auto& shadowDatas)
	{
		auto& buffer	= shadowDatas.buffers.GetBuffer();
		auto& dirty		= shadowDatas.dirtyShadows;
		auto& pool		= shadowDatas.pool;

		using LightType = std::decay_t<decltype(pool)>::LightType;
		buffer.UpdateBuffer(dirty, lightMgr.GetPool<LightType>(), tfPool, pool.GetIndexer());
	};

	GetBuffer<DirectionalLightShadow>().GetBuffer().UpdateBuffer(
		GetDirtyShadows<DirectionalLightShadow>(),
		lightMgr.GetPool<DirectionalLight>(),
		tfPool, GetPool<DirectionalLightShadow>().GetIndexer(),
		sceneBoundBox
	);

	Update(GetShadowDatas<PointLightShadow>());
	Update(GetShadowDatas<SpotLightShadow>());
}

void ShadowManager::UpdateSRBuffer(Device::DirectX & dx)
{	
	GetBuffer<DirectionalLightShadow>().GetBuffer().UpdateSRBuffer(dx);
	GetBuffer<PointLightShadow>().GetBuffer().UpdateSRBuffer(dx);
	GetBuffer<SpotLightShadow>().GetBuffer().UpdateSRBuffer(dx);
}

void ShadowManager::UpdateConstBuffer(Device::DirectX& dx)
{
	GetShadowMapCB<DirectionalLightShadow>().InitializePreparedCB(dx);
	GetShadowMapCB<PointLightShadow>().InitializePreparedCB(dx);
	GetShadowMapCB<SpotLightShadow>().InitializePreparedCB(dx);

	GetShadowMapCB<DirectionalLightShadow>().UpdateSubResource(dx, GetDirtyShadows<DirectionalLightShadow>(), GetPool<DirectionalLightShadow>().GetIndexer());
	GetShadowMapCB<PointLightShadow>().UpdateSubResource(dx, GetDirtyShadows<PointLightShadow>(), GetPool<PointLightShadow>().GetIndexer());
	GetShadowMapCB<SpotLightShadow>().UpdateSubResource(dx, GetDirtyShadows<SpotLightShadow>(), GetPool<SpotLightShadow>().GetIndexer());
}

void ShadowManager::DeleteAll()
{
	GetBuffer<PointLightShadow>().GetBuffer().DeleteAll();
	GetBuffer<SpotLightShadow>().GetBuffer().DeleteAll();
	GetBuffer<DirectionalLightShadow>().GetBuffer().DeleteAll();

	GetPool<PointLightShadow>().DeleteAll();
	GetPool<SpotLightShadow>().DeleteAll();
	GetPool<DirectionalLightShadow>().DeleteAll();

	GetDirtyShadows<PointLightShadow>().clear();
	GetDirtyShadows<SpotLightShadow>().clear();
	GetDirtyShadows<DirectionalLightShadow>().clear();
}

void ShadowManager::BindResources(Device::DirectX & dx, bool bindVS, bool bindGS, bool bindPS)
{	
	auto Bind = [&dx, bindVS, bindGS, bindPS](
		TextureBindIndex bind, ShaderResourceBuffer& srb)
	{
		if (bindVS)	VertexShader::BindShaderResourceView(dx, bind, srb.GetShaderResourceView());
		if (bindGS)	GeometryShader::BindShaderResourceView(dx, bind, srb.GetShaderResourceView());
		if (bindPS)	PixelShader::BindShaderResourceView(dx, bind, srb.GetShaderResourceView());
	};

	auto& plBuffer = GetBuffer<PointLightShadow>().GetBuffer();
	{
		Bind(TextureBindIndex::PointLightShadowParam, plBuffer.GetParamSRBuffer());
		Bind(TextureBindIndex::PointLightShadowViewProjMatrix,plBuffer.GetViewProjMatSRBuffer());
	}

	auto& dlBuffer = GetBuffer<DirectionalLightShadow>().GetBuffer();
	{
		Bind(TextureBindIndex::DirectionalLightShadowParam, dlBuffer.GetParamSRBuffer());
		Bind(TextureBindIndex::DirectionalLightShadowViewProjMatrix, dlBuffer.GetViewProjMatSRBuffer());
		Bind(TextureBindIndex::DirectionalLightShadowInvProjParam, dlBuffer.GetInvProjParamSRBuffer());
	}

	auto& slBuffer = GetBuffer<SpotLightShadow>().GetBuffer();
	{
		Bind(TextureBindIndex::SpotLightShadowParam, slBuffer.GetParamSRBuffer());
		Bind(TextureBindIndex::SpotLightShadowViewProjMatrix, slBuffer.GetViewProjMatSRBuffer());
	}

	if (bindVS) VertexShader::BindConstBuffer(dx, ConstBufferBindIndex::ShadowGlobalParam, _globalCB);
	if (bindGS) GeometryShader::BindConstBuffer(dx, ConstBufferBindIndex::ShadowGlobalParam, _globalCB);
	if (bindPS) PixelShader::BindConstBuffer(dx, ConstBufferBindIndex::ShadowGlobalParam, _globalCB);
	}

void ShadowManager::UnbindResources(Device::DirectX & dx, bool bindVS, bool bindGS, bool bindPS) const
{	
	auto Unbind = [&dx, bindVS, bindGS, bindPS](TextureBindIndex bind)
	{
		if (bindVS)	VertexShader::UnBindShaderResourceView(dx, bind);
		if (bindGS)	GeometryShader::UnBindShaderResourceView(dx, bind);
		if (bindPS)	PixelShader::UnBindShaderResourceView(dx, bind);
	};

	// SRBuffer
	{
		Unbind(TextureBindIndex::PointLightShadowParam);
		Unbind(TextureBindIndex::PointLightShadowViewProjMatrix);
		Unbind(TextureBindIndex::DirectionalLightShadowParam);
		Unbind(TextureBindIndex::DirectionalLightShadowViewProjMatrix);
		Unbind(TextureBindIndex::SpotLightShadowParam);
		Unbind(TextureBindIndex::SpotLightShadowViewProjMatrix);
	}

	if (bindVS) VertexShader::UnBindConstBuffer(dx, ConstBufferBindIndex::ShadowGlobalParam);
	if (bindGS) GeometryShader::UnBindConstBuffer(dx, ConstBufferBindIndex::ShadowGlobalParam);
	if (bindPS) PixelShader::UnBindConstBuffer(dx, ConstBufferBindIndex::ShadowGlobalParam);
}
