#pragma once

#include "DirectX.h"
#include "Matrix.h"
#include "ShaderManager.h"
#include "MaterialForm.hpp"

namespace Rendering
{
	namespace Material
	{
		class SkyBoxMaterial : public MaterialForm
		{
		public:
			using MaterialForm::MaterialForm;

			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr);
			void UpdateCubeMap(const Texture::Texture2D& tex);

			GET_CONST_ACCESSOR(VertexShader,	const auto&,	_vertexShader);
			GET_CONST_ACCESSOR(PixelShader,		const auto&,	_pixelShader);
			GET_CONST_ACCESSOR(CubeMap,			const auto&,	_cubeMap);
			GET_CONST_ACCESSOR(MaxMipLevel,		float,			_maxMipLevel);

		private:
			Shader::VertexShader	_vertexShader;
			Shader::PixelShader		_pixelShader;

			Texture::Texture2D		_cubeMap;
			float					_maxMipLevel	= 0.0f;
		};
	}
}
