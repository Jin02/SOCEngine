#pragma once

#include "Texture3D.h"

namespace Rendering
{
	namespace GI
	{
		class AnisotropicVoxelMapAtlas : public Texture::Texture3D
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
			uint _sideLength;
			uint _mipmapCount;
			uint _uavBindIndex;

			std::vector<View::UnorderedAccessView*>	_mipmapUAVs;

		public:
			AnisotropicVoxelMapAtlas();
			~AnisotropicVoxelMapAtlas();

		public:
			void Initialize(uint sideLength, uint maxNumOfCascade, DXGI_FORMAT format, uint mipmapCount, uint uavBindIndex);
			void Destroy();

			void BindUAVsToPixelShader(const Device::DirectX* dx);
			void UnbindUAVs(const Device::DirectX* dx);

		public:
			GET_ACCESSOR(SideLength,	uint,	_sideLength);
			GET_ACCESSOR(MipMapCount,	uint,	_mipmapCount);
		};
	}
}