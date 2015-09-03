#pragma once

#include "RenderingCommon.h"
#include "ShaderForm.h"
#include "Mesh.h"
#include <functional>

namespace Rendering
{
	namespace Renderer
	{
		class TransparencyRenderer
		{
		protected:
			std::vector<Shader::ShaderForm::InputConstBuffer>			_inputConstBuffers;
			std::vector<Shader::ShaderForm::InputTexture>				_inputTextures;
			std::vector<Shader::ShaderForm::InputShaderResourceBuffer>	_inputSRBuffers;

		public:
			TransparencyRenderer();
			~TransparencyRenderer();

		public:
			void Initialize();
			void Destroy();

		public:
			void Render(ID3D11RenderTargetView* outResultRT,
				const std::function<void(const std::vector<const Mesh::Mesh*>& meshes, RenderType renderType)>& renderMesh);
		};
	}
}