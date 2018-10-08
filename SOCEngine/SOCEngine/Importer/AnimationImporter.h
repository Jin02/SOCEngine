#pragma once

#include "Common.h"
#include "Half.h"
#include "G3djDataTypes.h"
#include "AnimationNode.h"

#include <unordered_map>

namespace Importer
{
	class AnimationImporter
	{
	public:
		static bool LoadAnimation(const std::string& path, std::vector<Rendering::Character::AnimationNode>& outNodes);
	};
}