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
		public:
			enum UsingTransformMatrix
			{
				OneByOne,
				AllCalculate
			};
			enum VertexShaderConstBufferUpdateType
			{
				Replace,
				Add
			};

		private:

			Structure::Vector<Material::Material> _materials;
			UsingTransformMatrix				_usingMatrix;

			const std::vector<Shader::BaseShader::BufferType>*	_optionalVertexShaderConstBuffers;			
			VertexShaderConstBufferUpdateType					_vertexShaderConstBufferUpdateType;

			const std::vector<const Texture::Texture*>*			_vertexShaderUsingTextures;
			const std::vector<Shader::BaseShader::BufferType>*	_pixelShaderUsingConstBuffer;

		public:
			MeshRenderer();
			~MeshRenderer();

		private:
			void UpdateVSBasicConstBufferData(ID3D11DeviceContext* context, std::vector<Shader::BaseShader::BufferType>& vertexShaderConstBuffers, const Core::TransformPipelineParam& transform);

		public:
			bool AddMaterial(Material::Material* material, bool copy = false);
			void UpdateAllMaterial(ID3D11DeviceContext* context, const Core::TransformPipelineParam& transform);
			bool UpdateMaterial(ID3D11DeviceContext* context, unsigned int index, const Core::TransformPipelineParam& transform);

		public:
			GET_ACCESSOR(MaterialCount, unsigned int, _materials.GetSize());
			SET_ACCESSOR(VertexShaderUsingTextures, const std::vector<const Texture::Texture*>*, _vertexShaderUsingTextures);
			SET_ACCESSOR(PixelShaderUsingConstBuffer, const std::vector<Shader::BaseShader::BufferType>*, _pixelShaderUsingConstBuffer);

			void SetOptionalVSConstBuffers(const std::vector<Shader::BaseShader::BufferType>* constBuffers, VertexShaderConstBufferUpdateType updateType);
		};
	}
}