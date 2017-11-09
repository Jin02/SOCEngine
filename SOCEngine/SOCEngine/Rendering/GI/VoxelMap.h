#pragma once

#include "Texture3D.h"
#include <vector>

namespace Rendering
{
	namespace GI
	{
		class VoxelMap final
		{
		public:
			enum class Direction : uint
			{
				PosX, NegX,
				PosY, NegY,
				PosZ, NegZ,
				Num
			};

		public:
			void Initialize(Device::DirectX& dx, uint sideLength, DXGI_FORMAT tex3DFormat, DXGI_FORMAT srvFormat, DXGI_FORMAT uavFormat, uint mipmapCount, bool isAnisotropic);

			GET_CONST_ACCESSOR(SideLength,		uint,		_sideLength);
			GET_CONST_ACCESSOR(MaxMipmapLevel,	uint,		_mipmapCount-1);
			GET_CONST_ACCESSOR(MipmapCount,		uint,		_mipmapCount);
			GET_ALL_ACCESSOR(Texture3D,			auto&,		_tex3D);

			GET_ALL_ACCESSOR(SourceMapUAV,		auto&,		_tex3D.GetUnorderedAccessView());	


			auto& GetMipmapUAV(uint index)				{ return _mipmapUAVs[index]; }
			const auto& GetMipmapUAV(uint index) const	{ return _mipmapUAVs[index]; }

		private:
			Texture::Texture3D						_tex3D;
			std::vector<View::UnorderedAccessView>	_mipmapUAVs;
			uint									_sideLength;
			uint									_mipmapCount;
		};
	}
}