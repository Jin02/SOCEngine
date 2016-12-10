#pragma once

#include "Lights.h"
#include "Frustum.h"
#include "VectorHashMap.h"
#include "ShaderResourceBuffer.h"

#include <functional>

namespace Rendering
{
	namespace Manager
	{
		class LightManager
		{
		public:
			struct Lights
			{
				uint				prevTransformUpdateCounter;
				Light::LightForm*	light;

				Lights(Light::LightForm* _light, uint prevTFCounter) : light(_light), prevTransformUpdateCounter(prevTFCounter) {}
				~Lights(){}
			};

		private:
			Structure::VectorHashMap<address, Lights>										_lights;
			Structure::VectorHashMap<address, Light::DirectionalLight*>						_directionalLights; // using for compute frustum

			Structure::VectorHashMap<address, Light::LightForm::LightTransformBuffer>		_pointLightTransformBuffer;
			Buffer::ShaderResourceBuffer*													_pointLightTransformSRBuffer;
			Structure::VectorHashMap<address, uint>											_pointLightColorBuffer;
			Buffer::ShaderResourceBuffer*													_pointLightColorSRBuffer;
			Structure::VectorHashMap<address, uint>											_pointLightOptionalParamIndexBuffer;
			Buffer::ShaderResourceBuffer*													_pointLightOptionalParamIndexSRBuffer;

			Structure::VectorHashMap<address, std::pair<ushort, ushort>>					_directionalLightDirBuffer;
			Buffer::ShaderResourceBuffer*													_directionalLightDirSRBuffer;
			Structure::VectorHashMap<address, uint>											_directionalLightColorBuffer;
			Buffer::ShaderResourceBuffer*													_directionalLightColorSRBuffer;
			Structure::VectorHashMap<address, uint>											_directionalLightOptionalParamIndexBuffer;
			Buffer::ShaderResourceBuffer*													_directionalLightOptionalParamIndexSRBuffer;

			Structure::VectorHashMap<address, Light::LightForm::LightTransformBuffer>		_spotLightTransformBuffer;
			Buffer::ShaderResourceBuffer*													_spotLightTransformSRBuffer;
			Structure::VectorHashMap<address, Light::SpotLight::Param>						_spotLightParamBuffer;
			Buffer::ShaderResourceBuffer*													_spotLightParamSRBuffer;
			Structure::VectorHashMap<address, uint>											_spotLightColorBuffer;
			Buffer::ShaderResourceBuffer*													_spotLightColorSRBuffer;
			Structure::VectorHashMap<address, uint>											_spotLightOptionalParamIndexBuffer;
			Buffer::ShaderResourceBuffer*													_spotLightOptionalParamIndexSRBuffer;

			BufferUpdateType																_pointLightBufferUpdateType;
			BufferUpdateType																_spotLightBufferUpdateType;
			BufferUpdateType																_directionalLightBufferUpdateType;

			bool																			_forceUpdateDL;
			bool																			_forceUpdatePL;
			bool																			_forceUpdateSL;

		public:
			LightManager(void);
			~LightManager(void);

		private:
			void UpdateSRBufferUsingMapDiscard(	ID3D11DeviceContext* context,
												const std::function<ushort(const Light::LightForm*)>& getShadowIndexInEachShadowLights,
												const std::function<uchar(const Light::LightForm*)>& getLightShaftIndex);
			void UpdateSRBufferUsingMapNoOverWrite(ID3D11DeviceContext* context);

		public:
			void InitializeAllShaderResourceBuffer();
			void DestroyAllShaderReourceBuffer();

			uint Add(Light::LightForm*& light);
			void UpdateSRBuffer(const Device::DirectX* dx,
								const std::function<ushort(const Light::LightForm*)>& getShadowIndexInEachShadowLights,
								const std::function<uchar(const Light::LightForm*)>& getLightShaftIndex);
			void ComputeDirectionalLightViewProj(const Intersection::BoundBox& sceneBoundBox, float directionalLightShadowMapResolution);

			bool Has(Light::LightForm*& light) const;
			uint FetchLightIndexInEachLights(const Light::LightForm* light) const;

			void Delete(const Light::LightForm*& light);
			void DeleteAll();
			void Destroy();

			uint GetPackedLightCount() const;
			void BindResources(const Device::DirectX* dx, bool bindVS, bool bindGS, bool bindPS) const;
			void UnbindResources(const Device::DirectX* dx, bool bindVS, bool bindGS, bool bindPS) const;

		public:
			GET_ACCESSOR(PointLightTransformSRBuffer,			const Buffer::ShaderResourceBuffer*,	_pointLightTransformSRBuffer);
			GET_ACCESSOR(PointLightColorSRBuffer,				const Buffer::ShaderResourceBuffer*,	_pointLightColorSRBuffer);
			GET_ACCESSOR(PointLightOptionalParamIndexSRBuffer,			const Buffer::ShaderResourceBuffer*,	_pointLightOptionalParamIndexSRBuffer);

			GET_ACCESSOR(DirectionalLightDirXYSRBuffer,			const Buffer::ShaderResourceBuffer*,	_directionalLightDirSRBuffer);
			GET_ACCESSOR(DirectionalLightColorSRBuffer,			const Buffer::ShaderResourceBuffer*,	_directionalLightColorSRBuffer);
			GET_ACCESSOR(DirectionalLightOptionalParamIndexSRBuffer,		const Buffer::ShaderResourceBuffer*,	_directionalLightOptionalParamIndexSRBuffer);

			GET_ACCESSOR(SpotLightTransformSRBuffer,			const Buffer::ShaderResourceBuffer*,	_spotLightTransformSRBuffer);
			GET_ACCESSOR(SpotLightParamSRBuffer,				const Buffer::ShaderResourceBuffer*,	_spotLightParamSRBuffer);
			GET_ACCESSOR(SpotLightColorSRBuffer,				const Buffer::ShaderResourceBuffer*,	_spotLightColorSRBuffer);
			GET_ACCESSOR(SpotLightOptionalParamIndexSRBuffer,			const Buffer::ShaderResourceBuffer*,	_spotLightOptionalParamIndexSRBuffer);

			GET_ACCESSOR(DirectionalLightCount,				uint,					_directionalLightColorBuffer.GetSize());
			GET_ACCESSOR(PointLightCount,					uint,					_pointLightColorBuffer.GetSize());
			GET_ACCESSOR(SpotLightCount,					uint,					_spotLightColorBuffer.GetSize());	
		};	
	}
}
