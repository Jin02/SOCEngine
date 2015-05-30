#pragma once

#include "CSOutputBuffer.h"

namespace Rendering
{
	namespace Light
	{
		class LightCulling_CSOutputBuffer : public GPGPU::DirectCompute::CSOutputBuffer
		{
		private:
			ID3D11ShaderResourceView*	_srv;

		public:
			LightCulling_CSOutputBuffer();
			virtual ~LightCulling_CSOutputBuffer();

		public:
			void Initialize(const Math::Size<unsigned int>& threadSize, unsigned int maxLightNumInTile);

		public:
			GET_ACCESSOR(ShaderResourceView, ID3D11ShaderResourceView*, _srv);
		};
	}
}