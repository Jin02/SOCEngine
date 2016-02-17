#pragma once

#include "InjectRadiance.h"

namespace Rendering
{
	namespace GI
	{
		class InjectRadianceFromPointLIght : public InjectRadiance
		{
		public:
			InjectRadianceFromPointLIght();
			~InjectRadianceFromPointLIght();

		public:
			void Initialize(const InjectRadiance::InitParam& initParam);
			void Inject(const Device::DirectX*& dx, const Shadow::ShadowRenderer*& shadowMgr, const Voxelization* voxelization);
		};
	}
}