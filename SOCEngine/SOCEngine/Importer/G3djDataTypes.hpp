#pragma once

#include <string>
#include <vector>
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Quaternion.h"
#include "Matrix.h"

namespace Importer
{
	struct Node final
	{
		struct Parts final
		{
			std::string			meshPartID = "";
			std::string			materialID = "";

			struct Bone
			{
				std::string			node		= "";
				Math::Vector3		translation = Math::Vector3::Zero();
				Math::Quaternion	rotation	= Math::Quaternion(0.0f, 0.0f, 0.0f, 0.0f);
				Math::Vector3		scale		= Math::Vector3::Zero();
			};
			std::vector<Bone>	bones;
		};

		template <typename Type>
		struct Transform final
		{
			Transform() { memset(&tf, 0, sizeof(Type)); }

			bool has = false;
			Type tf;
		};

		Node() : localMatrix(Math::Matrix::Identity()), worldMatrix(Math::Matrix::Identity())
		{
			rotation.tf.w = 1.0f;
		}

		std::string						id;
		Transform<Math::Quaternion>		rotation;
		Transform<Math::Vector3>		translation;
		Transform<Math::Vector3>		scale;
		Math::Matrix					localMatrix;
		Math::Matrix					worldMatrix;

		std::vector<Parts>	parts;
		std::vector<Node>	childs;
	};

	struct Material
	{
		struct Texture final
		{
			enum class Type
			{
				Undefined,
				Ambient,
				Normal,
				Diffuse,
				Emissive,
				Reflection,
				Shininess,
				Specular,
				Transparency
			};

			std::string id			= "";
			std::string fileName	= "";
			Type		type		= Type::Undefined;
		};

		std::string		id			= "";
		Math::Vector3	ambient		= Math::Vector3(0.3f, 0.3f, 0.3f);
		Math::Vector3	diffuse		= Math::Vector3(1.0f, 1.0f, 1.0f);
		Math::Vector3	emissive	= Math::Vector3(0.0f, 0.0f, 0.0f);
		Math::Vector3	specular	= Math::Vector3(1.0f, 1.0f, 1.0f);
		float			opacity		= 1.0f;
		float			shininess	= 0.0f;

		std::vector<Texture> textures;
	};

	enum class Attribute : uint //In order of buffer data
	{
		Position	= 0,
		Normal		= 1,
		Tangent		= 2,
		Color		= 3,
		TexCoord	= 4,
		BlendWeight	= 5
	};

	struct Mesh final
	{
		struct Intersection final
		{
			float			radius			= 0.0f;
			Math::Vector3	boundBoxMin		= Math::Vector3(0.0f, 0.0f, 0.0f),
							boundBoxMax		= Math::Vector3(0.0f, 0.0f, 0.0f);
		};

		struct Part final
		{
			std::string meshPartID				= "";
			std::vector<unsigned int> indices;

			Intersection intersection;
		};

		std::vector<std::string>	attributes;
		std::vector<float>			vertexDatas;
		std::vector<Part>			parts;
	};
}