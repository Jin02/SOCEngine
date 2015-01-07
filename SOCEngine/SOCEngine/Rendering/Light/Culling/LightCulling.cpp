#include "LightCulling.h"
#include "Director.h"

using namespace Device;
using namespace Core;
using namespace Rendering;
using namespace Rendering::Light;
using namespace Rendering::Buffer;
using namespace GPGPU::DirectCompute;

#define TILE_SIZE 16

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

LightCulling::LightCulling() : _computeShader(nullptr), _globalDataBuffer(nullptr)
{

}

LightCulling::~LightCulling()
{
	SAFE_DELETE(_computeShader);
	SAFE_DELETE(_globalDataBuffer);
}

void LightCulling::Init(const std::string& folderPath, const std::string& fileName, const Texture::RenderTexture* linearDepth)
{
	Director* director = Director::GetInstance();
	Scene* scene = Director::GetInstance()->GetCurrentScene();
	auto shaderMgr = scene->GetShaderManager();

	ID3DBlob* blob = shaderMgr->CreateBlob(folderPath, fileName, "cs", "LightCulling", false);

	ComputeShader::ThreadGroup threadGroup;
	UpdateThreadGroup(&threadGroup, false);
	_computeShader = new ComputeShader(threadGroup, blob);

	assert(_computeShader->Create());

	_globalDataBuffer = new ConstBuffer;
	assert(_globalDataBuffer->Create(sizeof(CullingConstBuffer)));

	ComputeShader::InputBuffer inputBufferElement;

	// Input Buffer Setting
	{
		// Point Light
		{
			CSInputBuffer* pointLightCenterWithRadius = new CSInputBuffer;
			assert(pointLightCenterWithRadius->Create(sizeof(Math::Vector4), POINT_LIGHT_LIMIT_NUM));
			inputBufferElement.idx = InputBuffer::PointLightRadiusWithCenter;
			inputBufferElement.buffer = pointLightCenterWithRadius;
			_inputBuffers.push_back(inputBufferElement);
		}

		// Spot Light
		{
			CSInputBuffer* spotLightCenterWithRadius = new CSInputBuffer;
			assert(spotLightCenterWithRadius->Create(sizeof(Math::Vector4), POINT_LIGHT_LIMIT_NUM));
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
		CSOutputBuffer_ReadCPU* lightIndexBuffer = new CSOutputBuffer_ReadCPU;
		Math::Size<unsigned int> size = director->GetWindowSize();

		assert(lightIndexBuffer->Create(0, 0));
		ComputeShader::OutputBuffer outputBuffer;
		{
			outputBuffer.idx = OutputBuffer::LightIndexBuffer;
			outputBuffer.buffer = lightIndexBuffer;
		}
		_outputBuffers.push_back(outputBuffer);
		_computeShader->SetOutputBuffers(_outputBuffers);
	}
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
		_inputBuffers[0].buffer->Update(context, pointLightCenterWithRadius.data());
		_inputBuffers[1].buffer->Update(context, spotLightCenterWithRadius.data());
	}
}

void LightCulling::Dispatch(ID3D11DeviceContext* context)
{
	_computeShader->Dispatch(context);
}

void LightCulling::UpdateThreadGroup(ComputeShader::ThreadGroup* outThreadGroup, bool updateComputeShader)
{
	auto CalcThreadLength = [](unsigned int size)
	{
		return (unsigned int)((size+TILE_SIZE-1) / (float)TILE_SIZE);
	};

	unsigned int width	= CalcThreadLength(Director::GetInstance()->GetWindowSize().w);
	unsigned int height = CalcThreadLength(Director::GetInstance()->GetWindowSize().h);

	ComputeShader::ThreadGroup threadGroup = ComputeShader::ThreadGroup(width, height, 1);

	if(outThreadGroup)
		(*outThreadGroup) = threadGroup;

	if(updateComputeShader)
		_computeShader->SetThreadGroupInfo(threadGroup);
}