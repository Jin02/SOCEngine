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
	class FontLoader : public Singleton<FontLoader>
	{
	private:
		friend class Singleton<FontLoader>;
		struct FontType
		{
			float left, right;
			int size;
		};

	private:
		std::array<FontType, BASIC_FONT_COUNT>	_font;
		Rendering::Texture::Texture2D*			_texture;
		Math::Size<uint>						_fontTextureSize;

	private:
		FontLoader();
		virtual ~FontLoader();

	private:
		bool LoadFontData(const std::string& filePath);
		bool LoadTexture(const std::string& texturePath);

	public:
		const FontLoader* Initialize(const std::string& fontDataFilePath, const std::string& fontTexturePath);

	public:
		GET_ACCESSOR(Texture, const Rendering::Texture::Texture2D*, _texture);
		GET_ACCESSOR(FontTextureSize, const Math::Size<uint>&, _fontTextureSize);
		inline const std::array<FontType, BASIC_FONT_COUNT>& GetFonts() const { return _font; }
	};
}
