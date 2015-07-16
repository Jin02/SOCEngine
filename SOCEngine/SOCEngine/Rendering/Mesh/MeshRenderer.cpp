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

bool MeshRenderer::HasMaterial(Material* material)
{
	for(auto iter = _materials.begin(); iter != _materials.end(); ++iter)
	{
		if((*iter) == material)
			return true;
	}

	return false;
}

bool MeshRenderer::AddMaterial(Material* material)
{
	if( HasMaterial(material) )
		return false;

	_materials.push_back(material);
	return true;
}

bool MeshRenderer::IsTransparent()
{
	for(auto iter = _materials.begin(); iter != _materials.end(); ++iter)
	{
		if( (*iter)->GetHasAlpha() == false )
			return false;
	}

	return true;
}