#pragma once

#include "Common.h"
#include "ShaderResourceView.h"
#include "UnorderedAccessView.h"

namespace Rendering
{
	namespace Texture
	{
		class BaseTexture final
		{
		public:
			BaseTexture();
			explicit BaseTexture(const View::ShaderResourceView& srv);
			explicit BaseTexture(const View::ShaderResourceView& srv, const View::UnorderedAccessView& uav);

			void Destroy();

			SET_ACCESSOR(ShaderResourceView,	const View::ShaderResourceView&,	_srv);
			SET_ACCESSOR(UnorderedAccessView,	const View::UnorderedAccessView&,	_uav);
			GET_CONST_ACCESSOR(ShaderResourceView,	const View::ShaderResourceView&, _srv);
			GET_CONST_ACCESSOR(UnorderedAccessView,	const View::UnorderedAccessView&, _uav);

		private:
			View::ShaderResourceView	_srv;
			View::UnorderedAccessView	_uav;
		};
	}
}