#pragma once

#include "ShadowManager.h"
#include "Rect.h"
#include "DepthMap.h"
#include "ShadowAtlasMap.h"
#include "MeshUtility.h"
#include "MeshRenderer.h"

#include "PointLightShadowMapRenderer.h"
#include "ShadowMapRenderer.h"

namespace Rendering
{
	namespace Renderer
	{
		class ShadowAtlasMapRenderer final
		{
		public:
			void Initialize(Device::DirectX& dx, uint dlMapResolution, uint slMapResolution, uint plMapResolution);
			void Destroy();

			struct ReSizeParam
			{
				uint mapResolution	= 128;
				uint capacity		= 8;

				ReSizeParam(uint resolution, uint _capacity)
					: mapResolution(resolution), capacity(_capacity) { }
			};

			template <typename ShadowType>
			void ReSizeShadowMap(Device::DirectX& dx, ReSizeParam param, bool useDestroy = true)
			{
				// Compute Param
				{
					auto Next2Squre = [](uint value) -> uint
					{
						return 1 << (uint)(ceil(log((uint)value) / log(2.0f)));
					};

					auto& atlasMap = GetShadowAtlasMap<ShadowType>();

					bool changedShadowMap = (atlasMap.GetResolution() != param.mapResolution) | (atlasMap.GetCapacity() < param.capacity);

					if (changedShadowMap == false)
						return;

					param.capacity		= Next2Squre(param.capacity);
					param.mapResolution	= Next2Squre(param.mapResolution);
				}

				// Resize Map
				{
					if (useDestroy)
						GetShadowAtlasMap<ShadowType>().Destroy();

					auto MakeMap = [&dx, &param](auto& depthMap, const Size<uint>& mapSize)
					{
						depthMap.Initialize(dx, mapSize, param.mapResolution, param.capacity);
					};

					Size<uint> mapSize = ComputeShadowAtlasMapSize<ShadowType>(param);
					MakeMap(GetShadowAtlasMap<ShadowType>(), mapSize);
				}
			}

			template <class ShadowType>
			void RenderShadowMap(Device::DirectX& dx, const Manager::ShadowManager& shadowMgr, const ShadowMap::RenderManagerParam& mgrParam)
			{
				auto ClassifyMeshes = [&param = mgrParam](auto& tempRenderQ, const auto& meshPool, const ShadowType::LightType* light)
				{
					Geometry::MeshUtility::ClassifyOpaqueMesh(tempRenderQ, meshPool, param.objMgr, param.transformPool,
						[&tfPool = param.transformPool, light]
						(const Geometry::Mesh& mesh, const Core::Transform& transform)
						{
							auto sphere = Intersection::Sphere(transform.GetWorldPosition(), mesh.GetRadius());
							return light->Intersect(sphere, tfPool);
						}
					);
				};

				// 1 단계
				// 현재 카메라 범위안에 속한 그림자를 그리는 빛만 추려냄
				const auto& influentialLights = shadowMgr.GetInfluentialLights<ShadowType>();
				for (auto light : influentialLights)
				{
					// 2 단계
					// 이 빛들은 하나의 카메라가 된다.
					// 렌더링 전에 컬링을 했던 것 처럼 여기서도 컬링한다. 
					ClassifyMeshes(_tempRenderQ.opaqueRenderQ, mgrParam.meshManager.GetOpaqueMeshPool(), light);
					ClassifyMeshes(_tempRenderQ.alphaTestRenderQ, mgrParam.meshManager.GetAlphaTestMeshPool(), light);

					// 3 단계 - 그린다					
					uint shadowIndex		= shadowMgr.GetIndexer<ShadowType>().Find(light->GetObjectID().Literal());
					auto& shadowMap			= GetShadowAtlasMap<ShadowType>();
					uint resolution			= shadowMap.GetResolution();
					const auto& shadowMapCB	= shadowMgr.GetShadowMapCBPool<ShadowType>().Get(shadowIndex);

					using Renderer = ShadowType::ShadowMapRenderer;
					ShadowType::ShadowMapRenderer::Render(dx, Renderer::Param(shadowMapCB, shadowMap, shadowIndex, resolution), _tempRenderQ, mgrParam);
				}
			}

			template <class ShadowType> auto&		GetShadowAtlasMap()
			{
				return std::get<Shadow::ShadowAtlasMap<ShadowType>>(_shadowAtlasMaps);
			}
			template <class ShadowType> const auto&	GetShadowAtlasMap() const
			{
				return std::get<Shadow::ShadowAtlasMap<ShadowType>>(_shadowAtlasMaps);
			}

		private:
			template <typename ShadowType>
			Size<uint> ComputeShadowAtlasMapSize(const ReSizeParam& param)
			{
				return Size<uint>(	param.mapResolution * param.capacity,
									param.mapResolution	);
			}
			template <>
			Size<uint> ComputeShadowAtlasMapSize<Shadow::PointLightShadow>(const ReSizeParam& param)
			{
				return Size<uint>(	param.mapResolution * param.capacity,
									param.mapResolution * 6	);
			}

		private:
			std::tuple<
				Shadow::ShadowAtlasMap<Shadow::DirectionalLightShadow>,
				Shadow::ShadowAtlasMap<Shadow::PointLightShadow>,
				Shadow::ShadowAtlasMap<Shadow::SpotLightShadow>
			> _shadowAtlasMaps;

			ShadowMap::TempRenderQueue _tempRenderQ;
		};
	}

	struct ShadowSystemParam
	{
		Manager::ShadowManager&		manager;
		Renderer::ShadowAtlasMapRenderer&	renderer;

		ShadowSystemParam(Manager::ShadowManager& _manager, Renderer::ShadowAtlasMapRenderer& _renderer)
			: manager(_manager), renderer(_renderer) { }
	};
}