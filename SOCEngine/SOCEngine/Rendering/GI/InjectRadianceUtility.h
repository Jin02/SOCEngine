#pragma once

#include "VXGICommon.h"
#include "ShadowAtlasMapRenderer.h"
#include "Voxelization.h"

namespace Rendering
{
	namespace GI
	{
		class InjectRadianceFormUtility
		{
		public:
			struct BindParam
			{
				const VXGIInfoCB&								infoCB;
				const Voxelization&								voxelization;
				const Renderer::TBRParamCB&						tbrParamCB;
				const Manager::ShadowManager::GlobalParamCB&	shadowGlobalParamCB;
			};

			static void Bind(Device::DirectX& dx, VoxelMap& outVoxelMap, const BindParam& param);
			static void UnBind(Device::DirectX& dx);
			static uint CalcThreadSideLength(uint dimension);
		};
	}
}