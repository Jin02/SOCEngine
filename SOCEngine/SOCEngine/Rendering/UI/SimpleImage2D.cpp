#include "SimpleImage2D.h"
#include "Director.h"
#include "EngineShaderFactory.hpp"

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
}

void SimpleImage2D::Create(const Math::Size<uint>& size, Rendering::Material* material)
{
	if(material == nullptr)
	{
		_material = new Material(_name, Material::Type::UI);
		_isOtherMaterial = true;

		auto shaderMgr = Director::GetInstance()->GetCurrentScene()->GetShaderManager();
		Factory::EngineFactory factory(shaderMgr);

		Shader::VertexShader*	vs = nullptr;
		Shader::PixelShader*	ps = nullptr;
		factory.LoadShader("SimpleUIImage2D", "VS", "PS", nullptr, &vs, &ps);

		_material->SetShaderTargets( Shader::RenderShaders(vs, ps) );
	}

	RectVertexInfo rectVertex[4];
	memset(rectVertex, 0, sizeof(RectVertexInfo) * 4);

	uint indices[] =
	{
		0, 2, 1,
		1, 2, 3
	};

	Mesh::MeshFilter::CreateFuncArguments meshCreateArgs("UI", "SimpleImage2D");
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

void SimpleImage2D::Render(const Math::Matrix& viewProjMat)
{
	const Device::Director* director	= Device::Director::GetInstance();
	const Device::DirectX* dx			= director->GetDirectX();
	ID3D11DeviceContext* context		= dx->GetContext();

	if(_changeSize)
	{
		const Math::Size<uint>& screenSize = Director::GetInstance()->GetWindowSize();

		float left		= -(screenSize.w / 2.0f);
		float right		= left + _size.w;
		float top		= -(screenSize.h / 2.0f);
		float bottom	= top + _size.h;
		//float left		= -(_size.w / 2.0f);
		//float right		= -left;
		//float top		= -(_size.h / 2.0f);
		//float bottom	= -top;

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

		_meshFilter->UpdateVertexBufferData(context, vertices, sizeof(RectVertexInfo) * 4);
		_changeSize = false;
	}

	Shader::VertexShader* vs = _material->GetShaderTargets().vs;
	Shader::PixelShader* ps = _material->GetShaderTargets().ps;

	if(_material->GetShaderTargets().ableRender())
	{		
		_meshFilter->IASetBuffer(context);

		vs->UpdateShaderToContext(context);
		vs->UpdateInputLayoutToContext(context);

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