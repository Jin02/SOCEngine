#pragma once

#include "LightForm.h"
#include "Frustum.h"
#include "DirectionalLightShadow.h"

#define DIRECTIONAL_LIGHT_FRUSTUM_MIN_Z		1.0f
#define DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z		10000.0f

namespace Rendering
{
	namespace Light
	{
		class DirectionalLight : public LightForm
		{
		public:
			struct Param
			{
				float invProj_33;
				float invProj_44;

				Param() : invProj_33(0.0f), invProj_44(0.0f) {}
				~Param() {}
			};

		private:
			Param						_param;
			Math::Matrix				_viewProjMat;

			Intersection::Frustum		_frustum;
			
			float						_projectionSize;
			bool						_useAutoProjectLocation;

		public:
			DirectionalLight();
			virtual ~DirectionalLight();

		public:
			virtual void OnUpdate(float deltaTime);

		public:
			void ComputeViewProjMatrix(const Intersection::BoundBox& sceneBoundBox, const Math::Matrix& invViewportMat);
			virtual void CreateShadow();

		public:
			bool Intersect(const Intersection::Sphere &sphere) const;
			void MakeLightBufferElement(std::pair<ushort, ushort>& outDir, Param& outParam) const;

		public:
			virtual Core::Component* Clone() const;

		public:
			GET_ACCESSOR(Frustum,							const Intersection::Frustum&,		_frustum);
			GET_ACCESSOR(Shadow,							Shadow::DirectionalLightShadow*,	static_cast<Shadow::DirectionalLightShadow*>(_shadow));
			GET_ACCESSOR(ViewProjectionMatrix,				const Math::Matrix&,				_viewProjMat);

			GET_SET_ACCESSOR(ProjectionSize,				float,								_projectionSize);
			GET_SET_ACCESSOR(UseAutoProjectionLocation,		bool,								_useAutoProjectLocation);
			GET_SET_ACCESSOR(LightShaftSize,				float,								_radius);
		};
	}
}
