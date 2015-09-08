#pragma once

#include "DirectX.h"
#include <string>
#include "Structure.h"
#include "Texture2D.h"

#include <hash_map>

namespace Rendering
{
	namespace Manager
	{
		class TextureManager
		{
		private:
			std::hash_map<std::string, Texture::Texture2D*> _hash;

		public:
			TextureManager();
			~TextureManager();

		public:
			Texture::Texture2D* LoadTextureFromFile(const std::string& fileDir, bool hasAlpha);
			Texture::Texture2D* Find(const std::string& name);
			void Remoave(const std::string& name);
			void RemoveAll();
		};
	}
}