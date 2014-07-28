#pragma once

#include "DirectX.h"
#include "Component.h"
#include "Frustum.h"
#include "LightManager.h"
#include "Component.h"
#include "Structure.h"

namespace Rendering
{
	class Camera : public Core::Component
	{
	public:
		static const Core::Component::Type GetComponentType() {	return Core::Component::Type::Camera;	}

	public:
		enum Type { Perspective, Orthographic };
		enum ClearFlag { FlagSkybox, FlagSolidColor, FlagTarget, FlagDontClear };
		//CLEAR_FLAG_DEPTHONLY는 제외함. 어떻게 구현하라는건지 잘 모르겠음 -ㅠ-;

	private:
		Frustum					*_frustum;
		//Shader::Shader			*rtShader;
		//Texture::RenderTarget	*renderTarget;

	public:  //굳이 private로 할 필요는 없지.
		float				_FOV;
		float				_clippingNear;
		float				_clippingFar;
		ClearFlag			_clearFlag;
		Type				_camType;
		float				_aspect;
		Color				_clearColor;

	public:
		Camera();
		~Camera(void);

	private:
		void CalcAspect();
		void Clear(Device::DirectX *dx);
		void RenderObjects(std::vector<Core::Object*>::iterator &objectBegin,	std::vector<Core::Object*>::iterator &objectEnd,	Light::LightManager* sceneLights);

	public:
		void ProjectionMatrix(Math::Matrix &outMatrix);
		void ViewMatrix(Math::Matrix& outMatrix);

	public:
		//static void SceneUpdate(float dt, std::vector<Object*> *sceneObjects);
		void Render( const Structure::Vector<Core::Object>& objects, Light::LightManager* sceneLights);

	public:
		virtual void Initialize();
		virtual void Destroy();
//		virtual Component::Type GetComponentType();
	};
}