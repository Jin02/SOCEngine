#pragma once

#include "Common.h"
#include "ShaderResourceView.h"
#include "UnorderedAccessView.h"

namespace Rendering
{
	namespace Texture
	{
		class TextureForm
		{
		public:
			enum class Type : uint
			{
				Unknown,
				Tex2D,
				Tex3D
			};

		private:
			Type _type;

		protected:
			View::ShaderResourceView*	_srv;
			View::UnorderedAccessView*	_uav;
	
		public:
			TextureForm(Type type);
			virtual ~TextureForm();

			void Destroy();

		public:
			GET_ACCESSOR(Type, Type, _type);

			GET_ACCESSOR(ShaderResourceView,	const View::ShaderResourceView*,	_srv);
			GET_ACCESSOR(UnorderedAccessView,	const View::UnorderedAccessView*,	_uav);
		};
	}
}