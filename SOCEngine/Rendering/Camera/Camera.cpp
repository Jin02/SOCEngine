#include "Camera.h"
#include "Scene.h"

using namespace Math;
using namespace std;
using namespace Rendering::Light;
using namespace Device;
using namespace Rendering::Buffer;

namespace Rendering
{
	Camera::Camera() : Component()
	{

	}

	Camera::~Camera(void)
	{
	
	}

	void Camera::Initialize()
	{
		FOV = 60;
		clippingNear = 0.01f;
		clippingFar = 1000.0f;
		normalizedViewPortRect = Rect<float>(0, 0, 1, 1);

		//이게 정상인가? 그런갑지;
		Size<int> windowSize = DeviceDirector::GetInstance()->GetApplication()->GetSize();
		aspect = (float)windowSize.w / (float)windowSize.h;

		camType    = Type::Perspective;
		clearColor = Color(0.5f, 0.5f, 1.0f,1.0f);

		frustum = new Frustum(0.0f);		
		this->skybox = nullptr;

		clearFlag = ClearFlag::FlagSolidColor;

		Graphics *graphics = DeviceDirector::GetInstance()->GetGraphics();
		//rtShader = new Shader::Shader(graphics, "Camera"); //render target shader?

		Scene *scene = dynamic_cast<Scene*>(Device::DeviceDirector::GetInstance()->GetScene());
		scene->GetShaderManager()->LoadShaderFromFile("Camera", &rtShader, false);

		renderTarget = new Texture::RenderTarget;
		renderTarget->Create(windowSize.w, windowSize.h, rtShader);
	}

	void Camera::Destroy()
	{
		Utility::SAFE_DELETE(rtShader);
		Utility::SAFE_DELETE(renderTarget);
		Utility::SAFE_DELETE(frustum);
	}

	void Camera::Clear(Graphics *gp)
	{
//		static Graphics *gp = DeviceDirector::GetInstance()->GetGraphics();

		//defulat is 'target clear option'.
//		Graphics flag = Graphics::FlagTarget;
		Graphics::ClearFlag flag = Graphics::ClearFlagType::FlagTarget;

		if( clearFlag == ClearFlag::FlagDontClear )
			return;

		if( clearFlag == ClearFlag::FlagSkybox )
		{
			//skybox Rendering
			skybox->Render();
			return;
		}

		else if( clearFlag == ClearFlag::FlagSolidColor )
			flag |= Graphics::ClearFlagType::FlagZBuffer;

		//Graphics *gpte = DeviceDirector::GetInstance()->GetGraphics();
		gp->Clear( 0, NULL, flag, clearColor, 1.0f, 0);
	}

	void Camera::CalcAspect()
	{
		float w = normalizedViewPortRect.size.w - normalizedViewPortRect.x;
		float h = normalizedViewPortRect.size.h - normalizedViewPortRect.y;

		Size<int> windowSize =  Device::DeviceDirector::GetInstance()->GetSize();
		
		aspect = ((float)windowSize.w * w) / ((float)windowSize.h * h);
	}

	void Camera::GetPerspectiveMatrix(Math::Matrix *outMatrix, float farGap)
	{
		float radian = FOV * SOCM_PI / 180.0f;
		SOCMatrixPerspectiveFovLH(outMatrix, radian, aspect, clippingNear, clippingFar + farGap);
	}
	void Camera::GetOrthoGraphicMatrix(Math::Matrix *outMatrix)
	{
		Size<int> windowSize = Device::DeviceDirector::GetInstance()->GetSize();;
		SOCMatrixOrthoLH(outMatrix, 
			(float)(windowSize.w * normalizedViewPortRect.size.w),
			(float)(windowSize.h * normalizedViewPortRect.size.h),
			clippingNear, clippingFar);
	}
	void Camera::GetProjectionMatrix(Math::Matrix *outMatrix, float farGap /* =0 */)
	{
		if(camType == Type::Perspective)
		{
			GetPerspectiveMatrix(outMatrix, farGap);
			return;
		}

		GetOrthoGraphicMatrix(outMatrix);
	}

	void Camera::SetViewPort(Common::Rect<float> rect)
	{
		normalizedViewPortRect = rect;
		CalcAspect();
	}

