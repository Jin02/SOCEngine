#pragma once

#include "BaseShadow.h"
#include "BaseLight.h"
#include "Transform.h"
#include "SpotLight.h"
#include "SpotLightShadowMapCBPool.h"

namespace Rendering
{
	namespace Manager
	{
		class ShadowManager;
	}
	namespace Renderer
	{
		namespace ShadowMap
		{
			class ShadowMapRenderer;
		}
	}

	namespace Shadow
	{
		namespace Buffer
		{
			class SpotLightShadowBuffer;
		}

		class SpotLightShadow final
		{
		public:
			using ViewProjMatType	= Math::Matrix;
			using ShadowBufferType	= Buffer::SpotLightShadowBuffer;
			using LightType			= Light::SpotLight;
			using ManagerType		= Rendering::Manager::ShadowManager;
			using ShadowMapRenderer	= Renderer::ShadowMap::ShadowMapRenderer;
			using CBPoolType		= Buffer::SpotLightShadowMapCBPool;

		public:
			SpotLightShadow(const BaseShadow& base) : _base(base) {}
			ViewProjMatType MakeVPMatParam(const Light::LightPool<Light::SpotLight>& lightPool, const Core::TransformPool& tfPool);

			GET_CONST_ACCESSOR(TransposedVPMat,			const auto&,	_transposedViewProjMat);
			GET_ALL_ACCESSOR_PTR(Base,					BaseShadow,		_base);

			GET_CONST_ACCESSOR(ObjectID,				Core::ObjectID,	_base.GetObjectID());

			inline void SetDirty(bool b) { _base.SetDirty(b); }
			GET_CONST_ACCESSOR(Dirty, bool, _base.GetDirty());

		private:
			Math::Matrix	_transposedViewProjMat;
			BaseShadow		_base;
		};
	}
}