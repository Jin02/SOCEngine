#pragma once

#include "VoxelMap.h"

namespace Rendering
{
	namespace GI
	{
		class AnisotropicVoxelMap
		{
		public:
			enum class Direction : uint
			{
				PosX, NegX,
				PosY, NegY,
				PosZ, NegZ,
				Num
			};

		private:
			VoxelMap*	_voxelMaps[(uint)Direction::Num];

		private:
			uint _sideLength;
			uint _mipmapLevels;

		public:
			AnisotropicVoxelMap();
			~AnisotropicVoxelMap();

		public:
			void Initialize(uint sideLength, DXGI_FORMAT format, uint mipmapLevels);
			void Destroy();
			void BindUAVsToContext(const Device::DirectX* dx);

		public:
			GET_ACCESSOR(SideLength, uint, _sideLength);
			GET_ACCESSOR(MipmapLabels, uint, _mipmapLevels);
		};
	}
}