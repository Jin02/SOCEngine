#include "SimpleImage2D.h"
#include "Director.h"
#include "EngineShaderFactory.hpp"

#include "ResourceManager.h"

using namespace UI;
using namespace Rendering;
using namespace Device;
using namespace Resource;

SimpleImage2D::SimpleImage2D(const std::string& name, Core::Object* parent) 
	: UIObject(name, parent), _meshFilter(nullptr), _material(nullptr), _isOtherMaterial(false), _changeSize(false)
{
}

SimpleImage2D::~SimpleImage2D()
{
	if(_isOtherMaterial == false)
		SAFE_DELETE(_material);

	SAFE_DELETE(_meshFilter);
}

void SimpleImage2D::Initialize(const Math::Size<uint>& size, const std::string& sharedVerticesKey, Rendering::Material* material)
{
	if(material == nullptr)
	{
		_material = new Material(_name, Material::Type::UI);
		_isOtherMaterial = true;
		
		const ResourceManager* resourceManager = ResourceManager::GetInstance();
		auto shaderMgr = resourceManager->GetShaderManager();
		Factory::EngineFactory factory(shaderMgr);

		Shader::VertexShader*	vs = nullptr;
		Shader::PixelShader*	ps = nullptr;
		factory.LoadShader("SimpleUIImage2D", "VS", "PS", nullptr, &vs, &ps);

		Material::CustomShader customShader;
		{
			customShader.isDeferred = false;
			customShader.shaderGroup = Shader::ShaderGroup(vs, ps, nullptr, nullptr);
		}
		_material->SetCustomShader(customShader);
	}

	RectVertexInfo rectVertex[4];
	memset(rectVertex, 0, sizeof(RectVertexInfo) * 4);

	std::vector<uint> indices(6);
	{
		indices[0] = 0; indices[1] = 1; indices[2] = 2;
		indices[3] = 1; indices[4] = 3; indices[3] = 2;
	}

	Geometry::MeshFilter::CreateFuncArguments meshCreateArgs("UI:SimpleImage2D", sharedVerticesKey);
	{
		meshCreateArgs.vertices.data		= rectVertex;
		meshCreateArgs.vertices.count		= 4;
		meshCreateArgs.vertices.byteWidth	= sizeof(RectVertexInfo);

		meshCreateArgs.indices				= &indices;
		meshCreateArgs.useDynamicVB			= true;
	}

	_meshFilter = new Geometry::MeshFilter;
	bool success = _meshFilter->Initialize(meshCreateArgs);
	ASSERT_COND_MSG(success, "Error, cant create SimpleImage2D meshfilter");

	Manager::UIManager* uiMgr = Director::GetInstance()->GetCurrentScene()->GetUIManager();
	uiMgr->AddRenderQueue(_name, this);

	if(_root == this)
		uiMgr->AddUpdateQueue(this);

	UIObject::InitConstBuffer();
	SetSize(size);
}

void SimpleImage2D::Render(const Device::DirectX* dx, const Math::Matrix& viewProjMat)
{
	ID3D11DeviceContext* context = dx->GetContext();

	if(_changeSize)
	{
		const Math::Size<uint>& screenSize = Director::GetInstance()->GetBackBufferSize();

		float left		= -(_size.w / 2.0f);
		float right		= -left;
		float bottom	= -(_size.h / 2.0f);
		float top		= -bottom;

		RectVertexInfo vertices[4];
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
		};

		_meshFilter->GetVertexBuffer()->UpdateVertexData(dx->GetContext(), vertices, sizeof(RectVertexInfo) * 4);
		_changeSize = false;
	}

	Shader::VertexShader* vs = _material->GetCustomShader().shaderGroup.vs;
	Shader::PixelShader* ps = _material->GetCustomShader().shaderGroup.ps;

	if(_material->GetCustomShader().shaderGroup.ableRender())
	{		
		_meshFilter->GetVertexBuffer()->IASetBuffer(context);
		_meshFilter->GetIndexBuffer()->IASetBuffer(context);

		vs->SetShaderToContext(context);
		vs->SetInputLayoutToContext(context);

		ps->SetShaderToContext(context);

		std::vector<Shader::ShaderForm::InputConstBuffer> constBuffers;
		{
			auto buffer = _transformCB;
			constBuffers.push_back(Shader::ShaderForm::InputConstBuffer(0, buffer, true, false, false, false));
		}
		std::vector<Shader::ShaderForm::InputTexture> textures;
		{
			Texture::Texture2D* tex = nullptr;
			_material->GetVariable<Texture::Texture2D*>(tex, "main");
			textures.push_back(Shader::ShaderForm::InputTexture(0, tex, false, false, false, true));
		}

		vs->UpdateResources(context, &constBuffers, nullptr, nullptr);
		ps->UpdateResources(context, nullptr, &textures, nullptr);
		context->DrawIndexed(_meshFilter->GetIndexCount(), 0, 0);
	}
}

void SimpleImage2D::UpdateMainImage(Texture::Texture2D* tex)
{
	_material->SetVariable<Texture::Texture2D*>("main", tex);
}

void SimpleImage2D::SetSize(const Math::Size<uint>& size)
{
	_size = size;
	_changeSize = true;
}