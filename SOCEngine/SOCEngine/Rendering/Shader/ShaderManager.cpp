#include "ShaderManager.h"
#include "Utility.hpp"

using namespace Rendering::Manager;
using namespace Rendering::Shader;

void ShaderManager::Destroy()
{
	GetPool<VertexShader>().Destroy();
	GetPool<PixelShader>().Destroy();
	GetPool<GeometryShader>().Destroy();
	GetPool<ComputeShader>().Destroy();
}