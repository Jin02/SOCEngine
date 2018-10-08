#pragma once

#include <vector>
#include <string>
#include "Vector3.h"
#include "Quaternion.h"

namespace Rendering
{
	namespace Character
	{
		struct AnimationNode
		{
			struct Keyframe
			{
				unsigned int			keytime = 0;
				Math::Vector3			translation = Math::Vector3(0.0f, 0.0f, 0.0f);
				Math::Vector3			scale = Math::Vector3(1.0f, 1.0f, 1.0f);
				Math::Quaternion		rotation = Math::Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
			};

			struct BoneKeyframe
			{
				std::string				boneId = "";
				std::vector<Keyframe>	keyframes;
			};

			std::string					id = "";
			std::vector<BoneKeyframe>	boneAnimations;
		};
	}
}