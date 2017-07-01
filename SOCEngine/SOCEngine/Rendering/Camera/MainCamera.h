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
	namespace Manager
	{
		class CameraManager;
	}

	namespace Camera
	{
		class MainCamera final
		{
		public:
			using ManagerType = Manager::CameraManager;

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

		public:
			MainCamera(Core::ObjectId objId) : _objId(objId) {}
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Rect<uint>& rect);
			bool UpdateCB(Device::DirectX& dx, const Core::Transform& transform);

			Math::Matrix		ComputePerspectiveMatrix(bool isInverted) const;
			Math::Matrix		ComputeOrthogonalMatrix(bool isInverted) const;

			void SortTransparentMeshRenderQueue(const Core::Transform& transform, const Manager::MeshManager& meshMgr, const Core::TransformPool& transformPool);

			GET_ACCESSOR(FieldOfViewDegree,			float,			_desc.fieldOfViewDegree);
			GET_ACCESSOR(Near,						float,			_desc.near);
			GET_ACCESSOR(Far,						float,			_desc.far);
			GET_ACCESSOR(ClearColor,				const Color&,	_desc.clearColor);
			GET_ACCESSOR(Initialized,				bool,			_Initialized);
			SET_ACCESSOR_DIRTY(FieldOfViewDegree,	float,			_desc.fieldOfViewDegree);
			SET_ACCESSOR_DIRTY(Near,				float,			_desc.near);
			SET_ACCESSOR_DIRTY(Far,					float,			_desc.far);
			SET_ACCESSOR_DIRTY(ClearColor,			const Color&,	_desc.clearColor);
			SET_ACCESSOR(ObjectId,					Core::ObjectId,	_objId);
			GET_CONST_ACCESSOR(RenderRect,			const auto&,	_desc.renderRect);
			GET_CONST_ACCESSOR(ViewProjMatrix,		const auto&,	_viewProjMat);
			GET_CONST_ACCESSOR(ProjMatrix,			const auto&,	_projMat);
			GET_ACCESSOR(CameraCB,					auto&,			_camCB);
			GET_CONST_ACCESSOR(Dirty,				bool,			_dirty);

		private:
			Buffer::ExplicitConstBuffer<CameraCBData>	_camCB;
			CameraCBData								_camCBData;

			Math::Matrix								_projMat;
			Math::Matrix								_viewProjMat;

			Math::Matrix								_prevViewProjMat;
			Intersection::Frustum						_frustum;
			TransformCB::ChangeState					_camCBChangeState = TransformCB::ChangeState::HasChanged;
			Desc										_desc;

			std::vector<const Geometry::Mesh*>			_transparentMeshes;

			Core::ObjectId								_objId;
			bool										_dirty = true;
			bool										_Initialized = false;
		};
	}
}