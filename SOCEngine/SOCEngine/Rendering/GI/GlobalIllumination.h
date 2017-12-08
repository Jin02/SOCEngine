#pragma once

#include "VXGI.h"

namespace Rendering
{
	namespace Renderer
	{
		class GlobalIllumination final
		{
		public:
			void SetVXGI_VoxelizeCenterPos(const Math::Vector3& pos) { _vxgi.SetStartCenterWorldPos(pos); }

			struct InitParam
			{
				const GI::VXGIStaticInfo&& vxgiInitParam;
			};

			GET_ACCESSOR_REF(VXGI, _vxgi);

		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Size<uint>& renderSize, const InitParam&& giParam);
			Texture::RenderTexture& Run(Device::DirectX& dx, const GI::VXGI::Param&& param);

		private:
			GI::VXGI _vxgi;
		};
	}
}