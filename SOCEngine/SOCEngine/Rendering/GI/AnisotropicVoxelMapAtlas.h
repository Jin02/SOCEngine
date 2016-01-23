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
			virtual ~AnisotropicVoxelMapAtlas();

		public:
			void Initialize(uint sideLength, uint maxNumOfCascade, DXGI_FORMAT typelessFormat, DXGI_FORMAT srvFormat, DXGI_FORMAT uavFormat, uint mipmapCount);
			void Destroy();

		public:
			GET_ACCESSOR(SideLength,		uint,		_sideLength);
			GET_ACCESSOR(MaxMipmapLevel,	uint,		_mipmapCount-1);
			GET_ACCESSOR(MipmapCount,		uint,		_mipmapCount);

			GET_ACCESSOR(SourceMapUAV, const View::UnorderedAccessView*, _uav);
			inline const View::UnorderedAccessView* GetMipmapUAV(uint index) const { return _mipmapUAVs[index]; }
		};
	}
}