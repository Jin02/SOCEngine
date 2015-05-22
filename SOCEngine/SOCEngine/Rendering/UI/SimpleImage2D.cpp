#include "SimpleImage2D.h"
#include "Director.h"
#include "EngineShaderFactory.hpp"

using namespace UI;
using namespace Rendering;
using namespace Device;

SimpleImage2D::SimpleImage2D(const std::string& name, const Core::Object* parent) 
	: UIObject(parent), _mesh(nullptr), _material(nullptr), _isOtherMaterial(false)
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

	Mesh::Mesh::CreateFuncArguments meshCreateArgs("UI", "SimpleImage2D");
	{
		meshCreateArgs.vertex.data		= rectVertex;
		meshCreateArgs.vertex.count		= 4;
		meshCreateArgs.vertex.byteWidth	= sizeof(RectVertexInfo) - 4;

		meshCreateArgs.index.data		= indices;
		meshCreateArgs.index.count		= ARRAYSIZE(indices);
		meshCreateArgs.index.byteWidth	= 0; //not use

		meshCreateArgs.material			= _material;
		meshCreateArgs.isDynamic		= false;
		meshCreateArgs.bufferFlag		= (uint)Mesh::MeshFilter::BufferElement::UV;
	}

	_mesh = new Mesh::Mesh;
	_mesh->Create(meshCreateArgs);

	Manager::UIManager* uiMgr = Director::GetInstance()->GetCurrentScene()->GetUIManager();
	uiMgr->Add(_name, this);
}
