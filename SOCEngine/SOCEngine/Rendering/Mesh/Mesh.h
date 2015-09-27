#pragma once

#include "MeshFilter.h"
#include "MeshRenderer.h"
#include "MeshBasicData.h"
#include "Component.h"

namespace Rendering
{
	namespace Mesh
	{
		class Mesh : public Core::Component
		{
		public:
			static const Core::Component::Type GetComponentType() {	return Core::Component::Type::Mesh;	}
			enum MaterialUpdateType
			{
				One, All
			};

		private:
			MeshFilter*							_filter;
			MeshRenderer*						_renderer;
			Buffer::ConstBuffer*				_transformConstBuffer;

			MaterialUpdateType					_updateType;
			unsigned int						_selectMaterialIndex;
			MeshRenderer::Type					_prevRenderType;

			Core::TransformPipelineShaderInput	_prevTransformData;

		public:
			Mesh();
			~Mesh();

		public:
			struct CreateFuncArguments : public MeshFilter::CreateFuncArguments
			{
				Material* material;
				CreateFuncArguments(const std::string& fileName, const std::string& key):
					MeshFilter::CreateFuncArguments(fileName, key){}
				~CreateFuncArguments() {}
			};

			void Initialize(const CreateFuncArguments& args);
			void Initialize(Rendering::Buffer::VertexBuffer*& vertexBuffer, Rendering::Buffer::IndexBuffer*& indexBuffer, Rendering::Material*& initMaterial);

		private:
			void ClassifyRenderMeshType();

		public:
			virtual void OnInitialize();
			virtual void OnUpdate(float deltaTime);
			virtual void OnUpdateTransformCB(const Core::TransformPipelineShaderInput& transpose_Transform);
			virtual void OnRenderPreview();
			virtual void OnDestroy();

		public:
			GET_ACCESSOR(MeshFilter, MeshFilter*, _filter);
			GET_ACCESSOR(MeshRenderer, MeshRenderer*, _renderer);

			GET_SET_ACCESSOR(MaterialUpdateType, MaterialUpdateType, _updateType);
			GET_SET_ACCESSOR(SelectMaterialIndex, unsigned int, _selectMaterialIndex);

			GET_ACCESSOR(TransformConstBuffer, const Buffer::ConstBuffer*, _transformConstBuffer);
			GET_ACCESSOR(PrevRenderType, MeshRenderer::Type, _prevRenderType);

		public:
			virtual Core::Component* Clone() const;
		};
	}
}