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

bool MeshRenderer::IsTransparent()
{
	const auto& vectors = _materials.GetVector();
	for(auto iter = vectors.begin(); iter != vectors.end(); ++iter)
	{
		if( GET_CONTENT_FROM_ITERATOR(iter)->GetHasAlpha() == false )
			return false;
	}

	return true;
}