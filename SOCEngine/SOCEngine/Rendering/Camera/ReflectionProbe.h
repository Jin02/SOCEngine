#pragma once

#include "Component.h"
#include "TextureCube.h"
#include "ConstBuffer.h"
#include "BoundBox.h"

namespace Core
{
	class Scene;
}

namespace Rendering
{
	namespace Camera
	{
		class ReflectionProbe : public Core::Component
		{
		public:
			static const Core::Component::Type GetComponentType() {	return Core::Component::Type::ReflectionProbe;	}

		private:
			Texture::TextureCube*	_cubeMap;

			Buffer::ConstBuffer*	_viewProjMatCB;
			Math::Matrix			_prevFrontZViewProjMat;

		private:
			float					_projNear;
			float					_range;
			Color					_clearColor;
			Intersection::BoundBox	_boundBox;

		public:
			ReflectionProbe();
			virtual ~ReflectionProbe();

		public:
			virtual void OnInitialize();
			virtual void OnDestroy();

			virtual void OnUpdateTransformCB(const Device::DirectX*& dx, const Rendering::TransformCB& transformCB);

		public:
			void Render(const Device::DirectX*& dx, const Core::Scene* scene);
		};
	}
}