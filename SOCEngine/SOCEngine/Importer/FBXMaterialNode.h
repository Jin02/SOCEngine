#pragma once

#include "VectorMap.h"

namespace Importer
{
	namespace FBX
	{
		struct MaterialNodeElement
		{
		public:
			enum class Type
			{
				None, Color, Texture, ColorWithTexture
			};

		public:
			Type			type;
			float			r,g,b,a;

			// key is UVSetName, value is file path.
			Structure::VectorMap<std::string, std::string> textures;

			MaterialNodeElement() : r(0), g(0), b(0), a(0), type(Type::None){}
			~MaterialNodeElement(){}
		};

		struct MaterialNode
		{
		public:
			enum class MaterialType
			{
				Unknown,
				Lambert,
				Phong
			};

		public:
			MaterialType			type;
			MaterialNodeElement		diffuse;
			MaterialNodeElement		normal;
			MaterialNodeElement		specular;


		public:
			MaterialNode() : type(MaterialType::Unknown) {}
			~MaterialNode(){}
		};
	}
}