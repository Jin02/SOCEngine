#include "SimpleImage2D.h"
#include "Director.h"
#include "EngineShaderFactory.hpp"

using namespace UI;
using namespace Rendering;
using namespace Device;

SimpleImage2D::SimpleImage2D(const std::string& name, const Core::Object* parent) 
	: UIObject(parent), _meshFilter(nullptr), _material(nullptr), _isOtherMaterial(false)
{
	_name = name;
}

SimpleImage2D::~SimpleImage2D()
{
	if(_isOtherMaterial == false)
		SAFE_DELETE(_material);
}

void SimpleImage2D::Create(Rendering::Material* material)
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

	struct RectVertexInfo
	{
		Math::Vector4	position;
		Math::Vector2	uv;
	};

	RectVertexInfo rectVertex[4];
	{
		rectVertex[0].position.x	= -1.0f;
		rectVertex[0].position.y	= -1.0f;
		rectVertex[0].position.z	=  0.0f;
		rectVertex[0].uv.x			= -1.0f;
		rectVertex[0].uv.x			= -1.0f;

		rectVertex[1].position.x	=  1.0f;
		rectVertex[1].position.y	= -1.0f;
		rectVertex[1].position.z	=  0.0f;
		rectVertex[1].uv.x			=  1.0f;
		rectVertex[1].uv.x			= -1.0f;

		rectVertex[2].position.x	= -1.0f;
		rectVertex[2].position.y	=  1.0f;
		rectVertex[2].position.z	=  0.0f;
		rectVertex[2].uv.x			= -1.0f;
		rectVertex[2].uv.x			=  1.0f;

		rectVertex[3].position.x	=  1.0f;
		rectVertex[3].position.y	=  1.0f;
		rectVertex[3].position.z	=  0.0f;
		rectVertex[3].uv.x			=  1.0f;
		rectVertex[3].uv.x			=  1.0f;
	};

	uint indices[] =
	{
		0, 1, 2,
		1, 3, 2
	};

	Mesh::MeshFilter::CreateFuncArguments meshCreateArgs("UI", "SimpleImage2D");
	{
		meshCreateArgs.vertex.data		= rectVertex;
		meshCreateArgs.vertex.count		= 4;
		meshCreateArgs.vertex.byteWidth	= sizeof(RectVertexInfo) - 4;

		meshCreateArgs.index.data		= indices;
		meshCreateArgs.index.count		= ARRAYSIZE(indices);
		meshCreateArgs.index.byteWidth	= 0; //not use

		meshCreateArgs.isDynamic		= false;
		meshCreateArgs.bufferFlag		= (uint)Mesh::MeshFilter::BufferElement::UV;
	}

	_meshFilter = new Mesh::MeshFilter;
	bool success = _meshFilter->CreateBuffer(meshCreateArgs);
	ASSERT_COND_MSG(success, "Error, cant create SimpleImage2D meshfilter");

	Manager::UIManager* uiMgr = Director::GetInstance()->GetCurrentScene()->GetUIManager();
	uiMgr->AddRenderQueue(_name, this);

	if(_root == this)
		uiMgr->AddUpdateQueue(this);
}

void SimpleImage2D::Render(const Math::Matrix& viewProjMat)
{
	const Device::Director* director	= Device::Director::GetInstance();
	const Device::DirectX* dx			= director->GetDirectX();
	//Core::Scene* scene					= director->GetCurrentScene();
	//UI::Manager::UIManager* uiMgr		= scene->GetUIManager();

	Shader::VertexShader* vs = _material->GetShaderTargets().vs;
	Shader::PixelShader* ps = _material->GetShaderTargets().ps;

	if(_material->GetShaderTargets().ableRender())
	{
		ID3D11DeviceContext* context	= dx->GetContext();
		UpdateTransform(context, viewProjMat);

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

		ps->UpdateResources(context, &constBuffers, &textures);
		context->DrawIndexed(_meshFilter->GetIndexCount(), 0, 0);
	}
}

void SimpleImage2D::UpdateMainImage(Texture::Texture* tex)
{
	_material->SetVariable<Texture::Texture*>("main", tex);
}