#pragma once

#include "InjectRadiance.h"

namespace Rendering
{
	namespace GI
	{
		class InjectRadianceFromSpotLIght : public InjectRadiance
		{
		public:
			InjectRadianceFromSpotLIght();
			~InjectRadianceFromSpotLIght();

		public:
			void Initialize(const InjectRadiance::InitParam& initParam);
			void Inject(const Device::DirectX*& dx, const Shadow::ShadowRenderer*& shadowMgr, const Voxelization* voxelization, uint dimension, uint maximumCascade);
		};
	}
}