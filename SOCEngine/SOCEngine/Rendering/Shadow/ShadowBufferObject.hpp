#pragma once

namespace Rendering
{
	namespace Shadow
	{
		namespace Buffer
		{
			template <class ShadowType>
			class ShadowBufferObject final
			{
			public:
				using ShadowBuffer = typename ShadowType::ShadowBufferType;

				ShadowBufferObject() = default;

				GET_CONST_ACCESSOR(MapResolution, uint, _mapResolution);
				SET_ACCESSOR_DIRTY(MapResolution, uint, _mapResolution);

				GET_CONST_ACCESSOR(LightCapacityInAtlas, uint, _lightCapacityInAtlas);
				SET_ACCESSOR_DIRTY(LightCapacityInAtlas, uint, _lightCapacityInAtlas);

				GET_SET_ACCESSOR(Dirty, bool, _dirty);

				ShadowBuffer& GetBuffer() { return _buffer; }

			private:
				ShadowBuffer	_buffer;

				uint			_lightCapacityInAtlas = 0;
				uint			_mapResolution = 0;

				bool			_dirty = true;
			};
		}
	}
}