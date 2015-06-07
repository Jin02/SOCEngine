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