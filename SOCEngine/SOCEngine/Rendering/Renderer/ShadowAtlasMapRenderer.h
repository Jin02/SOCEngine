#pragma once

#include "ShadowManager.h"
#include "Rect.h"
#include "DepthMap.h"
#include "ShadowAtlasMap.h"
#include "MeshUtility.h"
#include "MeshRenderer.h"

#include "PointLightShadowMapRenderer.h"
#include "ShadowMapRenderer.h"
#include "ShadowAtlasMapRenderer.h"

namespace Rendering
{
	namespace Renderer
	{
		class ShadowAtlasMapRenderer final
		{
		public:
			struct ResizeParam
			{
				uint mapResolution	= 256;
				uint capacity		= 1;

				explicit ResizeParam(uint resolution, uint _capacity)
					: mapResolution(resolution), capacity(_capacity) { }
			};

		public:
			ShadowAtlasMapRenderer() = default;
			void Initialize(Device::DirectX& dx, uint dlMapResolution, uint slMapResolution, uint plMapResolution);
			void Destroy();

			template <typename ShadowType>
			void ReSizeShadowMap(Device::DirectX& dx, uint shadowCount)
			{
				ReSizeShadowMap<ShadowType>(dx, ResizeParam(GetShadowAtlasMap<ShadowType>().GetResolution(), shadowCount));
			}

			template <typename ShadowType>
			void ReSizeShadowMap(Device::DirectX& dx, ResizeParam&& param)
			{
				auto Next2Squre = [](uint value) -> uint
				{
					return 1 << (uint)(ceil(log((uint)value) / log(2.0f)));
				};

				param.capacity		= Next2Squre(param.capacity);
				param.mapResolution	= Next2Squre(param.mapResolution);

				const auto& atlasMap = GetShadowAtlasMap<ShadowType>();

				bool hasChanged = (atlasMap.GetResolution() != param.mapResolution) | (atlasMap.GetCapacity() < param.capacity);
				_hasChangedAtlasMapSize |= hasChanged;

				if (hasChanged)
				{
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
			void RenderShadowMap(	Device::DirectX& dx,
									const Manager::ShadowManager& shadowMgr, const Manager::MaterialManager& materialMgr,
									const CullingParam& cullParam, const MeshRenderer::Param& meshParam	)
			{
				auto ClassifyMeshes = [&param = cullParam](auto& tempRenderQ, const auto& meshPool, const ShadowType::LightType* light)
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

				// 1 �ܰ�
				// ���� ī�޶� �����ȿ� ���� �׸��ڸ� �׸��� ���� �߷���
				const auto& influentialLights = shadowMgr.GetInfluentialLights<ShadowType>();

				// 2 �ܰ�
				// ���� �׸��� ������ üũ�ؼ� ShadowMap ũ�⸦ �����Ѵ�.
				// TODO :	�׸��ڸ� �׸� �� ������ üũ�ؼ� ShadowAtlasMap ũ�⸦ ������ �ؾ��Ѵ�.
				//			���߿� �ð����� ��ģ��.
				{
					ReSizeShadowMap<ShadowType>(dx, shadowMgr.GetPool<ShadowType>().GetSize());
				}

				for (auto light : influentialLights)
				{
					// 3 �ܰ�
					// �� ������ �ϳ��� ī�޶� �ȴ�.
					// ������ ���� �ø��� �ߴ� �� ó�� ���⼭�� �ø��Ѵ�. 
					ClassifyMeshes(_tempRenderQ.opaqueRenderQ, cullParam.meshManager.GetOpaqueMeshPool(), light);
					ClassifyMeshes(_tempRenderQ.alphaTestRenderQ, cullParam.meshManager.GetAlphaTestMeshPool(), light);

					// 4 �ܰ� - �׸���					
					uint shadowIndex				= shadowMgr.GetIndexer<ShadowType>().Find(light->GetObjectID().Literal());
					auto& shadowMap					= GetShadowAtlasMap<ShadowType>();
					uint resolution					= shadowMap.GetResolution();
					const auto& shadowMapVPMatCB	= shadowMgr.GetShadowMapVPMatCBPool<ShadowType>().Get(shadowIndex);

					using Renderer = ShadowType::ShadowMapRenderer;
					ShadowType::ShadowMapRenderer::Render(dx, Renderer::Param(materialMgr, shadowMap, shadowIndex, resolution), _tempRenderQ, meshParam, shadowMapVPMatCB);
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

			void ClearDirty() { _hasChangedAtlasMapSize = false; }
			GET_CONST_ACCESSOR(HasChangedAtlasMapSize, bool, _hasChangedAtlasMapSize);

		private:
			template <typename ShadowType>
			Size<uint> ComputeShadowAtlasMapSize(const ResizeParam& param)
			{
				return Size<uint>(	param.mapResolution * param.capacity,
									param.mapResolution	);
			}
			template <>
			Size<uint> ComputeShadowAtlasMapSize<Shadow::PointLightShadow>(const ResizeParam& param)
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

			TempRenderQueue _tempRenderQ;
			bool			_hasChangedAtlasMapSize = true;
		};
	}
}