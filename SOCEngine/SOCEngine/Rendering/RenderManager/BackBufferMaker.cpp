#include "BackBufferMaker.h"
#include "Director.h"

#include "EngineShaderFactory.hpp"
#include "ResourceManager.h"

using namespace Rendering::PostProcessing;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::Texture;
using namespace Rendering::Mesh;
using namespace Device;
using namespace Math;
using namespace Core;
using namespace Resource;

BackBufferMaker::BackBufferMaker()
	:	_vertexShader(nullptr), _pixelShader(nullptr), _depthBuffer(nullptr),
		_meshFilter(nullptr)
{
}

BackBufferMaker::~BackBufferMaker()
{
	Destroy();
}

void BackBufferMaker::Initialize()
{
	ResourceManager* resourceManager = ResourceManager::GetInstance();
	Manager::ShaderManager* shaderMgr = resourceManager->GetShaderManager();

	Factory::EngineFactory shaderLoader(shaderMgr);
	shaderLoader.LoadShader("BackBufferMaker", "VS", "PS", nullptr, &_vertexShader, &_pixelShader);
	ASSERT_COND_MSG(_vertexShader && _pixelShader, "Error, Cant load BackBufferMaker shader files");

	const Size<uint>& winSize = GlobalDeviceDirectorGetScreenSize;

	ConstBuffer* transformConstBuffer = new ConstBuffer;
	transformConstBuffer->Initialize(sizeof(Matrix));
	{
		Matrix world, view, proj;

		Matrix::Identity(world);
		world._43 = -10;

		Camera::CameraForm::ViewMatrix(view, world);
		Matrix::OrthoLH(proj, (float)winSize.w, (float)winSize.h, 0.01f, 1000.0f);

		Matrix viewProj = view * proj;
		Matrix::Transpose(viewProj, viewProj);
		
		ID3D11DeviceContext* context = Device::Director::GetInstance()->GetDirectX()->GetContext();
		transformConstBuffer->Update(context, &viewProj); 

		BaseShader::BufferType bufferType;
		bufferType.first	= 0;
		bufferType.second	= transformConstBuffer;
		_constBuffers.push_back(bufferType);
	}
	{
		for(int i=0; i<2; ++i)
		{
			BaseShader::TextureType type;
			type.first = i;
			type.second = nullptr;

			_textures.push_back(type);
		}
	}

	_depthBuffer = new DepthBuffer;	
	_depthBuffer->Initialize(winSize);

	//Initialize Mesh
	{
		Mesh::MeshFilter::CreateFuncArguments meshCreateArgs("BackBufferMaker", "BackBufferMaker");

		struct Vertex
		{
			Math::Vector3	position;
			Math::Vector2	uv;
		};
		
		float left		= -(winSize.w / 2.0f);
		float right		= -left;
		float bottom	= -(winSize.h / 2.0f);
		float top		= -bottom;

		Vertex vertices[4];
		memset(vertices, 0, sizeof(Vertex) * 4);
		{
			vertices[0].position.x	= left;
			vertices[0].position.y	= top;
			vertices[0].position.z	= 0.0f;

			vertices[0].uv.x		= 0.0f;
			vertices[0].uv.y		= 0.0f;

			vertices[1].position.x	= right;
			vertices[1].position.y	= top;
			vertices[1].position.z	= 0.0f;

			vertices[1].uv.x		= 1.0f;
			vertices[1].uv.y		= 0.0f;

			vertices[2].position.x	= left;
			vertices[2].position.y	= bottom;
			vertices[2].position.z	= 0.0f;

			vertices[2].uv.x		= 0.0f;
			vertices[2].uv.y		= 1.0f;

			vertices[3].position.x	= right;
			vertices[3].position.y	= bottom;
			vertices[3].position.z	= 0.0f;

			vertices[3].uv.x		=  1.0f;
			vertices[3].uv.y		=  1.0f;
		}

		uint indices[] =
		{
			0, 1, 2,
			1, 3, 2
		};

		meshCreateArgs.vertex.data		= vertices;
		meshCreateArgs.vertex.count		= 4;
		meshCreateArgs.vertex.byteWidth	= sizeof(Vertex);

		meshCreateArgs.index.data		= indices;
		meshCreateArgs.index.count		= ARRAYSIZE(indices);
		meshCreateArgs.index.byteWidth	= 0; //not use

		meshCreateArgs.isDynamic		= false;

		_meshFilter = new MeshFilter;
		_meshFilter->CreateBuffer(meshCreateArgs);
	}
}

void BackBufferMaker::Destroy()
{
	for(auto iter : _constBuffers)
		SAFE_DELETE(iter.second);

	SAFE_DELETE(_depthBuffer);
	SAFE_DELETE(_meshFilter);
}

void BackBufferMaker::Render(const Device::DirectX* dx, const Rendering::Camera::CameraForm* mainCamera, const Rendering::Camera::UICamera* uiCamera)
{
	ID3D11DeviceContext* context		= dx->GetContext();

	const RenderTexture* mainCamRT	= mainCamera ? mainCamera->GetRenderTarget() : nullptr;
	const RenderTexture* uiCamRT	= uiCamera ? uiCamera->GetRenderTarget() : nullptr;

	ID3D11RenderTargetView* deviceBackBuffer = dx->GetBackBuffer();
	context->OMSetRenderTargets(1, &deviceBackBuffer, _depthBuffer->GetDepthStencilView());
	_depthBuffer->Clear(context, 1.0f, 0);

	float color[] = {0, 0, 0, 1};
	context->ClearRenderTargetView(deviceBackBuffer, color);

	ID3D11SamplerState* sampler = dx->GetPointSamplerState();
	context->PSSetSamplers(0, 1, &sampler);
	{
		_meshFilter->IASetBuffer(dx);

		_vertexShader->UpdateShader(context);
		_vertexShader->UpdateInputLayout(context);
		_vertexShader->UpdateResources(context, &_constBuffers, nullptr);

		_pixelShader->UpdateShader(context);
		_textures[0].second = mainCamRT;
		_textures[1].second = uiCamRT;
		_pixelShader->UpdateResources(context, nullptr, &_textures);

		context->DrawIndexed(_meshFilter->GetIndexCount(), 0, 0);
	}
	ID3D11SamplerState* nullSampler = nullptr;
	context->PSSetSamplers(0, 1, &nullSampler);
}