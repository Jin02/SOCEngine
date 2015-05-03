#include "MeshRenderer.h"
#include "Director.h"
#include "Mesh.h"

using namespace Rendering;
using namespace Rendering::Mesh;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::Manager;

MeshRenderer::MeshRenderer()
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
			RenderManager::MeshType type = hasAlpha ? RenderManager::MeshType::transparent : RenderManager::MeshType::opaque;

			if(renderMgr->Find(material, parent, type == RenderManager::MeshType::transparent ? RenderManager::MeshType::opaque : RenderManager::MeshType::transparent))
				renderMgr->Change(material, parent, type);
			else if(renderMgr->Find(material, parent, type) == nullptr)
				renderMgr->Add(material, parent, type);

			material->SetChangedAlpha(false);
		}
	};

	_materials.IterateContent(IterMaterials);
}