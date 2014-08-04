#include "MeshRenderer.h"

namespace Rendering
{
	using namespace Mesh;

	MeshRenderer::MeshRenderer()
	{
	}

	MeshRenderer::~MeshRenderer()
	{
	}

	bool MeshRenderer::AddMaterial(Material::Material* material, bool copy)
	{
		if(_materials.Find(material->GetName()) )
			return false;

		_materials.Add(material->GetName(), material, copy);
		return true;
	}

	void MeshRenderer::UpdateMaterial()
	{
		auto& v = _materials.GetVector();
		for(auto iter = v.begin(); iter != v.end(); ++iter)
		{
			Shader::PixelShader* ps = GET_CONTENT_FROM_ITERATOR(iter)->GetPixelShader();
			Shader::VertexShader* vs = GET_CONTENT_FROM_ITERATOR(iter)->GetVertexShader();
		}
	}
}