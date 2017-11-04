#pragma once

#include "DefaultShaderLoader.h"
#include "MeshManager.hpp"
#include "MaterialManager.hpp"

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
				Device::DirectX&			dx;
				Manager::MaterialManager&	materialMgr;
				Manager::BufferManager&		bufferMgr;
				Buffer::ConstBuffer&		camCB;

				RenderParam(Device::DirectX& _dx, Manager::MaterialManager& _materialMgr, Manager::BufferManager& _bufferMgr, Buffer::ConstBuffer& _camCB) 
					: dx(_dx), materialMgr(_materialMgr), bufferMgr(_bufferMgr), camCB(_camCB) {}
			};

			void RenderWithoutIASetVB(RenderParam param,	DefaultRenderType renderType, Geometry::Mesh& mesh) const;
			void RenderTransparentMeshes(RenderParam param,	DefaultRenderType renderType, Rendering::RenderQueue::TransparentMeshRenderQueue& meshes) const;
			void RenderOpaqueMeshes(RenderParam param,		DefaultRenderType renderType, Rendering::RenderQueue::OpaqueMeshRenderQueue& meshes) const;
			void RenderAlphaBlendMeshes(RenderParam param,	DefaultRenderType renderType, Rendering::RenderQueue::AlphaBlendMeshRenderQueue& meshes) const { RenderOpaqueMeshes(param, renderType, meshes); }

		private:
			Manager::DefaultShaderLoader _loader;
		};
	}
}