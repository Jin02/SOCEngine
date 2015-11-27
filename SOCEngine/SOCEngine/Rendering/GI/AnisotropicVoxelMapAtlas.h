#pragma once

#include "VoxelMap.h"

namespace Rendering
{
	namespace GI
	{
		class AnisotropicVoxelMapAtlas
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
			VoxelMap*	_voxelMapAtlas;

		private:
			uint _sideLength;
			uint _mipmapLevels;

		public:
			AnisotropicVoxelMapAtlas();
			~AnisotropicVoxelMapAtlas();

		public:
			void Initialize(uint sideLength, uint maxNumOfCascade, DXGI_FORMAT format, uint mipmapLevels);
			void Destroy();

			void BindUAVsToPixelShader(const Device::DirectX* dx, const uint bindSlotOffset);
			void UnbindUAVs(const Device::DirectX* dx, const uint bindSlotOffset);

		public:
			GET_ACCESSOR(SideLength, uint, _sideLength);
			GET_ACCESSOR(MipmapLabels, uint, _mipmapLevels);
		};
	}
}