#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "GlobalIlluminationCommon.h"

#include "Voxelization.h"

#include "InjectRadianceFromDirectionalLIght.h"
#include "InjectRadianceFromPointLIght.h"
#include "InjectRadianceFromSpotLIght.h"

namespace Rendering
{
	namespace GI
	{
		class GlobalIllumination
		{
		private:
			GlobalInfo				_prevInfo;
			Buffer::ConstBuffer*	_giGlobalInfoCB;

		public:
			GlobalIllumination();
			~GlobalIllumination();

		public:
			void Initialize(uint dimension, float minWorldSize);
		};
	}
}