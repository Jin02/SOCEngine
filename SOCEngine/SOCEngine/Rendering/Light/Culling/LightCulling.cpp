#include "LightCulling.h"
#include "Director.h"

using namespace Device;
using namespace Core;
using namespace Rendering;
using namespace Rendering::Light;
using namespace Rendering::Buffer;
using namespace GPGPU::DirectCompute;

#define TILE_SIZE 16

LightCulling::LightCulling() : _computeShader(nullptr), _globalDataBuffer(nullptr)
{

}

LightCulling::~LightCulling()
{
	SAFE_DELETE(_computeShader);
	SAFE_DELETE(_globalDataBuffer);
}

void LightCulling::Init(const std::string& folderPath, const std::string& fileName)
{
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
			inputBufferElement.idx = 0;
			inputBufferElement.buffer = pointLightCenterWithRadius;
			_inputBuffers.push_back(inputBufferElement);
		}

		// Spot Light
		{
			CSInputBuffer* spotLightCenterWithRadius = new CSInputBuffer;
			assert(spotLightCenterWithRadius->Create(sizeof(Math::Vector4), POINT_LIGHT_LIMIT_NUM));
			inputBufferElement.idx = 1;
			inputBufferElement.buffer = spotLightCenterWithRadius;
			_inputBuffers.push_back(inputBufferElement);
		}

		// depth buffer
		{			
			inputBufferElement.idx = 2;
			inputBufferElement.buffer = nullptr;
		}

		_computeShader->SetInputBuffers(_inputBuffers);
	}

	// Ouput Buffer Setting
	{
		CSOutputBuffer* lightIndexBuffer = new CSOutputBuffer;
		assert(lightIndexBuffer->Create(0, 0));
		ComputeShader::OutputBuffer outputBuffer;
		{
			outputBuffer.idx = 0;
			outputBuffer.buffer = lightIndexBuffer;
		}
		_outputBuffers.push_back(outputBuffer);
		_computeShader->SetOutputBuffers(_outputBuffers);
	}
}

void LightCulling::UpdateBuffer(const CullingConstBuffer& cbData, const std::array<Math::Vector4, POINT_LIGHT_LIMIT_NUM>& pointLightCenterWithRadius, const std::array<Math::Vector4, SPOT_LIGHT_LIMIT_NUM>& spotLightCenterWithRadius, const Texture::RenderTexture* linearDepth)
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

	// Depth Buffer Setting
	{
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