#pragma once

#include "BasicMaterial.h"
#include "Structure.h"
#include "TransformPipelineParam.h"

namespace Rendering
{
	namespace Mesh
	{
		class MeshRenderer
		{
		private:
			Structure::Vector<std::string, BasicMaterial>	_materials;

		private:
			BasicMaterial*	_depthWriteMaterial;
			BasicMaterial*	_alphaTestMaterial;

		public:
			MeshRenderer(BasicMaterial* depthWriteMaterial = nullptr, BasicMaterial*	alphaTestMaterial = nullptr);
			~MeshRenderer();

		public:
			bool AddMaterial(BasicMaterial* material, bool copy = false);

			void UpdateAllMaterial(ID3D11DeviceContext* context, const Buffer::ConstBuffer* transformBuffer, const Buffer::ConstBuffer* camera);
			bool UpdateMaterial(ID3D11DeviceContext* context, unsigned int index, const Buffer::ConstBuffer* transformBuffer, const Buffer::ConstBuffer* camera);
			
			void ClearResource(ID3D11DeviceContext* context);
			void ClassifyMaterialWithMesh(void* mesh);

		public:
			GET_ACCESSOR(MaterialCount, unsigned int, _materials.GetSize());
			GET_ACCESSOR(DepthWriteMaterial, BasicMaterial*, _depthWriteMaterial);
			GET_ACCESSOR(AlphaTestMaterial, BasicMaterial*, _alphaTestMaterial);
		};
	}
}