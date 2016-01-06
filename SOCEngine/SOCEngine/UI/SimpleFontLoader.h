#pragma once

#include <array>
#include <fstream>
#include "Texture2D.h"
#include "Singleton.h"

#define BASIC_FONT_COUNT 96
#define TEMP_FONT_DATA_PATH "../../SOCEngine/SOCEngine/Rendering/UI/Resource/fontdata.txt"
#define TEMP_FONT_TEXTURE_PATH "../../SOCEngine/SOCEngine/Rendering/UI/Resource/font.dds"

namespace UI
{
	class SimpleFontLoader
	{
	private:
		struct FontType
		{
			float left, right;
			int size;
		};

		std::array<FontType, BASIC_FONT_COUNT>	_font;
		Rendering::Texture::Texture2D*			_texture;
		Math::Size<uint>						_fontTextureSize;

	public:
		SimpleFontLoader();
		~SimpleFontLoader();

	private:
		bool LoadFontData(const std::string& filePath);
		bool LoadTexture(const std::string& texturePath);

	public:
		const SimpleFontLoader* Initialize(const std::string& fontDataFilePath, const std::string& fontTexturePath);
		void Destroy();

	public:
		GET_ACCESSOR(Texture, const Rendering::Texture::Texture2D*, _texture);
		GET_ACCESSOR(FontTextureSize, const Math::Size<uint>&, _fontTextureSize);
		inline const std::array<FontType, BASIC_FONT_COUNT>& GetFonts() const { return _font; }
	};
}
