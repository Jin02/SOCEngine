#pragma once

#include "VXGI.h"
#include "GIInitParam.h"

namespace Rendering
{
	namespace Renderer
	{
		class GlobalIllumination final
		{
		public:
			GET_ACCESSOR(VXGIResultMap, const Texture::Texture2D&, _vxgi.GetIndirectColorMap());
			void SetVXGI_VoxelizeCenterPos(const Math::Vector3& pos) { _vxgi.SetStartCenterWorldPos(pos); }

		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Size<uint>& renderSize, const GIInitParam& giParam);
			void Run(Device::DirectX& dx, const GI::VXGI::Param&& param);

		private:
			GI::VXGI _vxgi;
		};
	}
}