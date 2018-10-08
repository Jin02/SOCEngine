#include "AnimationImporter.h"
#include <cassert>
#include "Utility.hpp"
#include <fstream>
#include <rapidjson/document.h>

using namespace Importer;
using namespace rapidjson;
using namespace Rendering::Character;

bool AnimationImporter::LoadAnimation(const std::string& path, std::vector<AnimationNode>& outNodes)
{
	std::string fileName, fileFormat, folderDir;
	assert(Utility::String::ParseDirectory(path, folderDir, fileName, fileFormat));

	std::ifstream	g3dFile;
	std::string		g3dFileFormat;
	const std::string supportedFileFormat[] = { "g3dj", /*"g3db"*/ };
	for (uint i = 0; i<ARRAYSIZE(supportedFileFormat); ++i)
	{
		g3dFileFormat = supportedFileFormat[i];

		std::string g3dFilePath = folderDir + fileName + "_" + fileFormat + "." + supportedFileFormat[i];
		g3dFile.open(g3dFilePath.c_str(), std::ios::in | std::ios::ate | std::ios::binary);

		if (g3dFile.is_open() && g3dFile.good())
			break;
	}

	// "Error, Invalid Mesh File"
	bool open = g3dFile.is_open();
	bool good = g3dFile.good();
	assert(open & good);

	std::streamoff length = g3dFile.tellg();
	g3dFile.seekg(0, g3dFile.beg);

	char* buffer = new char[(uint)length + 1];
	{
		g3dFile.read(buffer, length);
		buffer[length] = '\0';
	}
	g3dFile.close();

	// Parsing
	{
		AnimationNode animData;

		Document document;
		document.Parse(buffer);

		assert(document.HasParseError() == false);
		assert(document.HasMember("animations"));

		const auto& animations	= document["animations"];
		uint size				= animations.Size();
		for (uint i = 0; i < size; ++i)
		{
			animData.id = animations[i]["id"].GetString();

			const auto& bones = animations[i]["bones"];
			uint size = bones.Size();
			for (uint i = 0; i < size; ++i)
			{
				const auto& boneNode = bones[i];

				AnimationNode::BoneKeyframe boneKeyframe;
				boneKeyframe.boneId = boneNode["boneId"].GetString();

				AnimationNode::Keyframe prevFrame;

				const auto& keyframesData = boneNode["keyframes"];
				uint keyframeCount = keyframesData.Size();
				for (uint i = 0; i < keyframeCount; ++i)
				{
					AnimationNode::Keyframe newKeyframe;

					const auto& keyframe = keyframesData[i];
					newKeyframe.keytime = static_cast<uint>(keyframe["keytime"].GetDouble() * 1000.0);

					if (keyframe.HasMember("rotation"))
					{
						const auto& node = keyframe["rotation"];
						newKeyframe.rotation.x = node[0u].GetDouble();
						newKeyframe.rotation.y = node[1u].GetDouble();
						newKeyframe.rotation.z = node[2u].GetDouble();
						newKeyframe.rotation.w = node[3u].GetDouble();

						prevFrame.rotation = newKeyframe.rotation;
					}
					else
					{
						newKeyframe.rotation = prevFrame.rotation;
					}

					if (keyframe.HasMember("scale"))
					{
						const auto& node = keyframe["scale"];
						newKeyframe.scale.x = node[0u].GetDouble();
						newKeyframe.scale.y = node[1u].GetDouble();
						newKeyframe.scale.z = node[2u].GetDouble();

						newKeyframe.scale = prevFrame.scale;
					}
					else
					{
						newKeyframe.scale = prevFrame.scale;
					}

					if (keyframe.HasMember("translation"))
					{
						const auto& node = keyframe["translation"];
						newKeyframe.translation.x = node[0u].GetDouble();
						newKeyframe.translation.y = node[1u].GetDouble();
						newKeyframe.translation.z = node[2u].GetDouble();

						newKeyframe.translation = prevFrame.translation;
					}
					else
					{
						newKeyframe.translation = prevFrame.translation;
					}

					boneKeyframe.keyframes.push_back(newKeyframe);
				}

				animData.boneAnimations.push_back(boneKeyframe);
			}

			outNodes.push_back(animData);
		}
	}

	// Processing
	{
		// todo(jin02) : 
		// 나중에 binary 파일로 뽑아야하는 경우에 구현할 예정
	}

	delete buffer;
	return true;
}
