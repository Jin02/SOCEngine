#pragma once

#include "Material.h"

namespace Rendering
{
	class BasicMaterial : public Material
	{
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

	public:
		BasicMaterial(const std::string& name);
		BasicMaterial(const std::string& name, const Color& color);
		~BasicMaterial(void);

	public:
		void InitColorBuffer(ID3D11DeviceContext* context);
		void UpdateColorBuffer(ID3D11DeviceContext* context);
		void UpdateColor(const Color& color);

	public:

		GET_ACCESSOR(ColorBuffer, Buffer::ConstBuffer*, _colorBuffer);
		GET_ACCESSOR(Color, const Color&, _color);
	};
}