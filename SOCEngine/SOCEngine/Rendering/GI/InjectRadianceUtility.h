#pragma once

#include "VXGICommon.h"
#include "ShadowRenderer.h"
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
				VXGIInfoCB&								infoCB;
				Voxelization&							voxelization;
				Renderer::TBRParamCB&					tbrParamCB;
				Manager::ShadowManager::GlobalParamCB&	shadowGlobalParamCB;
				VoxelMap&								outVoxelMap;

				BindParam(	VXGIInfoCB& _infoCB,
							Voxelization& _voxelization,
							Renderer::TBRParamCB& _tbrParamCB,
							Manager::ShadowManager::GlobalParamCB& _shadowGlobalParamCB,
							VoxelMap& _outVoxelMap	)
					:	infoCB(_infoCB), voxelization(_voxelization), 
						tbrParamCB(_tbrParamCB), shadowGlobalParamCB(_shadowGlobalParamCB),
						outVoxelMap(_outVoxelMap)
				{

				}
			};

			static void Bind(Device::DirectX& dx, BindParam& param);
			static void UnBind(Device::DirectX& dx);
			static uint CalcThreadSideLength(uint dimension);
		};
	}
}