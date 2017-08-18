#pragma once

#include "DefaultShaderLoader.h"
#include "MeshManager.hpp"
#include "MaterialManager.hpp"

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

				DefaultRenderType			renderType;
				Buffer::ConstBuffer&		camCB;

				RenderParam(
					Device::DirectX& _dx, Manager::MaterialManager&	_materialMgr,
					DefaultRenderType _renderType, Buffer::ConstBuffer& _camCB
				) : dx(_dx), materialMgr(_materialMgr), renderType(_renderType), camCB(_camCB) {}
			};

			void RenderWithoutIASetVB(RenderParam param, Geometry::Mesh& mesh);
			void RenderUsingSortedMeshVectorByVB(RenderParam param, Manager::MeshPool& meshes);

		private:
			Manager::DefaultShaderLoader _loader;
		};
	}
}