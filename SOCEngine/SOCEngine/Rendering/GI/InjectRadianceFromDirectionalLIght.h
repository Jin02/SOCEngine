#pragma once

#include "InjectRadiance.h"

namespace Rendering
{
	namespace GI
	{
		class InjectRadianceFromDirectionalLIght : public InjectRadiance
		{
		public:
			InjectRadianceFromDirectionalLIght();
			~InjectRadianceFromDirectionalLIght();

		public:
			void Initialize(const InjectRadiance::InitParam& initParam);
			void Inject(const Device::DirectX*& dx, const Shadow::ShadowRenderer*& shadowMgr,
						const std::vector<Buffer::ConstBuffer*>& voxelizationInfoConstBuffers);
		};
	}
}