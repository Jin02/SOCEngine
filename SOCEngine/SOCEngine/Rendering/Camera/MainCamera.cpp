#include "MainCamera.h"
#include "EngineShaderFactory.hpp"
#include "Utility.hpp"

using namespace Utility;
using namespace Device;
using namespace Core;
using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::Camera;
using namespace Device;
using namespace Rendering::Manager;

void MainCamera::Initialize(DirectX& dx, ShaderManager& shaderMgr)
{
	std::vector<Shader::ShaderMacro> macros;
	{
		ShaderMacro msaaMacro = dx.GetMSAAShaderMacro();
		macros.push_back(msaaMacro);

		macros.push_back(ShaderMacro("USE_COMPUTE_SHADER", ""));
		macros.push_back(ShaderMacro("ENABLE_BLEND", ""));
	}

	// Load Shader
	{
		Factory::EngineShaderFactory pathFind(nullptr);

		std::string fullPath;
		pathFind.FetchShaderFullPath(fullPath, "TBDR");

		std::string folderPath, fileName, format;
		String::ParseDirectory(fullPath, folderPath, fileName, format);

		DXResource<ID3DBlob> blob = shaderMgr.CreateBlob(
			folderPath, fileName + "." + format,
			"cs", "TileBasedDeferredShadingCS",
			false, &macros);
		Shader::ComputeShader shader(blob, "@TBDR");
		shader.Initialize(dx);

		Size<uint> size = ComputeThreadGroupSize(dx.GetBackBufferSize());
		ComputeShader::ThreadGroup threadGroup(size.w, size.h, 1);
		shader.SetThreadGroupInfo(threadGroup);

		_tbrShader = shader;
	}

	Size<float> fltSize = Size<float>(	static_cast<float>(dx.GetBackBufferSize().w),
										static_cast<float>(dx.GetBackBufferSize().h)	);
	Rect<float> renderRect = Rect<float>(0.0f, 0.0f, fltSize.w, fltSize.h);
	_meshCam.Initialize(dx, renderRect, true);
}

const Size<uint> MainCamera::ComputeThreadGroupSize(const Size<uint>& size) const
{
	return Size<uint>(
		static_cast<uint>((size.w + LIGHT_CULLING_TILE_RES - 1) / static_cast<float>(LIGHT_CULLING_TILE_RES)),
		static_cast<uint>((size.h + LIGHT_CULLING_TILE_RES - 1) / static_cast<float>(LIGHT_CULLING_TILE_RES))
		);
}

uint Rendering::Camera::MainCamera::CalcMaxNumLightsInTile(const Size<uint>& size) const
{
	const uint key = LIGHT_CULLING_TILE_RES;
	return (LIGHT_CULLING_LIGHT_MAX_COUNT_IN_TILE - (key * (size.h / 120)));
}

void MainCamera::Render(Device::DirectX& dx)
{
	//ID3D11DeviceContext* context = dx->GetContext();

	//std::vector<ShaderForm::InputConstBuffer> inputConstBuffers;

	//ASSERT_MSG_IF(tbrCB && mainCamCB, "Error, tbrCB and mainCB is null");
	//{
	//	inputConstBuffers.push_back(ShaderForm::InputConstBuffer((uint)ConstBufferBindIndex::TBRParam,	tbrCB));
	//	inputConstBuffers.push_back(ShaderForm::InputConstBuffer((uint)ConstBufferBindIndex::Camera,	mainCamCB));
	//}

	//if(additionalConstBuffers)
	//	inputConstBuffers.insert(inputConstBuffers.end(), additionalConstBuffers->begin(), additionalConstBuffers->end());

	//_computeShader->SetInputConstBuffers(inputConstBuffers);
	//_computeShader->Dispatch(context);
}