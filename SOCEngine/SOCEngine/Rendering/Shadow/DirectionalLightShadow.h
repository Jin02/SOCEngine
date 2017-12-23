#pragma once

#include "BaseShadow.h"
#include "BaseLight.h"
#include "Transform.h"
#include "DirectionalLight.h"
#include "ShadowMapCBPool.h"

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
			class DirectionalLightShadowBuffer;
		}

		class DirectionalLightShadow final
		{
		public:
			using ViewProjMatType	= Math::Matrix;
			using ShadowBufferType	= Buffer::DirectionalLightShadowBuffer;
			using LightType			= Light::DirectionalLight;
			using ManagerType		= Rendering::Manager::ShadowManager;
			using ShadowMapRenderer	= Renderer::ShadowMap::ShadowMapRenderer;
			using CBPoolType		= Buffer::ShadowMapCBPool;

			struct Param
			{
				Math::Vector3 forward	= Math::Vector3(0.0f, 0.0f, 0.0f);
				float zPos				= 0.0f;
			};

		public:
			DirectionalLightShadow(const BaseShadow& base) : _base(base) {}
			ViewProjMatType MakeVPMatParam(	const Light::LightPool<Light::DirectionalLight>& lightPool,
											const Core::TransformPool& tfPool, const Intersection::BoundBox& sceneBoundBox	);

			GET_CONST_ACCESSOR(ViewProjectionMatrix,		const auto&,	_transposedViewProjMat);
			GET_CONST_ACCESSOR(Param,						const auto&,	_param);
			GET_CONST_ACCESSOR(ParamCBData,					auto,			_param);

			GET_CONST_ACCESSOR(ProjectionSize,				float,			_projectionSize);
			GET_CONST_ACCESSOR(UseAutoProjectionLocation,	bool,			_useAutoProjectLocation);

			GET_CONST_ACCESSOR(ObjectID,					Core::ObjectID,	_base.GetObjectID());
			GET_CONST_ACCESSOR(Dirty,						bool,			_base.GetDirty());

			SET_ACCESSOR(ProjectionSize,					float,			_projectionSize);
			SET_ACCESSOR(UseAutoProjectionLocation,			bool,			_useAutoProjectLocation);

			GET_ALL_ACCESSOR_PTR(Base,						BaseShadow,		_base);

			inline void SetDirty(bool b) { _base.SetDirty(b); }

		public:
			constexpr static float FrustumMinZ = 1.0f;
			constexpr static float FrustumMaxZ = 10000.0f;

		private:
			BaseShadow		_base;
			Math::Matrix	_transposedViewProjMat;
			Param			_param;

			float			_projectionSize			= 0.0f;
			bool			_useAutoProjectLocation	= true;
		};
	}
}