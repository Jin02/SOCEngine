#include "LightCulling.h"
#include "Director.h"

using namespace Device;
using namespace Core;
using namespace Rendering;
using namespace Rendering::Light;
using namespace Rendering::Buffer;
using namespace GPGPU::DirectCompute;

#define TILE_SIZE				16
#define LIGHT_MAX_NUM_IN_TILE	544


enum InputBuffer : unsigned int
{
	PointLightRadiusWithCenter	= 0,
	SpotLightRadiusWithCenter	= 1
};

enum InputTexture : unsigned int
{
	LinearDepthBuffer = 2
};

enum OutputBuffer : unsigned int
{
	LightIndexBuffer = 0
};

LightCulling::LightCulling() : _computeShader(nullptr), _globalDataBuffer(nullptr), _lightIndexBuffer(nullptr)
{

}

LightCulling::~LightCulling()
{
	Destroy();
}

void LightCulling::Init(const std::string& folderPath, const std::string& fileName, const Texture::RenderTexture* linearDepth)
{
	//Ȥ�� �𸣴�, �ѹ� �ʱ�ȭ
	Destroy();

	Director* director = Director::GetInstance();
	Scene* scene = Director::GetInstance()->GetCurrentScene();
	auto shaderMgr = scene->GetShaderManager();

	ID3DBlob* blob = shaderMgr->CreateBlob(folderPath, fileName, "cs", "LightCullingCS", false);

	ComputeShader::ThreadGroup threadGroup;
	UpdateThreadGroup(&threadGroup, false);
	_computeShader = new ComputeShader(threadGroup, blob);

	ASSERT_COND_MSG(_computeShader->Initialize(), "can not create compute shader");

	_globalDataBuffer = new ConstBuffer;
	ASSERT_COND_MSG(_globalDataBuffer->Initialize(sizeof(CullingConstBuffer)), "can not create const buffer");

	ComputeShader::InputBuffer inputBufferElement;

	// Input Buffer Setting
	{
		// Point Light
		{
			CSInputBuffer* pointLightCenterWithRadius = new CSInputBuffer;
			ASSERT_COND_MSG(pointLightCenterWithRadius->Initialize(sizeof(Math::Vector4), POINT_LIGHT_LIMIT_NUM), "cant create cs input buffer");
			inputBufferElement.idx = InputBuffer::PointLightRadiusWithCenter;
			inputBufferElement.buffer = pointLightCenterWithRadius;
			_inputBuffers.push_back(inputBufferElement);
		}

		// Spot Light
		{
			CSInputBuffer* spotLightCenterWithRadius = new CSInputBuffer;
			ASSERT_COND_MSG(spotLightCenterWithRadius->Initialize(sizeof(Math::Vector4), POINT_LIGHT_LIMIT_NUM), "cant create cs input buffer");
			inputBufferElement.idx = InputBuffer::SpotLightRadiusWithCenter;
			inputBufferElement.buffer = spotLightCenterWithRadius;
			_inputBuffers.push_back(inputBufferElement);
		}

		// depth buffer
		{
			ComputeShader::InputTexture inputTex;
			inputTex.idx		= InputTexture::LinearDepthBuffer;
			inputTex.texture	= linearDepth;
			_inputTextures.push_back(inputTex);
		}

		_computeShader->SetInputBuffers(_inputBuffers);
		_computeShader->SetInputTextures(_inputTextures);
	}

	// Ouput Buffer Setting
	{
		LightCulling_CSOutputBuffer* lightIndexBuffer = new LightCulling_CSOutputBuffer;
		Math::Size<unsigned int> size = CalcThreadSize();
		lightIndexBuffer->Initialize(size, CalcMaxNumLightsInTile());

		ComputeShader::OutputBuffer outputBuffer;
		{
			outputBuffer.idx = OutputBuffer::LightIndexBuffer;
			outputBuffer.buffer = lightIndexBuffer;
		}
		_outputBuffers.push_back(outputBuffer);
		_computeShader->SetOutputBuffers(_outputBuffers);
	}
}

unsigned int LightCulling::CalcMaxNumLightsInTile()
{
	const Math::Size<unsigned int>& winSize = Director::GetInstance()->GetWindowSize();
	const unsigned key = 16;

	return ( LIGHT_MAX_NUM_IN_TILE - ( key * ( winSize.h / 120 ) ) );
}

void LightCulling::UpdateInputBuffer(const CullingConstBuffer& cbData, const std::array<Math::Vector4, POINT_LIGHT_LIMIT_NUM>& pointLightCenterWithRadius, const std::array<Math::Vector4, SPOT_LIGHT_LIMIT_NUM>& spotLightCenterWithRadius)
{
	const Director* director = Device::Director::GetInstance();
	ID3D11DeviceContext* context = director->GetDirectX()->GetContext();

	_globalDataBuffer->Update(context, &cbData);

	ComputeShader::InputBuffer inputBuffer;

	// Input Buffer Setting
	{
		_globalDataBuffer->Update(context, &cbData);

		//0���� pointLight �������
		_inputBuffers[0].buffer->Update(context, pointLightCenterWithRadius.data());

		//1���� spot light
		_inputBuffers[1].buffer->Update(context, spotLightCenterWithRadius.data());
	}
}

void LightCulling::Dispatch(ID3D11DeviceContext* context, const Texture::RenderTexture* linearDepth)
{
	// 0���� depth buffer ����
	_inputTextures[0].texture = linearDepth;

	_computeShader->Dispatch(context);
	
	//Ȥ�� �𸣴� �ʱ�ȭ
	_inputTextures[0].texture = nullptr;
}

void LightCulling::UpdateThreadGroup(ComputeShader::ThreadGroup* outThreadGroup, bool updateComputeShader)
{
	Math::Size<unsigned int> threadSize = CalcThreadSize();
	ComputeShader::ThreadGroup threadGroup = ComputeShader::ThreadGroup(threadSize.w, threadSize.h, 1);

	if(outThreadGroup)
		(*outThreadGroup) = threadGroup;

	if(updateComputeShader)
		_computeShader->SetThreadGroupInfo(threadGroup);
}

const Math::Size<unsigned int> LightCulling::CalcThreadSize()
{
	auto CalcThreadLength = [](unsigned int size)
	{
		return (unsigned int)((size+TILE_SIZE-1) / (float)TILE_SIZE);
	};

	const Math::Size<unsigned int>& winSize = Director::GetInstance()->GetWindowSize();

	unsigned int width	= CalcThreadLength(winSize.w);
	unsigned int height = CalcThreadLength(winSize.h);

	return Math::Size<unsigned int>(width, height);
}

void LightCulling::Destroy()
{
	SAFE_DELETE(_computeShader);
	SAFE_DELETE(_globalDataBuffer);
}