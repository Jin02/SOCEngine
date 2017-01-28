#pragma once

#include "CameraForm.h"
#include "ShadingWithLightCulling.h"
#include "OnlyLightCulling.h"
#include "OffScreen.h"
#include <functional>

#include "RenderTypes.h"
#include "SkyForm.h"

namespace Rendering
{
	namespace Camera
	{		
		class MeshCamera : public CameraForm
		{
		public:
			static const Usage GetUsage() {	return Usage::MeshRender; }
			static const uint NumOfRenderTargets = 4;

		private:
			Light::LightCulling::TBRParam::Packed		_prevPackedParamData;
			Buffer::ConstBuffer*						_tbrParamConstBuffer;

		private:
			Texture::RenderTexture*						_albedo_occlusion;
			Texture::RenderTexture*						_normal_roughness;
			Texture::RenderTexture*						_velocity_metallic_specularity;
			Texture::RenderTexture*						_emission_materialFlag;

			TBDR::ShadingWithLightCulling*				_deferredShadingWithLightCulling;
			Texture::DepthBuffer*						_opaqueDepthBuffer;

			Light::OnlyLightCulling*					_blendedMeshLightCulling;
			Texture::DepthBuffer*						_blendedDepthBuffer;
			TBDR::OffScreen*							_offScreen;

			float										_gamma;
			bool										_useTransparent;

		public:
			MeshCamera();
			virtual ~MeshCamera(void);
		
		public:
			virtual void OnInitialize();
			virtual void OnDestroy();

		public:
			virtual void CullingWithUpdateCB(const Device::DirectX* dx, const std::vector<Core::Object*>& objects, const Manager::LightManager* lightManager, bool useHDR);
			void Render(
				const Device::DirectX* dx,
				const Manager::RenderManager* renderManager, const Manager::LightManager* lightManager,
				const Buffer::ConstBuffer* shadowGlobalParamCB,
				Sky::SkyForm* sky,
				std::function<const Texture::RenderTexture*(MeshCamera*)> giPass);

		public:
			static void RenderMeshWithoutIASetVB(
				const Device::DirectX* dx, const Manager::RenderManager* renderManager,
				const Geometry::Mesh* mesh, RenderType renderType,
				const Buffer::ConstBuffer* camMatConstBuffer);
			static void RenderMeshesUsingSortedMeshVectorByVB(
				const Device::DirectX* dx, const Manager::RenderManager* renderManager,
				const Manager::RenderManager::MeshList& meshes,
				RenderType renderType, const Buffer::ConstBuffer* camMatConstBuffer,
				std::function<bool(const Intersection::Sphere&)>* intersectFunc = nullptr);
			static void RenderMeshesUsingMeshVector(
				const Device::DirectX* dx, const Manager::RenderManager* renderManager,
				const std::vector<const Geometry::Mesh*>& meshes,
				RenderType renderType, const Buffer::ConstBuffer* camMatConstBuffer,
				std::function<bool(const Intersection::Sphere&)>* intersectFunc = nullptr);

		public:
			void EnableRenderTransparentMesh(bool enable);
			inline void ReCompileOffScreen(bool useIndirectColorMap) { _offScreen->ReCompile(useIndirectColorMap); }

		public:
			virtual Core::Component* Clone() const;

		public:
			GET_ACCESSOR(TBRParamConstBuffer,					const Buffer::ConstBuffer*,				_tbrParamConstBuffer);

			GET_ACCESSOR(GBufferAlbedoOcclusion,				const Texture::RenderTexture*,			_albedo_occlusion);
			GET_ACCESSOR(GBufferNormalRoughness,				const Texture::RenderTexture*,			_normal_roughness);
			GET_ACCESSOR(GBufferVelocityMetallicSpecularity,	const Texture::RenderTexture*,			_velocity_metallic_specularity);
			GET_ACCESSOR(GBufferEmissionMaterialFlag,			const Texture::RenderTexture*,			_emission_materialFlag);

			GET_ACCESSOR(OpaqueDepthBuffer,						const Texture::DepthBuffer*,			_opaqueDepthBuffer);
			GET_ACCESSOR(BlendedDepthBuffer,					const Texture::DepthBuffer*,			_blendedDepthBuffer);

			GET_ACCESSOR(OffScreen,								const Texture::RenderTexture*,			_renderTarget);

			GET_ACCESSOR(DiffuseLightBuffer,					const Texture::RenderTexture*,			_deferredShadingWithLightCulling->GetDiffuseLightBuffer());
			GET_ACCESSOR(SpecularLightBuffer,					const Texture::RenderTexture*,			_deferredShadingWithLightCulling->GetSpecularLightBuffer());
			GET_ACCESSOR(PerLightIndicesBuffer,					const Buffer::ShaderResourceBuffer*,	_deferredShadingWithLightCulling->GetPerLightIndicesSRBuffer());

			GET_ACCESSOR(UseIndirectColorMap,					bool,									_offScreen->GetUseIndirectColorMap());

			GET_SET_ACCESSOR(Gamma,								float,									_gamma);
		};
	}

	namespace Renderer
	{
		typedef Camera::MeshCamera MainRenderer;
	}
}
