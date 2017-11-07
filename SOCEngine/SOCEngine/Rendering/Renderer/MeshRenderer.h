#pragma once

#include "DefaultShaderLoader.h"
#include "MeshManager.h"
#include "MaterialManager.h"

#include "MeshRenderQueue.h"

namespace Rendering
{
	namespace Renderer
	{
		class MeshRenderer final
		{
		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr);
			void Destroy();

		public:
			struct RenderParam
			{
				const Manager::MaterialManager&	materialMgr;
				const Manager::BufferManager&	bufferMgr;
				const Buffer::ConstBuffer&		camCB;

				RenderParam(const Manager::MaterialManager& _materialMgr, const Manager::BufferManager& _bufferMgr, const Buffer::ConstBuffer& _camCB) 
					: materialMgr(_materialMgr), bufferMgr(_bufferMgr), camCB(_camCB) {}
			};

			void RenderWithoutIASetVB(Device::DirectX& dx,		RenderParam param,	DefaultRenderType renderType, Geometry::Mesh& mesh) const;
			void RenderTransparentMeshes(Device::DirectX& dx,	RenderParam param,	DefaultRenderType renderType, Rendering::RenderQueue::TransparentMeshRenderQueue& meshes) const;
			void RenderOpaqueMeshes(Device::DirectX& dx,		RenderParam param,	DefaultRenderType renderType, Rendering::RenderQueue::OpaqueMeshRenderQueue& meshes) const;
			void RenderAlphaTestMeshes(Device::DirectX& dx,	RenderParam param,	DefaultRenderType renderType, Rendering::RenderQueue::AlphaTestMeshRenderQueue& meshes) const { RenderOpaqueMeshes(dx, param, renderType, meshes); }

		private:
			Manager::DefaultShaderLoader _loader;
		};
	}
}