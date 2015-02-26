#pragma once

#include "Material.h"

namespace Rendering
{
	class BasicMaterial : public Material
	{
	public:
		enum class PSConstBufferSlot : unsigned int	{ MaterialColor	= 0	};
		enum class PSTextureSlot { Diffuse, Normal, Specular, Opacity, Ambient };

	public:
		struct Color
		{
			//a value is alpha
			Rendering::Color main;

			//a value is shiness
			Rendering::Color specular;

			Color();
			~Color();
		};

	private:
		Color					_color;
		Buffer::ConstBuffer*	_colorBuffer;

		bool					_isInit;
		uint					_colorConstBufferIdx;

	public:
		BasicMaterial(const std::string& name);
		BasicMaterial(const std::string& name, const Color& color);
		~BasicMaterial(void);

	public:
		void Init(ID3D11DeviceContext* context);
		void UpdateColorBuffer(ID3D11DeviceContext* context);
		void UpdateColor(const Color& color);

		void UpdateDiffuseMap(const Rendering::Texture::Texture* tex);
		void UpdateNormalMap(const Rendering::Texture::Texture* tex);
		void UpdateSpecularMap(const Rendering::Texture::Texture* tex);
		void UpdateOpacityMap(const Rendering::Texture::Texture* tex);
		void UpdateAmbientMap(const Rendering::Texture::Texture* tex);

	public:
		GET_ACCESSOR(ColorBuffer, Buffer::ConstBuffer*, _colorBuffer);
		GET_ACCESSOR(Color, const Color&, _color);
	};
}