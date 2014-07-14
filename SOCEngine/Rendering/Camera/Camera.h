#pragma once

//#include "Transform.h"
#include "Graphics.h"
#include "Component.h"
#include "Frustum.h"
#include "LightManager.h"
#include "Component.h"
#include "Shader.h"
//#include "RenderTarget.h"

namespace Rendering
{
	class Camera : public Component
	{
	public:
		static const Component::Type ComponentType = Component::Type::Camera;

	public:
		enum Type { Perspective, Orthographic };
		enum ClearFlag { FlagSkybox, FlagSolidColor, FlagTarget, FlagDontClear };
		//CLEAR_FLAG_DEPTHONLY는 제외함. 어떻게 구현하라는건지 잘 모르겠음 -ㅠ-;

	private:
		Frustum					*frustum;
		Shader::Shader			*rtShader;
		Texture::RenderTarget	*renderTarget;

	private:
		Math::Rect<float>	  normalizedViewPortRect;

	public:  //굳이 private로 할 필요는 없지.
		float				FOV;
		float				clippingNear;
		float				clippingFar;
		ClearFlag			clearFlag;
		Type				camType;
		float				aspect;
		Color				clearColor;
		//Skybox				*skybox;


	public:
		Camera();
		~Camera(void);

	private:
		void CalcAspect();
		void Clear(Device::Graphics *gp);
		void RenderObjects(std::vector<Object*>::iterator &objectBegin,	std::vector<Object*>::iterator &objectEnd,	Light::LightManager* sceneLights);

	public:
		void GetPerspectiveMatrix(Math::Matrix *outMatrix, float farGap);
		void GetOrthoGraphicMatrix(Math::Matrix *outMatrix);
		void GetProjectionMatrix(Math::Matrix *outMatrix, float farGap = 0);
		void GetViewMatrix(Math::Matrix *outMatrix);
		void GetViewProjectionMatrix(Math::Matrix *outMatrix, float farGap = 0);

	public:
		//static void SceneUpdate(float dt, std::vector<Object*> *sceneObjects);
		static void SceneRender(Camera *cam, 
			std::vector<Object*>::iterator &objectBegin,
			std::vector<Object*>::iterator &objectEnd,
			Light::LightManager* sceneLights);

		void Render(std::vector<Object*>::iterator &objectBegin,
			std::vector<Object*>::iterator &objectEnd,
			Light::LightManager* sceneLights);

	public:
		void SetViewPort(Math::Rect<float> rect);

	public:
		virtual void Initialize();
		virtual void Destroy();
//		virtual Component::Type GetComponentType();
	};
}