#pragma once

#include "LightForm.h"
#include "VectorMap.h"
#include "ShaderResourceBuffer.h"

namespace Rendering
{
	namespace Manager
	{
		class LightShaftManager
		{
		private:
			struct LightPack
			{
				uint prevUpdateCounter;
				uint index;
				const Light::LightForm* light;

				LightPack(uint _prevUpdateCounter, uint _index, const Light::LightForm* _light)
					: prevUpdateCounter(_prevUpdateCounter), index(_index), light(_light){}
				~LightPack(){}
			};

			Structure::VectorMap<address, LightPack>	_lights;

			std::vector<std::pair<ushort, ushort>>		_lightPosInNDCBuffer;
			Buffer::ShaderResourceBuffer*				_lightPosInNDCSRBuffer;

			std::vector<ushort>							_lightSizeBuffer;
			Buffer::ShaderResourceBuffer*				_lightSizeSRBuffer;

		public:
			LightShaftManager();
			~LightShaftManager();

		public:
			void Initialize();
			void Destroy();

		public:
			void Add(const Light::LightForm*& light);
			bool Has(const Light::LightForm*& light) const;
			void Delete(const Light::LightForm*& light);
			uint GetLightShaftIndex(const Light::LightForm*& light) const;

		public:
			void UpdateSRBuffer(const Device::DirectX* dx, const Math::Matrix& viewProjMat);

		public:
			GET_ACCESSOR(LightPosInNDCSRBuffer,	const Buffer::ShaderResourceBuffer*, _lightPosInNDCSRBuffer);
			GET_ACCESSOR(LightSizeSRBuffer,		const Buffer::ShaderResourceBuffer*, _lightSizeSRBuffer);
		};
	}
}
