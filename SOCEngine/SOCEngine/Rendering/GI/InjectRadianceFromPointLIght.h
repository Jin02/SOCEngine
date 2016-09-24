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
			void Initialize();
			void Inject(const Device::DirectX*& dx, const Manager::LightManager* lightMgr, const Shadow::ShadowRenderer*& shadowMgr, const InjectRadiance::DispatchParam& param);
		};
	}
}