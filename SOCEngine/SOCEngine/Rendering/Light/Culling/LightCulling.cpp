#include "LightCulling.h"
#include "Director.h"

#include "ResourceManager.h"

#include "Utility.h"

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
	PointLightRadiusWithCenter		= 0,
	SpotLightRadiusWithCenter		= 1
};

enum InputTexture : unsigned int
{
	InvetedOpaqueDepthBuffer		= 2,
	InvetedBlendedDepthBuffer		= 3
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

void LightCulling::Initialize(const std::string& filePath, 
						const Texture::DepthBuffer* invertedOpaqueDepthBuffer, 
						const Texture::DepthBuffer* invertedBlendedDepthBuffer)
{
	//혹시 모르니, 한번 초기화
	Destroy();

	ResourceManager* resourceManager = ResourceManager::GetInstance();
	auto shaderMgr = resourceManager->GetShaderManager();

	std::string fileName, fileExtension, folderDir;
	bool valid = Utility::String::ParseDirectory(filePath, folderDir, fileName, fileExtension);
	ASSERT_COND_MSG(valid, "Where is your file extension?");

	std::string macroCode = "#define BLEND_ENABLE\n";
	bool enableMSAA = Device::Director::GetInstance()->GetDirectX()->GetUseMSAA();
	if(enableMSAA)
		macroCode += "#define MSAA_ENABLE\n";

	ID3DBlob* blob = shaderMgr->CreateBlob(folderDir, fileName, 
		"cs", "LightCullingCS", false, &macroCode);

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
			// Opaque
			{
				ComputeShader::InputTexture inputTex;
				inputTex.idx		= InputTexture::InvetedOpaqueDepthBuffer;
				inputTex.texture	= invertedOpaqueDepthBuffer;
				_inputTextures.push_back(inputTex);
			}

			// Blended, Transparent
			{
				ComputeShader::InputTexture inputTex;
				inputTex.idx		= InputTexture::InvetedBlendedDepthBuffer;
				inputTex.texture	= invertedBlendedDepthBuffer;
				_inputTextures.push_back(inputTex);
			}
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

void LightCulling::UpdateInputBuffer(const Device::DirectX* dx, const CullingConstBuffer& cbData, const std::array<Math::Vector4, POINT_LIGHT_LIMIT_NUM>& pointLightCenterWithRadius, const std::array<Math::Vector4, SPOT_LIGHT_LIMIT_NUM>& spotLightCenterWithRadius)
{
	ID3D11DeviceContext* context = dx->GetContext();

	_globalDataBuffer->Update(context, &cbData);

	ComputeShader::InputBuffer inputBuffer;

	// Input Buffer Setting
	{
		_globalDataBuffer->Update(context, &cbData);

		//0번이 pointLight 들고있음
		_inputBuffers[0].buffer->Update(context, pointLightCenterWithRadius.data());

		//1번이 spot light
		_inputBuffers[1].buffer->Update(context, spotLightCenterWithRadius.data());
	}
}

void LightCulling::Dispatch(const Device::DirectX* dx, const Texture::DepthBuffer* invertedDepthBuffer)
{
	ID3D11DeviceContext* context = dx->GetContext();
	
	// 0번이 depth buffer 넣음
	_inputTextures[0].texture = invertedDepthBuffer;

	_computeShader->Dispatch(context);
	
	//혹시 모르니 초기화
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