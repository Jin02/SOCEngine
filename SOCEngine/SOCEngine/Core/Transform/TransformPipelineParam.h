#pragma once

#include "EngineMath.h"

namespace Core
{
	struct TransformPipelineShaderInput
	{
		Math::Matrix worldMat;
		Math::Matrix worldViewMat;
		Math::Matrix worldViewProjMat;
	};
}