#pragma once

#include "InjectRadiance.h"

namespace Rendering
{
	namespace GI
	{
		class InjectRadianceFromDirectionalLIght : private InjectRadiance
		{
		public:
			InjectRadianceFromDirectionalLIght();
			~InjectRadianceFromDirectionalLIght();

		public:
			void Initialize(const Buffer::ConstBuffer*& giInfoConstBuffer);
		};
	}
}