#include "MeshRenderer.h"
#include "Director.h"
#include "Mesh.h"

using namespace Rendering;
using namespace Rendering::Mesh;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::Manager;

MeshRenderer::MeshRenderer(Material* depthWriteMaterial, Material*	alphaTestMaterial) 
	: _depthWriteMaterial(depthWriteMaterial), _alphaTestMaterial(alphaTestMaterial)
{

}

MeshRenderer::~MeshRenderer()
{
}

bool MeshRenderer::AddMaterial(Material* material, bool copy)
{
	if(_materials.Find(material->GetName()) )
		return false;

	_materials.Add(material->GetName(), material, copy);
	return true;
}

void MeshRenderer::UpdateAllMaterial(ID3D11DeviceContext* context, const Buffer::ConstBuffer* transformBuffer, const Buffer::ConstBuffer* camera)
{
	auto& materials = _materials.GetVector();
	for(auto iter = materials.begin(); iter != materials.end(); ++iter)
	{
		Material* material = GET_CONTENT_FROM_ITERATOR(iter);
		material->UpdateBasicConstBuffer(context, transformBuffer, camera);
		material->UpdateResources(context);
	}
}

bool MeshRenderer::UpdateMaterial(ID3D11DeviceContext* context, unsigned int index, const Buffer::ConstBuffer* transformBuffer, const Buffer::ConstBuffer* camera)
{
	if( index >= _materials.GetSize() )
		return false;

	Material* material = GET_CONTENT_FROM_ITERATOR( (_materials.GetVector().begin() + index) );
	if(material == nullptr)
		return false;

	material->UpdateBasicConstBuffer(context, transformBuffer, camera);
	material->UpdateResources(context);
	return true;
}

void MeshRenderer::ClearResource(ID3D11DeviceContext* context)
{
	auto& materials = _materials.GetVector();
	for(auto iter = materials.begin(); iter != materials.end(); ++iter)
		GET_CONTENT_FROM_ITERATOR(iter)->ClearResource(context);
}

void MeshRenderer::ClassifyMaterialWithMesh(void* mesh)
{
	RenderManager* renderMgr = Device::Director::GetInstance()->GetCurrentScene()->GetRenderManager();
	Rendering::Mesh::Mesh* parent = static_cast<Rendering::Mesh::Mesh*>(mesh);

	auto IterMaterials = [&](Material* material)
	{
		bool changd = material->GetChangedAlpha();
		if(changd)
		{
			bool hasAlpha = material->GetHasAlpha();
			RenderManager::MeshType type = hasAlpha ? RenderManager::MeshType::hasAlpha : RenderManager::MeshType::nonAlpha;

			if(renderMgr->Find(material, parent, type == RenderManager::MeshType::hasAlpha ? RenderManager::MeshType::nonAlpha : RenderManager::MeshType::hasAlpha))
				renderMgr->Change(material, parent, type);
			else if(renderMgr->Find(material, parent, type) == nullptr)
				renderMgr->Add(material, parent, type);

			material->SetChangedAlpha(false);
		}
	};

	_materials.IterateContent(IterMaterials);
}