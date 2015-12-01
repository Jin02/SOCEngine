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

			std::vector<View::UnorderedAccessView*>	_mipmapUAVs;

		public:
			AnisotropicVoxelMapAtlas();
			~AnisotropicVoxelMapAtlas();

		public:
			void Initialize(uint sideLength, uint maxNumOfCascade, DXGI_FORMAT srvFormat, DXGI_FORMAT uavFormat, uint mipmapCount);
			void Destroy();

			//void BindUAVToPixelShader(const Device::DirectX* dx, uint mipLevel, uint bindIndex);
			//void UnbindUAVToPixelShader(const Device::DirectX* dx, uint bindIndex);

			inline const View::UnorderedAccessView* GetMipmapUAV(uint index) const { return (index == 0) ? _uav : _mipmapUAVs[index - 1]; }

		public:
			GET_ACCESSOR(SideLength,	uint,	_sideLength);
			GET_ACCESSOR(MipMapCount,	uint,	_mipmapCount);
		};
	}
}