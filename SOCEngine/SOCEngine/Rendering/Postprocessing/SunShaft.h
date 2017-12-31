#pragma once

#include "FullScreen.h"
#include "ConstBuffer.h"

#include "DirectionalLight.h"
#include "MainCamera.h"

#include "ObjectManager.h"
#include "MainRenderer.h"

namespace Rendering
{
	namespace PostProcessing
	{
		class SunShaft final
		{
		public:
			struct Param
			{
				ushort			bufferSizeW	= 0;
				ushort			bufferSizeH	= 0;

				Half			sunUV_X		= Half(-1.0f);
				Half			sunUV_Y		= Half(-1.0f);

				ushort			dlIndex		= -1;
				Half			aspect		= Half(1.0f);

				Half			size		= Half(0.4f);
				Half			intensity	= Half(1.0f);
			};

			GET_CONST_ACCESSOR(CircleIntensity,	float, _paramCBData.intensity);
			SET_ACCESSOR_DIRTY(CircleIntensity,	float, _paramCBData.intensity);
			GET_CONST_ACCESSOR(Size,			float, _paramCBData.size);
			SET_ACCESSOR_DIRTY(Size,			float, _paramCBData.size);

			inline void SetDirectionalLightID(Core::ObjectID id) { _dlObjID = id; _changedDL = true; }

		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Camera::MainCamera& mainCam);
			void UpdateParamCB(Device::DirectX& dx, const Core::ObjectManager& objMgr, const Manager::LightManager& lightMgr, const Core::TransformPool& tfPool, const Camera::MainCamera& mainCam);
			void Render(Device::DirectX& dx, Texture::RenderTexture& resultMap, const Texture::Texture2D& inputColorMap, const Renderer::MainRenderer& renderer, const Camera::MainCamera& mainCamera, const Manager::LightManager& lightMgr);
			void Destroy();

		private:
			FullScreen							_occlusionMapScreen;
			FullScreen							_shaftScreen;
			Texture::RenderTexture				_occlusionMap;

		private:
			Param								_paramCBData;
			Buffer::ExplicitConstBuffer<Param>	_paramCB;
			Core::ObjectID						_dlObjID;

		private:
			bool								_renderAble	= false;
			bool								_dirty		= true;
			bool								_changedDL	= true;
		};
	}
}