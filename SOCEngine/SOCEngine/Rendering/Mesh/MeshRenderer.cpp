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

void MeshRenderer::Test(void* mesh)
{
	MeshManager* meshMgr = Device::Director::GetInstance()->GetCurrentScene()->GetMeshManager();
	Rendering::Mesh::Mesh* parent = static_cast<Rendering::Mesh::Mesh*>(mesh);

	auto IterMaterials = [&](Material* material)
	{
		bool changd = material->GetChangedAlpha();
		if(changd)
		{
			bool hasAlpha = material->GetHasAlpha();
			MeshManager::MeshType type = hasAlpha ? MeshManager::MeshType::hasAlpha : MeshManager::MeshType::nonAlpha;

			if(meshMgr->Find(material, parent, type == MeshManager::MeshType::hasAlpha ? MeshManager::MeshType::nonAlpha : MeshManager::MeshType::hasAlpha))
				meshMgr->Change(material, parent, type);
			else if(meshMgr->Find(material, parent, type) == nullptr)
				meshMgr->Add(material, parent, type);

			material->SetChangedAlpha(false);
		}
	};

	_materials.IterateContent(IterMaterials);
}