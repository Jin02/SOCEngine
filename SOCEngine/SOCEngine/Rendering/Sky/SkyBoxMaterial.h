#pragma once

#include "DirectX.h"
#include "Matrix.h"
#include "ShaderManager.h"
#include "Material.h"

namespace Rendering
{
	class SkyBoxMaterial : public Material
	{
	public:
		using Material::Material;

		void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr);
		void UpdateConstBuffer(Device::DirectX& dx, const Math::Matrix& transposedWVPMat);
		void UpdateCubeMap(TextureBindIndex bind, const Texture::Texture2D& tex);

		GET_CONST_ACCESSOR(ConstBuffer, const Buffer::ExplicitConstBuffer<Math::Matrix>&, _wvpCB);
		GET_CONST_ACCESSOR(VertexShader, const Shader::VertexShader&, _vs);
		GET_CONST_ACCESSOR(PixelShader, const Shader::PixelShader&, _ps);

	private:
		Shader::VertexShader				_vs;
		Shader::PixelShader				_ps;
		Buffer::ExplicitConstBuffer<Math::Matrix>	_wvpCB;
	};
}
