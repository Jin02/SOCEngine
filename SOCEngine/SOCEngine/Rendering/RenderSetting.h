#pragma once

#include <string>
#include "Rect.h"
#include "Common.h"

struct RenderSetting
{
	std::string	mainCamName			= "";
	Rect<uint>	renderRect			= Rect<uint>(0, 0, 0, 0);
	uint		shadowMapResolution	= 1024;
	uint		voxelizeDimension	= 256;
	float		voxelizeSize		= 10.0f;

	RenderSetting() = default;
	RenderSetting(const std::string& _mainCamName, const Rect<uint>& _renderRect, uint _shadowMapResolution, uint _voxelizeDimension, float _voxelizeSize)
		: mainCamName(_mainCamName), renderRect(_renderRect), shadowMapResolution(_shadowMapResolution), voxelizeDimension(_voxelizeDimension), voxelizeSize(_voxelizeSize)
	{
	}
};