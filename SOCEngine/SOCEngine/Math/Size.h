#pragma once

namespace Math
{
	template <typename Type>
	struct Size
	{
	public:
		Type w, h;

	public:
		Size(Type w = 0, Type h = 0)
		{
			this->w = w;
			this->h = h;
		}

		template<typename CastType>
		inline Size<CastType> Cast() const
		{
			return Size<CastType>((CastType)this->w, (CastType)this->h);
		}
	};

}