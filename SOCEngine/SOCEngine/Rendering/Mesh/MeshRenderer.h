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

		public:
			MeshRenderer();
			~MeshRenderer();

		public:
			bool AddMaterial(Material* material, bool copy = false);

			void UpdateAllMaterial(ID3D11DeviceContext* context,
				const Buffer::ConstBuffer* transformBuffer, const Buffer::ConstBuffer* camera);
			bool UpdateMaterial(ID3D11DeviceContext* context, unsigned int index,
				const Buffer::ConstBuffer* transformBuffer, const Buffer::ConstBuffer* camera);
			
			void ClearResource(ID3D11DeviceContext* context);
			void ClassifyMaterialWithMesh(void* mesh);

		public:
			GET_ACCESSOR(MaterialCount, unsigned int, _materials.GetSize());
		};
	}
}