#pragma once

#include "RenderTexture.h"
#include "UnorderedAccessView.h"

namespace GPGPU
{
	namespace DirectCompute
	{
		class CSRWTexture : public Rendering::Shader::UnorderedAccessView
		{
		private:
			Rendering::Texture::RenderTexture*		_renderTexture;

		public:
			CSRWTexture();
			virtual ~CSRWTexture();

		public:
			void Initialize(const Math::Size<uint>& size, DXGI_FORMAT format, uint optionalBindFlags);
			void Destroy();

		public:
			GET_ACCESSOR(RenderTexture, const Rendering::Texture::RenderTexture*, _renderTexture);
		};
	}
}
