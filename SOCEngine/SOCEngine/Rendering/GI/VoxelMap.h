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
			//View::UnorderedAccessView*	_uav;

		public:
			VoxelMap();
			virtual ~VoxelMap();

		public:
			void Initialize(uint width, uint height, uint depth, DXGI_FORMAT format);
			void Destroy();

		public:
			//GET_ACCESSOR(UnorderedAccessView, const View::UnorderedAccessView*, _uav);
		};
	}
}