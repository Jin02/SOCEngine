#include "SimpleImage2D.h"
#include "Director.h"
#include "EngineShaderFactory.hpp"

#include "ResourceManager.h"

using namespace UI;
using namespace Rendering;
using namespace Device;

SimpleImage2D::SimpleImage2D(const std::string& name, const Core::Object* parent) 
	: UIObject(parent), _meshFilter(nullptr), _material(nullptr), _isOtherMaterial(false), _changeSize(false)
{
	_name = name;
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
		factory.LoadShader("SimpleUIImage2D", "VS", "PS", nullptr, nullptr, &vs, &ps);

		_material->SetCustomRenderSceneShader( Shader::ShaderGroup(vs, ps, nullptr, nullptr) );
	}

	RectVertexInfo rectVertex[4];
	memset(rectVertex, 0, sizeof(RectVertexInfo) * 4);

	uint indices[] =
	{
		0, 1, 2,
		1, 3, 2
	};

	Mesh::MeshFilter::CreateFuncArguments meshCreateArgs("UI:SimpleImage2D", sharedVerticesKey);
	{
		meshCreateArgs.vertex.data		= rectVertex;
		meshCreateArgs.vertex.count		= 4;
		meshCreateArgs.vertex.byteWidth	= sizeof(RectVertexInfo);

		meshCreateArgs.index.data		= indices;
		meshCreateArgs.index.count		= ARRAYSIZE(indices);
		meshCreateArgs.index.byteWidth	= 0; //not use

		meshCreateArgs.isDynamic		= true;
		meshCreateArgs.bufferFlag		= (uint)Mesh::MeshFilter::BufferElement::UV;
	}

	_meshFilter = new Mesh::MeshFilter;
	bool success = _meshFilter->CreateBuffer(meshCreateArgs);
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
		const Math::Size<uint>& screenSize = Director::GetInstance()->GetWindowSize();

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

		_meshFilter->UpdateVertexBufferData(dx, vertices, sizeof(RectVertexInfo) * 4);
		_changeSize = false;
	}

	Shader::VertexShader* vs = _material->GetCustomRenderSceneShader().vs;
	Shader::PixelShader* ps = _material->GetCustomRenderSceneShader().ps;

	if(_material->GetCustomRenderSceneShader().ableRender())
	{		
		_meshFilter->IASetBuffer(dx);

		vs->UpdateShader(context);
		vs->UpdateInputLayout(context);

		ps->UpdateShader(context);

		std::vector<Shader::BaseShader::BufferType> constBuffers;
		{
			Shader::BaseShader::BufferType type;
			type.first = 0;
			type.second = dynamic_cast<Buffer::BaseBuffer*>(_transformCB);

			constBuffers.push_back(type);
		}
		std::vector<Shader::BaseShader::TextureType> textures;
		{
			Shader::BaseShader::TextureType type;
			type.first = 0;

			Texture::Texture* tex = nullptr;
			_material->GetVariable<Texture::Texture*>(tex, "main");
			Math::Size<uint> size = tex->FetchSize();

			type.second = tex;

			textures.push_back(type);
		}

		vs->UpdateResources(context, &constBuffers, nullptr);
		ps->UpdateResources(context, nullptr, &textures);
		context->DrawIndexed(_meshFilter->GetIndexCount(), 0, 0);
	}
}

void SimpleImage2D::UpdateMainImage(Texture::Texture* tex)
{
	_material->SetVariable<Texture::Texture*>("main", tex);
}

void SimpleImage2D::SetSize(const Math::Size<uint>& size)
{
	_size = size;
	_changeSize = true;
}