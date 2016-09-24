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
			void Initialize();
			void Inject(const Device::DirectX*& dx, const Manager::LightManager* lightMgr, const Shadow::ShadowRenderer*& shadowMgr, const InjectRadiance::DispatchParam& param);
		};
	}
}