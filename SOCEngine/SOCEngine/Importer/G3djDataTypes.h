#pragma once

#include <string>
#include <vector>
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Quaternion.h"

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

		std::string			id;
		Math::Quaternion	rotation;
		Math::Vector3		translation;
		Math::Vector3		scale;

		std::vector<Parts>	parts;
		std::vector<Node>	childs;

		Node() : rotation(0.f, 0.f, 0.f, 1.f), translation(0.f, 0.f, 0.f), scale(1.f, 1.f, 1.f){}
		~Node(){}
	};

	struct Material
	{
		struct Texture
		{
			std::string id;
			std::string fileName;
			std::string type;
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

	struct Mesh
	{
		struct Part
		{
			std::string meshPartId;
//			std::string type;
			std::vector<unsigned int> indices;
		};

		std::string					id;
		std::vector<std::string>	attributes;
		std::vector<float>			vertexDatas;
		std::vector<Part>			parts;
	};
}