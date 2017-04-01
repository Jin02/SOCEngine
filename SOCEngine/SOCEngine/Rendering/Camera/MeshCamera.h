#pragma once

#include "DirectX.h"
#include "Mesh.h"
#include "Frustum.h"
#include "RenderTexture.h"
#include "ConstBuffer.h"
#include "Transform.h"
#include "TransformCB.h"
#include "MeshManager.hpp"
#include <functional>
#include <memory>
#include "GBufferInfo.h"

#undef near
#undef far

namespace Rendering
{
	namespace Camera
	{
		class MeshCamera final
		{
		public:
			struct CameraCBData
			{
				Math::Matrix	viewMat;
				Math::Matrix	viewProjMat;
				Math::Matrix	prevViewProjMat;

				Math::Vector3	worldPos			= Math::Vector3(0.0f, 0.0f, 0.0f);
				uint			packedCamNearFar	= 0;
			};
			struct Param
			{
				Intersection::Frustum					frustum;
				float									fieldOfViewDegree	= 45.0f;
				Half									near				= Half(0.1f);
				Half									far					= Half(1000.0f);
				float									aspect				= 1.0f;
				Color									clearColor			= Color::Black();
				Rect<float>								renderRect			= Rect<float>(0.0f, 0.0f, 0.0f, 0.0f);
				float									gamma				= 2.2f;
			};

		private:
			Math::Matrix								_viewProjMat;
			Math::Matrix								_prevViewProjMat;

			Buffer::ExplicitConstBuffer<CameraCBData>	_camCB;
			TransformCB::ChangeState					_camCBChangeState = TransformCB::ChangeState::HasChanged;

			Texture::RenderTexture						_renderTarget;
			GBuffers									_gbuffer;

			Core::ObjectId								_id;
			std::shared_ptr<Param>						_param = nullptr;
			bool										_dirty = true;

			using ConstMeshRef = std::shared_ptr<const Geometry::Mesh>;
			std::vector<ConstMeshRef>					_transparentMeshes;

		public:
			MeshCamera(Core::ObjectId);

			void Initialize(Device::DirectX& dx, const Rect<float>& renderRect, bool useMipmap);

			void SortTransparentMeshRenderQueue(const Core::Transform& transform, const Manager::MeshManager& meshMgr, const Core::TransformPool& transformPool);

			void		ComputePerspectiveMatrix(Math::Matrix &outMatrix, bool isInverted) const;
			void		ComputeOrthogonalMatrix(Math::Matrix &outMatrix, bool isInverted) const;
			static void ComputeViewMatrix(Math::Matrix &outMatrix, const Math::Matrix &worldMatrix);
			static void ComputeViewportMatrix(Math::Matrix& outMat, const Rect<float>& rect);
			static void ComputeInvViewportMatrix(Math::Matrix& outMat, const Rect<float>& rect);

			void UpdateCB(Device::DirectX& dx, const Core::Transform& transform);

			GET_ACCESSOR(ObjectId, const Core::ObjectId&, _id);
		};
	}
}