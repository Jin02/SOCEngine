#pragma once

#include "SkyGeometry.h"
#include "SkyBoxMaterial.h"
#include "Texture2DManager.h"
#include "DirectX.h"
#include "ShaderManager.h"
#include "BufferManager.hpp"
#include "MaterialManager.hpp"

namespace Rendering
{
	namespace Sky
	{
		class SkyBox
		{
		private:
			SkyGeometry			_base;
			std::string			_skyBoxMaterialKey = "";

		public:
			SkyBox() = default;
			void Initialize(Device::DirectX& dx, Manager::BufferManager& bufferMgr, const std::string& skyBoxMaterialKey);
		};
	}
}