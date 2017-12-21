#include "ShadowManager.h"

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

#include "BindIndexInfo.h"

#include "VertexShader.h"
#include "PixelShader.h"
#include "GeometryShader.h"

#include "Object.h"

#include <type_traits>

#include "ShadowAtlasMapRenderer.h"

using namespace Core;
using namespace Rendering;
using namespace Device;
using namespace Rendering::Manager;
using namespace Rendering::Shadow;
using namespace Rendering::Shadow::Buffer;
using namespace Rendering::Light;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::Camera;
using namespace Rendering::Renderer;
using namespace Intersection;

void ShadowManager::Initialize(DirectX& dx)
{
	_globalCB.Initialize(dx);

	GetBuffer<DirectionalLightShadow>().Initialize(dx, DIRECTIONAL_LIGHT_BUFFER_MAX_NUM);
	GetBuffer<SpotLightShadow>().Initialize(dx, SPOT_LIGHT_BUFFER_MAX_NUM);
	GetBuffer<PointLightShadow>().Initialize(dx, POINT_LIGHT_BUFFER_MAX_NUM);
}

void ShadowManager::CheckDirtyWithCullShadows(const Manager::CameraManager& camMgr, const ObjectManager& objMgr,
											  const LightManager& lightMgr, const TransformPool& tfPool)
{
	auto Work = [&lightMgr, &tfPool, &objMgr, &camMgr](auto& shadowDatas)
	{
		auto& pool				= shadowDatas.pool;
		auto& dirtys			= shadowDatas.dirtyShadows;
		auto& influentialLights	= shadowDatas.influentialLights;

		uint size = pool.GetSize();
		for (uint i = 0; i < size; ++i)
		{
			auto& shadow = pool.Get(i);
			
			Core::ObjectID objID = shadow.GetObjectID();
			using ShadowPoolType = std::decay_t<decltype(pool)>;

			const auto* light		= lightMgr.GetPool<ShadowPoolType::LightType>().Find(objID.Literal());
			const auto& lightTF		= *tfPool.Find(objID.Literal());

			const Object* object	= objMgr.Find(objID);

			// TODO :	VXGI에서 TraceShadowCone 테스트 해보고 성능이 안좋으면 복구해야함
			//			안좋으면 그땐 또 그때나름 -_-.. 이걸 어찌저찌 잡아다가 최적화 해야된다.
#if 0
			bool dirty	= light->GetBase().GetDirty() | shadow.GetDirty() | lightTF.GetDirty();
			if (dirty)
				dirtys.push_back(&shadow);

			bool culled	= (light->Intersect(frustum, tfPool) & object->GetUse()) == false;
			if (culled == false)
				culleds.push_back(&shadow);
#else

			bool visibleLight = false;
			if (object->GetUse())
			{
				camMgr.CallFrustumAllCamera(
					[&visibleLight, light, &tfPool](const Frustum& frustum)
					{
						visibleLight |= light->Intersect(frustum, tfPool);
					}
				);
			}

			if (visibleLight)
			{
				bool dirty = light->GetBase()->GetDirty() | shadow.GetDirty() | lightTF.GetDirty();
				if (dirty)
					dirtys.push_back(&shadow);

				influentialLights.push_back(light);
			}
#endif
		}
	};

	Work(GetShadowDatas<PointLightShadow>());
	Work(GetShadowDatas<SpotLightShadow>());
	Work(GetShadowDatas<DirectionalLightShadow>());
}

void ShadowManager::ClearDirty()
{	
	auto Clear = [](auto& datas)
	{
		auto& dirtys = datas.dirtyShadows;

		for(auto shadow : dirtys)
			shadow->SetDirty(false);

		dirtys.clear();
		datas.influentialLights.clear();
	};

	Clear(GetShadowDatas<DirectionalLightShadow>());
	Clear(GetShadowDatas<PointLightShadow>());
	Clear(GetShadowDatas<SpotLightShadow>());

	_changedShadowCounts = false;
}

void ShadowManager::UpdateBuffer(const LightManager& lightMgr, const TransformPool& tfPool, const Intersection::BoundBox& sceneBoundBox)
{
	auto Update = [&lightMgr, &tfPool, &sceneBoundBox](auto& shadowDatas)
	{
		auto& buffer	= shadowDatas.buffers;
		auto& dirty		= shadowDatas.dirtyShadows;
		auto& pool		= shadowDatas.pool;

		using LightType = std::decay_t<decltype(pool)>::LightType;
		buffer.UpdateBuffer(dirty, lightMgr.GetPool<LightType>(), tfPool, pool.GetIndexer());
	};

	GetBuffer<DirectionalLightShadow>().UpdateBuffer(
		GetDirtyShadows<DirectionalLightShadow>(),
		lightMgr.GetPool<DirectionalLight>(),
		tfPool, GetPool<DirectionalLightShadow>().GetIndexer(),
		sceneBoundBox
	);

	Update(GetShadowDatas<PointLightShadow>());
	Update(GetShadowDatas<SpotLightShadow>());
}

void ShadowManager::UpdateSRBuffer(DirectX& dx)
{	
	auto UpdateSRBuffer = [&dx](auto& shadowDatas)
	{
		shadowDatas.buffers.UpdateSRBuffer(dx, shadowDatas.mustUpdateToGPU);
		shadowDatas.mustUpdateToGPU = false;
	};

	UpdateSRBuffer(GetShadowDatas<DirectionalLightShadow>());
	UpdateSRBuffer(GetShadowDatas<PointLightShadow>());
	UpdateSRBuffer(GetShadowDatas<SpotLightShadow>());
}

void ShadowManager::UpdateConstBuffer(DirectX& dx, const ShadowAtlasMapRenderer& renderer)
{
	_globalCB.UpdateSubResource(dx, *this, renderer);

	GetShadowMapVPMatCBPool<DirectionalLightShadow>().InitializePreparedCB(dx);
	GetShadowMapVPMatCBPool<PointLightShadow>().InitializePreparedCB(dx);
	GetShadowMapVPMatCBPool<SpotLightShadow>().InitializePreparedCB(dx);

	GetShadowMapVPMatCBPool<DirectionalLightShadow>().UpdateSubResource(dx, GetDirtyShadows<DirectionalLightShadow>(), GetPool<DirectionalLightShadow>().GetIndexer());
	GetShadowMapVPMatCBPool<PointLightShadow>().UpdateSubResource(dx, GetDirtyShadows<PointLightShadow>(), GetPool<PointLightShadow>().GetIndexer());
	GetShadowMapVPMatCBPool<SpotLightShadow>().UpdateSubResource(dx, GetDirtyShadows<SpotLightShadow>(), GetPool<SpotLightShadow>().GetIndexer());
}

void ShadowManager::DeleteAll()
{
	auto DeleteAll = [](auto& shadowDatas)
	{
		shadowDatas.pool.DeleteAll();
		shadowDatas.dirtyShadows.clear();
		shadowDatas.buffers.DeleteAll();
		shadowDatas.cbPool.DeleteAll();

		shadowDatas.mustUpdateToGPU = true;
	};

	DeleteAll(GetShadowDatas<DirectionalLightShadow>());
	DeleteAll(GetShadowDatas<PointLightShadow>());
	DeleteAll(GetShadowDatas<SpotLightShadow>());

	_changedShadowCounts = true;
}