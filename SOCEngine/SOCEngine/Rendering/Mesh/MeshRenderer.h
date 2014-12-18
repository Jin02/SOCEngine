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
			Structure::Vector<std::string, Material> _materials;

		public:
			MeshRenderer();
			~MeshRenderer();

		public:
			bool AddMaterial(Material* material, bool copy = false);

			void UpdateAllMaterial(ID3D11DeviceContext* context, Buffer::ConstBuffer* transformBuffer);
			bool UpdateMaterial(ID3D11DeviceContext* context, unsigned int index, Buffer::ConstBuffer* transformBuffer);

			void ClearResource(ID3D11DeviceContext* context);
			bool CheckAlphaMaterial();

		public:
			GET_ACCESSOR(MaterialCount, unsigned int, _materials.GetSize());
		};
	}
}