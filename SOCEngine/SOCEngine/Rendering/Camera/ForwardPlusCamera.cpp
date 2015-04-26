#include "ForwardPlusCamera.h"
#include "Director.h"

using namespace Rendering::Camera;
using namespace Rendering::Texture;
using namespace Device;
using namespace Math;

ForwardPlusCamera::ForwardPlusCamera()
	: _opaqueDepthBuffer(nullptr), _transparentDepthBuffer(nullptr), Camera()
{
	_renderType = RenderType::ForwardPlus;
}

ForwardPlusCamera::~ForwardPlusCamera()
{
}

void ForwardPlusCamera::Initialize()
{
	Camera::Initialize();
	const Size<unsigned int> windowSize = Director::GetInstance()->GetWindowSize();

	_opaqueDepthBuffer =  new DepthBuffer;
	_opaqueDepthBuffer->Create(windowSize);

	_transparentDepthBuffer =  new DepthBuffer;
	_transparentDepthBuffer->Create(windowSize);
}

void ForwardPlusCamera::Destroy()
{
	SAFE_DELETE(_opaqueDepthBuffer);
	SAFE_DELETE(_transparentDepthBuffer);

	Camera::Destroy();
}