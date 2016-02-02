#include "Mesh.h"
#include "Director.h"
//#include "TransformPipelineParam.h"

using namespace Rendering::Manager;
using namespace Rendering::Geometry;
using namespace Rendering::Buffer;

Mesh::Mesh() : 
	_filter(nullptr), _renderer(nullptr), 
	_selectMaterialIndex(0), _prevRenderType(MeshRenderer::Type::Unknown), _show(true), _prevShow(false)
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
	if(_transformConstBuffer->Initialize(sizeof(Rendering::TransformCB)) == false)
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
	if(_transformConstBuffer->Initialize(sizeof(Rendering::TransformCB)) == false)
		ASSERT_MSG("Error, transformBuffer->Initialize");

	ClassifyRenderMeshType();
}

void Mesh::OnInitialize()
{
}

void Mesh::OnUpdate(float deltaTime)
{
}

void Mesh::OnUpdateTransformCB(const Device::DirectX*& dx, const Rendering::TransformCB& transformCB)
{
	ID3D11DeviceContext* context = dx->GetContext();
	_transformConstBuffer->UpdateSubResource(context, &transformCB);
}

void Mesh::OnDestroy()
{
	SAFE_DELETE(_filter);
	SAFE_DELETE(_renderer);
	SAFE_DELETE(_transformConstBuffer);
}

void Mesh::ClassifyRenderMeshType()
{
	Manager::RenderManager* renderMgr = Device::Director::SharedInstance()->GetCurrentScene()->GetRenderManager();

	renderMgr->UpdateRenderList(this, _prevShow);
	_prevShow = _show;

	_prevRenderType = _renderer->GetCurrentRenderType();
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
		newMesh->_transformConstBuffer->Initialize(sizeof(Rendering::TransformCB));
		newMesh->ClassifyRenderMeshType();

		newMesh->_owner = _owner;
	}

	return newMesh;
}

void Mesh::SetShow(bool b)
{
	_show = b;
	ClassifyRenderMeshType();
}