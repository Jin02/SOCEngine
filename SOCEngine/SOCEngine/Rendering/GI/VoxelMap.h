#pragma once

#include "Texture3D.h"
#include "UnorderedAccessView.h"

namespace Rendering
{
	namespace GI
	{
		class VoxelMap : public Texture::Texture3D
		{
		private:
			Shader::UnorderedAccessView*	_uav;

		public:
			VoxelMap();
			virtual ~VoxelMap();

		public:
			void Initialize(const Math::Size<unsigned int>& size, uint depth, DXGI_FORMAT format);
			void Destroy();
		};
	}
}