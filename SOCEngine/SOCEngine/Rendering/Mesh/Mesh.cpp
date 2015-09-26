#include "Mesh.h"
#include "Director.h"
//#include "TransformPipelineParam.h"

using namespace Rendering::Manager;
using namespace Rendering::Mesh;
using namespace Rendering::Buffer;

Mesh::Mesh() : 
	_filter(nullptr), _renderer(nullptr), 
	_selectMaterialIndex(0), _transformConstBuffer(nullptr)
{
	_updateType = MaterialUpdateType::All;
}

Mesh::~Mesh()
{
	OnDestroy();
}

void Mesh::Initialize(const CreateFuncArguments& args)
{
	_filter = new MeshFilter;
	if(_filter->Initialize(args) == false)
		ASSERT_MSG("Error, filter->cratebuffer");

	_renderer = new MeshRenderer;
	if(_renderer->AddMaterial(args.material) == false)
		ASSERT_MSG("Error, renderer addmaterial");

	_transformConstBuffer = new Buffer::ConstBuffer;
	if(_transformConstBuffer->Initialize(sizeof(Core::TransformPipelineShaderInput)) == false)
		ASSERT_MSG("Error, transformBuffer->Initialize");

	ClassifyRenderMeshType();
}

void Mesh::Initialize(Rendering::Buffer::VertexBuffer*& vertexBuffer, 
					  Rendering::Buffer::IndexBuffer*& indexBuffer,
					  Rendering::Material*& initMaterial)
{
	_filter = new MeshFilter;
	if(_filter->Initialize(vertexBuffer, indexBuffer) == false)
		ASSERT_MSG("Error, filter->cratebuffer");

	_renderer = new MeshRenderer;
	if(_renderer->AddMaterial(initMaterial) == false)
		ASSERT_MSG("Error, cant add material in MeshRenderer");

	_transformConstBuffer = new Buffer::ConstBuffer;
	if(_transformConstBuffer->Initialize(sizeof(Core::TransformPipelineShaderInput)) == false)
		ASSERT_MSG("Error, transformBuffer->Initialize");

	ClassifyRenderMeshType();
}

void Mesh::OnInitialize()
{
}

void Mesh::OnUpdate(float deltaTime)
{
}

void Mesh::OnUpdateTransformCB(const Core::TransformPipelineShaderInput& transpose_Transform)
{
	if( memcmp(&transpose_Transform, &_prevTransformData, sizeof(Core::TransformPipelineShaderInput)) )
	{
		ID3D11DeviceContext* context = Device::Director::GetInstance()->GetDirectX()->GetContext();
		_transformConstBuffer->UpdateSubResource(context, &transpose_Transform);

		memcpy(&_prevTransformData, &transpose_Transform, sizeof(Core::TransformPipelineShaderInput));
	}
}

void Mesh::OnDestroy()
{
	SAFE_DELETE(_filter);
	SAFE_DELETE(_renderer);
	SAFE_DELETE(_transformConstBuffer);
}

void Mesh::ClassifyRenderMeshType()
{
	Manager::RenderManager* renderMgr = Device::Director::GetInstance()->GetCurrentScene()->GetRenderManager();
	if(_renderer)
	{
		bool isTransparentMesh = _renderer->IsTransparent();
		RenderManager::MeshType type = isTransparentMesh ? 
			RenderManager::MeshType::Transparent : RenderManager::MeshType::Opaque;

		renderMgr->UpdateRenderList(this, type);
	}
}

void Mesh::OnRenderPreview()
{
	ClassifyRenderMeshType();
}

Core::Component* Mesh::Clone() const
{
	Mesh* newMesh = new Mesh;
	{
		newMesh->_filter = new MeshFilter(*_filter);
		newMesh->_renderer = new MeshRenderer(*_renderer);
		newMesh->_transformConstBuffer = new Buffer::ConstBuffer;
		newMesh->_transformConstBuffer->Initialize(sizeof(Core::TransformPipelineShaderInput));
		newMesh->ClassifyRenderMeshType();

		newMesh->_owner = _owner;
	}

	return newMesh;
}
