#pragma once

#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstBuffer.h"
#include "DepthBuffer.h"
#include "UICamera.h"
#include "MeshFilter.h"

namespace Rendering
{
	namespace PostProcessing
	{
		class BackBufferMaker
		{
		private:
			Shader::VertexShader*							_vertexShader;
			Shader::PixelShader*							_pixelShader;

			std::vector<Shader::BaseShader::BufferType>		_constBuffers;
			std::vector<Shader::BaseShader::TextureType>	_textures;

			Texture::DepthBuffer*							_depthBuffer;
			Mesh::MeshFilter*								_meshFilter;

		public:
			BackBufferMaker();
			~BackBufferMaker();

		public:
			void Initialize();
			void Destroy();

		public:
			void Render(const Device::DirectX* dx, const Camera::CameraForm* mainCamera, const Camera::UICamera* uiCamera);
		};
	}
}