	void Camera::RenderObjects(std::vector<Object*>::iterator &objectBegin,	std::vector<Object*>::iterator &objectEnd, Light::LightManager* sceneLights)
	{
		Math::Matrix projMat, viewMat, viewProjMat;
		GetProjectionMatrix(&projMat);
		GetViewMatrix(&viewMat);

		SOCMatrixMultiply(&viewProjMat, &viewMat, &projMat);

		//Clear();
		frustum->Make(&viewProjMat);

		//추후 작업.

		vector<LightForm*> lights;
		sceneLights->Intersect(frustum, &lights);
		//월드 상의 빛에서 절두체에 겹치는거 모두 찾음.

		for(vector<Object*>::iterator iter = objectBegin; iter != objectEnd; ++iter)
		{
			(*iter)->Culling(frustum);
			(*iter)->Render(&lights, &viewMat, &projMat, &viewProjMat);
		}
	}

	void Camera::Render(std::vector<Object*>::iterator &objectBegin, std::vector<Object*>::iterator &objectEnd,	Light::LightManager* sceneLights)
	{
		static Graphics *gp = DeviceDirector::GetInstance()->GetGraphics();

		bool test = false;
		Surface hwBackBuffer(gp);
		test = gp->GetRenderTarget(0, &hwBackBuffer);

		Surface sceneSurface(gp);
		if(renderTarget->GetSurfaceLevel(0, &sceneSurface))
		{
			gp->SetRenderTarget(0, &sceneSurface);
			sceneSurface.ReleaseSurface();
		}

		test = gp->Clear( 0, NULL, Graphics::ClearFlagType::FlagTarget, clearColor, 1.0f, 0);
		RenderObjects(objectBegin, objectEnd, sceneLights);

		gp->SetRenderTarget(0, &hwBackBuffer);
		hwBackBuffer.ReleaseSurface();

		renderTarget->Render();
	}

	void Camera::SceneRender(Camera *cam, std::vector<Object*>::iterator& objectBegin,
			std::vector<Object*>::iterator& objectEnd, Light::LightManager* sceneLights)
	{
		static Graphics *gp = DeviceDirector::GetInstance()->GetGraphics();

		Math::Matrix projMat, viewMat, viewProjMat;
		cam->GetProjectionMatrix(&projMat);
		cam->GetViewMatrix(&viewMat);

		SOCMatrixMultiply(&viewProjMat, &viewMat, &projMat);

		cam->Clear(gp);
		cam->frustum->Make(&viewProjMat);

		//추후 작업.

		vector<LightForm*> lights;
		sceneLights->Intersect(cam->frustum, &lights);
		//월드 상의 빛에서 절두체에 겹치는거 모두 찾음.

		for(vector<Object*>::iterator iter = objectBegin; iter != objectEnd; ++iter)
		{
			(*iter)->Culling(cam->frustum);
			(*iter)->Render(&lights, &viewMat, &projMat, &viewProjMat);
		}
	}

	void Camera::GetViewMatrix(Math::Matrix *outMatrix)
	{
		ownerTransform->GetWorldMatrix(outMatrix);

		Vector3 worldPos = ownerTransform->GetWorldPosition();

		Vector3 p = Vector3(
			-SOCVec3Dot(&ownerTransform->GetRight(), &worldPos),
			-SOCVec3Dot(&ownerTransform->GetUp(), &worldPos),
			-SOCVec3Dot(&ownerTransform->GetForward(), &worldPos));

		outMatrix->_41 = p.x;
		outMatrix->_42 = p.y;
		outMatrix->_43 = p.z;
		outMatrix->_44 = 1.0f;
	}

	void Camera::GetViewProjectionMatrix(Math::Matrix *outMatrix, float farGap)
	{
		Math::Matrix proj;

		ownerTransform->GetWorldMatrix(outMatrix); // view Matrix
		GetProjectionMatrix(&proj, farGap);

		SOCMatrixMultiply(outMatrix, outMatrix, &proj);
	}

	//Component::Type Camera::GetComponentType()
	//{
	//	return Component::Type::Camera;
	//}
}