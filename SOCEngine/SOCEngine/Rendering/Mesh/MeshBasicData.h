#pragma once

#include "EngineMath.h"
#include <vector>
#include "Color.h"
#include <D3D11.h>
#include "Common.h"

namespace Rendering
{
	namespace Geometry
	{
		class MeshBasicTransportData
		{
		public:
			typedef int Count;

		public:
			Math::Vector3* vertices;
			Math::Vector3* normals;

			Math::Vector3* tangents;
			Math::Vector3* binormals;

			std::pair<Count, Math::Vector2**> texcoords;
			std::pair<Count, Color**> colors;

			std::pair<Count, ENGINE_INDEX_TYPE*>  indices;
	
			int			count;

		public:
			MeshBasicTransportData();
			~MeshBasicTransportData();

		public:
			bool CreateVertexDeclations(std::vector<D3D11_INPUT_ELEMENT_DESC>& out, const MeshBasicTransportData& datas);
		};


		//struct MeshBasicData
		//{
		//	Math::Vector3				vertex;
		//	Math::Vector3				normal;
		//	Math::Vector3				tangent;
		//	Math::Vector3				binormals;
		//	std::vector<Math::Vector2>	texCoords;
		//	std::vector<Color>			colors;
		//};
	}
}