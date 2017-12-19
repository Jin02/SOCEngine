#include "PreIntegrateEnvBRDF.h"
#include "ShaderManager.h"
#include "ComputeShader.h"
#include "DirectX.h"
#include "ShaderFactory.hpp"

using namespace Math;
using namespace Device;
using namespace Core;
using namespace Rendering;
using namespace Rendering::Precompute;
using namespace Rendering::Shader;
using namespace Rendering::Manager;
using namespace Rendering::Texture;
using namespace Rendering::Manager;
using namespace Rendering::Factory;

Texture::Texture2D& PreIntegrateEnvBRDF::CreatePreBRDFMap(Device::DirectX& dx, ShaderManager& shaderMgr)
{
	std::shared_ptr<ComputeShader> shader = ShaderFactory::LoadComputeShader(dx, shaderMgr, "PreIntegrateEnvBRDF", "CS", nullptr, nullptr);

	const Size<uint> size(256, 256);

	_texture.Initialize(dx, size.w, size.h,
						DXGI_FORMAT_R16G16_FLOAT, DXGI_FORMAT_R16G16_FLOAT,
						D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, 1, 1);

	shader->Dispatch(dx, ComputeShader::ThreadGroup((size.w + PRE_INTEGRATE_TILE_RES - 1) / PRE_INTEGRATE_TILE_RES,
													(size.h + PRE_INTEGRATE_TILE_RES - 1) / PRE_INTEGRATE_TILE_RES, 1));
	shaderMgr.GetPool<ComputeShader>().Delete(shader->GetKey());

	return _texture;
}