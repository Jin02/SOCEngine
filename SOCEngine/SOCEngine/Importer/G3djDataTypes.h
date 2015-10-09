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
	struct Node
	{
		struct Parts
		{
			std::string meshPartId;
			std::string materialId;
			//std::vector<uint> uvMapping;
		};
		template <typename Type>
		struct Transform
		{
			bool has;
			Type tf;
			Transform() : has(false) { memset(&tf, 0, sizeof(Type)); }
			~Transform(){}
		};

		std::string						id;
		Transform<Math::Quaternion>		rotation;
		Transform<Math::Vector3>		translation;
		Transform<Math::Vector3>		scale;
		Math::Matrix					localMatrix;
		Math::Matrix					worldMatrix;

		std::vector<Parts>	parts;
		std::vector<Node>	childs;

		Node() : rotation(), translation(), scale()
		{
			rotation.tf.w = 1.0f;

			Math::Matrix::Identity(localMatrix);
			Math::Matrix::Identity(worldMatrix);
		}
		~Node(){}
	};

	struct Material
	{
		struct Texture
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

			std::string id;
			std::string fileName;
			Type		type;
		};

		std::string		id;
		Math::Vector3	ambient;
		Math::Vector3	diffuse;
		Math::Vector3	emissive;
		Math::Vector3	specular;
		float			opacity;
		float			shininess;

		std::vector<Texture> textures;
		Material() : ambient(0.5f, 0.5f, 0.5f), diffuse(1.f, 1.f, 1.f), emissive(0.f, 0.f, 0.f), specular(1.f, 1.f, 1.f), opacity(1.0f), shininess(0.0f) {}
		~Material(){}
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

	struct Mesh
	{
		struct Intersection
		{
			float radius;
			Math::Vector3 boundBoxMin, boundBoxMax;

			Intersection() : radius(0.0f), boundBoxMin(0.0f, 0.0f, 0.0f), boundBoxMax(0.0f, 0.0f, 0.0f) {}
			~Intersection() {}
		};

		struct Part
		{
			std::string meshPartId;
			std::vector<unsigned int> indices;

			Intersection intersection;

			Part() : intersection() {}
			~Part(){}
		};

		std::vector<std::string>	attributes;
		std::vector<float>			vertexDatas;
		std::vector<Part>			parts;
	};
}