#include "MeshRenderer.h"
#include "Director.h"
#include "Mesh.h"

using namespace Rendering;
using namespace Rendering::Geometry;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::Manager;

MeshRenderer::MeshRenderer() : _useAlphaBlending(false)
{

}

MeshRenderer::~MeshRenderer()
{
}

bool MeshRenderer::HasMaterial(const Material* material) const
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

bool MeshRenderer::IsTransparent() const
{
	for(auto iter = _materials.begin(); iter != _materials.end(); ++iter)
	{
		if( (*iter)->GetHasAlpha() == false )
			return false;
	}

	return true;
}

MeshRenderer::Type MeshRenderer::GetCurrentRenderType() const
{
	bool isTransparnet = IsTransparent();

	if(isTransparnet)			return Type::Transparent;
	else if(_useAlphaBlending)	return Type::AlphaBlend;

	return Type::Opaque;
}