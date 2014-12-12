#pragma once

#include "EngineMath.h"

namespace Core
{
	struct TransformPipelineParam
	{
		Math::Matrix worldMat;
		Math::Matrix viewMat;
		Math::Matrix projMat;
		Math::Matrix worldViewProjMat;
	};
}