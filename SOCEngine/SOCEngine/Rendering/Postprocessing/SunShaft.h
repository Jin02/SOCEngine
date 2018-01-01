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
			struct ParamCBData
			{
				ushort			dlIndex		= -1;
				Half			aspect		= Half(1.0f);

				Half			size		= Half(0.4f);
				Half			intensity	= Half(1.0f);

				ushort			bufferSizeW	= 0;
				ushort			bufferSizeH	= 0;

				Half			sunUV_X		= Half(-1.0f);
				Half			sunUV_Y		= Half(-1.0f);
			};

			GET_CONST_ACCESSOR(CircleIntensity,	float,	_paramCBData.intensity);
			SET_ACCESSOR_DIRTY(CircleIntensity,	Half,	_paramCBData.intensity);
			GET_CONST_ACCESSOR(CircleSize,		float,	_paramCBData.size);
			SET_ACCESSOR_DIRTY(CircleSize,		Half,	_paramCBData.size);
			GET_CONST_ACCESSOR(RenderAble,		bool,	_renderAble);

			inline void SetDirectionalLightID(Core::ObjectID id) { _dlObjID = id; _changedDL = true; }

		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Camera::MainCamera& mainCam);
			void UpdateParamCB(Device::DirectX& dx, const Core::ObjectManager& objMgr, const Manager::LightManager& lightMgr, const Core::TransformPool& tfPool, const Camera::MainCamera& mainCam);
			void Render(Device::DirectX& dx, Texture::RenderTexture& resultMap, const Texture::Texture2D& inputColorMap, const Renderer::MainRenderer& renderer, const Camera::MainCamera& mainCamera, const Manager::LightManager& lightMgr);
			void Destroy();

		private:
			FullScreen									_occlusionMapScreen;
			FullScreen									_shaftScreen;
			Texture::RenderTexture						_occlusionMap;

		private:
			ParamCBData									_paramCBData;
			Buffer::ExplicitConstBuffer<ParamCBData>	_paramCB;
			Core::ObjectID								_dlObjID;

		private:
			bool										_renderAble	= false;
			bool										_dirty		= true;
			bool										_changedDL	= true;
		};
	}
}