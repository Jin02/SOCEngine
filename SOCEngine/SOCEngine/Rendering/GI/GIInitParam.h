#pragma once

#include "Common.h"

struct GIInitParam
{
	struct VXGIParam
	{
		uint		voxelizeDimension	= 0;
		float		voxelizeSize		= 0.0f;

		VXGIParam() = default;
		VXGIParam(uint _dimension, float _voxelizeSize) : voxelizeDimension(_dimension), voxelizeSize(_voxelizeSize) { }
	};

	inline bool GetUseVXGI() const
	{
		return (vxgiParam.voxelizeDimension != 0) & (vxgiParam.voxelizeSize > 0.0f);
	}

	inline bool GetUseGI() const
	{
		return GetUseVXGI();
	}

	VXGIParam	vxgiParam;
};