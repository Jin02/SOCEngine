#pragma once

#include "EngineMath.h"

namespace Core
{
	struct TransformPipelineParam
	{
		Math::Matrix worldMat;
		Math::Matrix viewMat;
		Math::Matrix projMat;
	};

	struct TransformPipelineShaderInput
	{
		Math::Matrix worldMat;
		Math::Matrix worldViewMat;
		Math::Matrix worldViewProjMat;
	};
}