#pragma once

#include "Half.h"
#include "ComputeShader.h"
#include "Transform.h"
#include "ShaderManager.h"
#include "MeshManager.h"
#include "Frustum.h"
#include "RenderTexture.h"
#include "DepthMap.h"
#include "ObjectID.hpp"
#include "MeshRenderQueue.h"

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

				Math::Vector3	worldPos			= Math::Vector3(0.0f, 0.0f, 0.0f);
				uint			packedCamNearFar	= 0;
			};
			struct Desc
			{				
				float			fieldOfViewDegree	= 45.0f;
				float			near				= 0.1f;
				float			far					= 1000.0f;
				float			aspect				= 1.0f;
				Color			clearColor			= Color::Black();
				Rect<uint>		renderRect			= Rect<uint>(0, 0, 0, 0);
			};

		public:
			MainCamera(Core::ObjectID objID) : _objID(objID) {}
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Rect<uint>& rect);
			bool UpdateCB(Device::DirectX& dx, const Core::Transform& transform);

			Math::Matrix ComputePerspectiveMatrix(bool isInverted) const;
			Math::Matrix ComputeOrthogonalMatrix(bool isInverted) const;

			GET_ACCESSOR(FieldOfViewDegree,				float,			_desc.fieldOfViewDegree);
			GET_ACCESSOR(Near,							float,			_desc.near);
			GET_ACCESSOR(Far,							float,			_desc.far);
			GET_ACCESSOR(ClearColor,					const Color&,	_desc.clearColor);
			SET_ACCESSOR_DIRTY(FieldOfViewDegree,		float,			_desc.fieldOfViewDegree);
			SET_ACCESSOR_DIRTY(Near,					float,			_desc.near);
			SET_ACCESSOR_DIRTY(Far,						float,			_desc.far);
			SET_ACCESSOR_DIRTY(ClearColor,				const Color&,	_desc.clearColor);
			GET_CONST_ACCESSOR(ObjectID,				Core::ObjectID,	_objID);
			SET_ACCESSOR(ObjectID,						Core::ObjectID,	_objID);
			GET_CONST_ACCESSOR(RenderRect,				const auto&,	_desc.renderRect);
			GET_CONST_ACCESSOR(ViewProjMatrix,			const auto&,	_viewProjMat);
			GET_CONST_ACCESSOR(ProjMatrix,				const auto&,	_projMat);
			GET_ACCESSOR(CameraCB,						auto&,			_camCB);
			GET_CONST_ACCESSOR(Dirty,					bool,			_dirty);
			GET_CONST_ACCESSOR(Frustum,					const auto&,	_frustum);

			GET_CONST_ACCESSOR(TransparentMeshRenderQ,	const auto&,	_transparentMeshRenderQ);
			GET_CONST_ACCESSOR(OpaqueMeshRenderQ,		const auto&,	_opaqueMeshRenderQ);
			GET_CONST_ACCESSOR(AlphaTestMeshRenderQ,	const auto&,	_alphaTestMeshRenderQ);

			void ClassifyMesh(Geometry::MeshPoolRefs pool, const Core::ObjectManager& objMgr, const Core::TransformPool& transformPool);

		private:
			Buffer::ExplicitConstBuffer<CameraCBData>	_camCB;
			CameraCBData								_camCBData;

			Math::Matrix								_projMat;
			Math::Matrix								_viewProjMat;

			Math::Matrix								_prevViewProjMat;
			Intersection::Frustum						_frustum;
			TransformCB::ChangeState					_camCBChangeState = TransformCB::ChangeState::HasChanged;
			Desc										_desc;

			RenderQueue::TransparentMeshRenderQueue		_transparentMeshRenderQ;
			RenderQueue::OpaqueMeshRenderQueue			_opaqueMeshRenderQ;
			RenderQueue::AlphaTestMeshRenderQueue		_alphaTestMeshRenderQ;

			Core::ObjectID								_objID;
			bool										_dirty = true;
		};
	}
}
