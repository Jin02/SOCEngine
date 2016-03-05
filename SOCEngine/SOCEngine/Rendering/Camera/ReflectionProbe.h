#pragma once

#include "Component.h"
#include "TextureCube.h"
#include "ConstBuffer.h"
#include "BoundBox.h"
#include "CameraForm.h"
#include "DepthBufferCube.h"

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
			struct RPInfo
			{
				Math::Matrix	viewProjs[6];
				uint			packedNumOfLights;
				Math::Vector3	camWorldPos;
			};
			enum Type { Baked, RealTime };


		private:
			Texture::TextureCube*		_cubeMap;
			Texture::DepthBufferCube*	_opaqueDepthBuffer;

			Buffer::ConstBuffer*		_rpInfoCB;
			
			Math::Matrix				_prevFrontZViewProjMat;
			uint						_prevPackedNumOfLights;

		private:
			float						_projNear;
			float						_range;
			Math::Vector3				_worldPos;

			bool						_useTransparent;
			CameraForm::RenderQueue		_transparentMeshQueue;

			uint						_skipFrame;
			Type						_type;

		public:
			ReflectionProbe();
			virtual ~ReflectionProbe();

		public:
			virtual void OnInitialize();
			virtual void OnDestroy();

			virtual void OnUpdateTransformCB(const Device::DirectX*& dx, const Rendering::TransformCB& transformCB){}

		public:
			void UpdateReflectionProbeCB(const Device::DirectX*& dx, uint packedNumOfLights);
			void Render(const Device::DirectX*& dx, const Core::Scene* scene);

		public:
			GET_SET_ACCESSOR(ProjNear,			float,							_projNear);
			GET_SET_ACCESSOR(Range,				float,							_range);
			GET_SET_ACCESSOR(UseTransparent,	bool,							_useTransparent);
			GET_SET_ACCESSOR(SkipFrame,			uint,							_skipFrame);		// not working
			GET_ACCESSOR(Type,					Type,							_type);				// not working

			GET_ACCESSOR(CubeMap,				const Texture::TextureCube*,	_cubeMap);
		};
	}
}