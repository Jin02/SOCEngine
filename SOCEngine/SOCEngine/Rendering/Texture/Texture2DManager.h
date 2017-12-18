#pragma once

#include "DirectX.h"
#include <string>
#include "Texture2D.h"
#include <unordered_map>

namespace Rendering
{
	namespace Manager
	{
		class Texture2DManager final
		{
		public:
			Texture2DManager() = default;
			DISALLOW_ASSIGN_COPY(Texture2DManager);

			using Texture2DPtr = std::shared_ptr<Texture::Texture2D>;

			Texture2DPtr LoadTextureFromFile(Device::DirectX& dx, const std::string& fileDir, bool hasAlpha);
			Texture2DPtr Find(const std::string& key);
			void Delete(const std::string& key);
			void DeleteAll();

		private:
			std::unordered_map<std::string, Texture2DPtr> _hash;
		};
	}
}