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
			struct TBRCBData
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
				float									near = 0.1f;
				float									far = 1000.0f;
				float									aspect = 1.0f;
				Color									clearColor = Color::Black();
				Rect<uint>								renderRect = Rect<uint>(0, 0, 0, 0);
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
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Rect<uint>& rect);
			void UpdateCB(Device::DirectX& dx, const Core::Transform& transform, uint packedNumOfLights);

			Math::Matrix		ComputePerspectiveMatrix(bool isInverted) const;
			Math::Matrix		ComputeOrthogonalMatrix(bool isInverted) const;

			void SortTransparentMeshRenderQueue(const Core::Transform& transform, const Manager::MeshManager& meshMgr, const Core::TransformPool& transformPool);

			GET_ACCESSOR(FieldOfViewDegree,			float,			_desc.fieldOfViewDegree);
			GET_ACCESSOR(Near,						float,			_desc.near);
			GET_ACCESSOR(Far,						float,			_desc.far);
			GET_ACCESSOR(ClearColor,				const Color&,	_desc.clearColor);
			GET_CONST_ACCESSOR(gamma,				float,			_tbrCBData.gamma);
			GET_ACCESSOR(Initialized,				bool,			_Initialized);
			SET_ACCESSOR_DIRTY(FieldOfViewDegree,	float,			_desc.fieldOfViewDegree);
			SET_ACCESSOR_DIRTY(Near,				float,			_desc.near);
			SET_ACCESSOR_DIRTY(Far,					float,			_desc.far);
			SET_ACCESSOR_DIRTY(ClearColor,			const Color&,	_desc.clearColor);
			SET_ACCESSOR_DIRTY(Gamma,				float,			_tbrCBData.gamma);
			SET_ACCESSOR(ObjectId,					Core::ObjectId,	_objId);
			GET_CONST_ACCESSOR(RenderRect,			const auto&,	_desc.renderRect);
			GET_ACCESSOR(GBuffers,					auto&,			_gbuffer);

			GET_ACCESSOR(CameraCB, auto&, _camCB);
			GET_ACCESSOR(TBRParamCB, auto&, _tbrCB);

		private:
			Texture::RenderTexture						_renderTarget;
			Shader::ComputeShader						_tbrShader;

			Buffer::ExplicitConstBuffer<CameraCBData>	_camCB;
			Buffer::ExplicitConstBuffer<TBRCBData>		_tbrCB;
			TBRCBData									_tbrCBData;
			CameraCBData								_camCBData;

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