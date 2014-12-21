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

void MeshRenderer::UpdateAllMaterial(ID3D11DeviceContext* context, Buffer::ConstBuffer* transformBuffer)
{
	Sampler* sampler = Device::Director::GetInstance()->GetCurrentScene()->GetSampler();
	std::vector<Shader::PixelShader::SamplerType> samplers;

	//임시로, 0번에 linear만 넣음.
	samplers.push_back(std::make_pair(0, sampler));

	auto& materials = _materials.GetVector();
	for(auto iter = materials.begin(); iter != materials.end(); ++iter)
	{
		Material* material = GET_CONTENT_FROM_ITERATOR(iter);
		material->UpdateTransformBuffer(context, transformBuffer);
		material->UpdateResources(context, samplers);
	}
}

bool MeshRenderer::UpdateMaterial(ID3D11DeviceContext* context, unsigned int index, Buffer::ConstBuffer* transformBuffer)
{
	if( index >= _materials.GetSize() )
		return false;

	Material* material = GET_CONTENT_FROM_ITERATOR( (_materials.GetVector().begin() + index) );
	if(material == nullptr)
		return false;

	Sampler* sampler = Device::Director::GetInstance()->GetCurrentScene()->GetSampler();
	std::vector<Shader::PixelShader::SamplerType> samplers;

	//임시로, 0번에 linear만 넣음.
	samplers.push_back(std::make_pair(0, sampler));
	material->UpdateTransformBuffer(context, transformBuffer);
	material->UpdateResources(context, samplers);
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