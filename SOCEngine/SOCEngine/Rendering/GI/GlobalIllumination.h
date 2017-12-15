#pragma once

#include "VXGI.h"

namespace Rendering
{
	namespace Renderer
	{
		class GlobalIllumination final
		{
		public:
			GET_ACCESSOR(VXGIResultMap, const Texture::Texture2D&, _vxgi.GetIndirectColorMap());
			void SetVXGI_VoxelizeCenterPos(const Math::Vector3& pos) { _vxgi.SetStartCenterWorldPos(pos); }

			struct InitParam
			{
				const GI::VXGIStaticInfo&& vxgiInitParam;
			};

		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Size<uint>& renderSize, const InitParam&& giParam);
			void Run(Device::DirectX& dx, const GI::VXGI::Param&& param);

		private:
			GI::VXGI _vxgi;
		};
	}
}