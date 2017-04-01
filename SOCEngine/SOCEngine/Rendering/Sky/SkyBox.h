#pragma once

#include "BaseSky.h"
#include "SkyBoxMaterial.h"
#include "Texture2DManager.h"
#include "DirectX.h"
#include "ShaderManager.h"
#include "BufferManager.h"
#include "MaterialManager.hpp"

namespace Rendering
{
	namespace Sky
	{
		class SkyBox
		{
		private:
			BaseSky									_base;
			std::shared_ptr<SkyBoxMaterial>			_skyBoxMaterial;

		public:
			void Initialize(
				Device::DirectX& dx,
				Manager::ShaderManager& shaderMgr,
				Manager::Texture2DManager& tex2DMgr,
				Manager::BufferManager& bufferMgr,
				Manager::MaterialManager& materialMgr,
				const std::string& materialName, const std::string& cubeMapFilePath);
		};
	}
}