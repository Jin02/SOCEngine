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
		SkyBoxMaterial(const std::string& key);

	public:
		void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr);
		void UpdateConstBuffer(Device::DirectX& dx, const Math::Matrix& transposedWVPMat);
		void UpdateCubeMap(TextureBindIndex bind, const Texture::Texture2D& tex);

		GET_CONST_ACCESSOR(Shaders, const Shader::ShaderGroup&, _shader);
		GET_CONST_ACCESSOR(ConstBuffer, const Buffer::ExplicitConstBuffer<Math::Matrix>&, _wvpCB);

	protected:
		SET_ACCESSOR(Shaders, const Shader::ShaderGroup&, _shader);
		SET_ACCESSOR(ConstBuffer, const Buffer::ExplicitConstBuffer<Math::Matrix>&, _wvpCB);

	private:
		Shader::ShaderGroup							_shader;
		Buffer::ExplicitConstBuffer<Math::Matrix>	_wvpCB;
	};
}