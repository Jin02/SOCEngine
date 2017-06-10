#pragma once

#include "Half.h"
#include "ComputeShader.h"
#include "Transform.h"
#include "ShaderManager.h"
#include "MeshManager.hpp"
#include "Frustum.h"
#include "RenderTexture.h"
#include "DepthBuffer.h"
#include "ObjectId.hpp"

#undef near
#undef far

namespace Rendering
{
	namespace Camera
	{
		class MainCamera final
		{
		public:
			struct TBRParam
			{	
				Math::Matrix 		invProjMat;
				Math::Matrix 		invViewProjMat;
				Math::Matrix 		invViewProjViewport;
				
				struct Packed
				{
					uint		packedViewportSize		= 0;
					uint 		packedNumOfLights		= 0;
					uint 		maxNumOfperLightInTile	= 0;
				};
				
				Packed			packedParam;
				float			gamma					= 2.2f;
			};
			struct CameraCBData
			{
				Math::Matrix	viewMat;
				Math::Matrix	viewProjMat;
				Math::Matrix	prevViewProjMat;

				Math::Vector3	worldPos = Math::Vector3(0.0f, 0.0f, 0.0f);
				uint			packedCamNearFar = 0;
			};
			struct Desc
			{				
				float									fieldOfViewDegree = 45.0f;
				Half									near = Half(0.1f);
				Half									far = Half(1000.0f);
				float									aspect = 1.0f;
				Color									clearColor = Color::Black();
				Rect<float>								renderRect = Rect<float>(0.0f, 0.0f, 0.0f, 0.0f);
				float									gamma = 2.2f;
			};
			struct GBuffers
			{
				Texture::RenderTexture		albedo_occlusion;
				Texture::RenderTexture		normal_roughness;
				Texture::RenderTexture		velocity_metallic_specularity;
				Texture::RenderTexture		emission_materialFlag;
				Texture::DepthBuffer		opaqueDepthBuffer;
				Texture::DepthBuffer		blendedDepthBuffer;
			};

			DISALLOW_COPY_CONSTRUCTOR(MainCamera);

		public:
			MainCamera(Core::ObjectId objId) : _objId(objId) {}
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr);
			void UpdateCB(Device::DirectX& dx, const Core::Transform& transform);

			const Size<uint> ComputeThreadGroupSize(const Size<uint>& size) const;
			uint CalcMaxNumLightsInTile(const Size<uint>& size) const;

			Math::Matrix		ComputePerspectiveMatrix(bool isInverted) const;
			Math::Matrix		ComputeOrthogonalMatrix(bool isInverted) const;
			static Math::Matrix ComputeViewMatrix(const Math::Matrix &worldMatrix);
			static Math::Matrix ComputeViewportMatrix(const Rect<float>& rect);
			static Math::Matrix ComputeInvViewportMatrix(const Rect<float>& rect);

			void SortTransparentMeshRenderQueue(const Core::Transform& transform, const Manager::MeshManager& meshMgr, const Core::TransformPool& transformPool);

			GET_ACCESSOR(FieldOfViewDegree,			float,			_desc.fieldOfViewDegree);
			GET_ACCESSOR(Near,						float,			_desc.near);
			GET_ACCESSOR(Far,						float,			_desc.far);
			GET_ACCESSOR(ClearColor,				const Color&,	_desc.clearColor);
			GET_ACCESSOR(gamma,						float,			_desc.gamma);
			GET_ACCESSOR(Initialized,				bool,			_Initialized);
			SET_ACCESSOR_DIRTY(FieldOfViewDegree,	float,			_desc.fieldOfViewDegree);
			SET_ACCESSOR_DIRTY(Near,				float,			_desc.near);
			SET_ACCESSOR_DIRTY(Far,					float,			_desc.far);
			SET_ACCESSOR_DIRTY(ClearColor,			const Color&,	_desc.clearColor);
			SET_ACCESSOR_DIRTY(gamma,				float,			_desc.gamma);
			SET_ACCESSOR(ObjectId,					Core::ObjectId,	_objId);

		private:
			Texture::RenderTexture						_renderTarget;
			Shader::ComputeShader						_tbrShader;
			Buffer::ExplicitConstBuffer<CameraCBData>	_camCB;
			GBuffers									_gbuffer;

			std::vector<const Geometry::Mesh*>			_transparentMeshes;
			Math::Matrix								_viewProjMat;
			Math::Matrix								_prevViewProjMat;
			TransformCB::ChangeState					_camCBChangeState = TransformCB::ChangeState::HasChanged;
			Desc										_desc;
			Intersection::Frustum						_frustum;
			Core::ObjectId								_objId; //1
			bool										_dirty = true;  //2
			bool										_Initialized = false;
		};
	}
}