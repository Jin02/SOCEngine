#pragma once

#include "Material.h"
#include "Structure.h"
#include "TransformPipelineParam.h"

namespace Rendering
{
	namespace Mesh
	{
		class MeshRenderer
		{
		private:
			Structure::Vector<std::string, Material>	_materials;

		private:
			Material*	_depthWriteMaterial;
			Material*	_alphaTestMaterial;

		public:
			MeshRenderer(Material* depthWriteMaterial = nullptr, Material*	alphaTestMaterial = nullptr);
			~MeshRenderer();

		public:
			bool AddMaterial(Material* material, bool copy = false);

			void UpdateAllMaterial(ID3D11DeviceContext* context, Buffer::ConstBuffer* transformBuffer);
			bool UpdateMaterial(ID3D11DeviceContext* context, unsigned int index, Buffer::ConstBuffer* transformBuffer);
			
			void ClearResource(ID3D11DeviceContext* context);
			void ClassifyMaterialWithMesh(void* mesh);

		public:
			GET_ACCESSOR(MaterialCount, unsigned int, _materials.GetSize());
			GET_ACCESSOR(DepthWriteMaterial, Material*, _depthWriteMaterial);
			GET_ACCESSOR(AlphaTestMaterial, Material*, _alphaTestMaterial);
		};
	}
}