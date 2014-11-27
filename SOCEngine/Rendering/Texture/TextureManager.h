#pragma once

#include "DirectX.h"
#include <string>
#include "Structure.h"
#include "Texture.h"

namespace Rendering
{
	namespace Texture
	{
		class TextureManager
		{
		private:
			Structure::HashMap<Texture> _hash;

		public:
			TextureManager();
			~TextureManager();

		public:
			static bool LoadTextureFromFile(ID3D11ShaderResourceView** outShaderResourceView, const std::string& fileDir);

		public:
			Texture* LoadTextureFromFile(const std::string& fileDir);
			Texture* Find(const std::string& name);
			void Remoave(const std::string& name);
			void RemoveAll();
		};
	}
}