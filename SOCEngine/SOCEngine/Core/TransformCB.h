#pragma once

#include "Matrix.h"

struct TransformCB
{
	enum class ChangeState : unsigned int
	{
		No			= 0,
		HasChanged,
		HadChanged,
		MAX
	};

	Math::Matrix world;
	Math::Matrix worldInvTranspose;

	Math::Matrix prevWorld;
};