#pragma once

#include "DirectX.h"
#include <string>
#include "Structure.h"
#include "Texture.h"

namespace Rendering
{
	namespace Manager
	{
		class TextureManager
		{
		private:
			Structure::HashMap<Texture::Texture> _hash;

		public:
			TextureManager();
			~TextureManager();

		public:
			static bool LoadTextureFromFile(ID3D11ShaderResourceView** outShaderResourceView, const std::string& fileDir);

		public:
			Texture::Texture* LoadTextureFromFile(const std::string& fileDir);
			Texture::Texture* Find(const std::string& name);
			void Remoave(const std::string& name);
			void RemoveAll();
		};
	}
}