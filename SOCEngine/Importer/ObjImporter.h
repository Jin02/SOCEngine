#pragma once

#include <string>
#include <fstream>

#include "Utility.h"
#include "Vector2.h"
#include "Vector3.h"

namespace Importer
{
	class ObjImporter
	{
	private:
		struct ObjTriangle
		{
			int vertex[3];
			int normal[3];
			int uvs[3];
		};

	public:
		ObjImporter();
		~ObjImporter();

	private:
		void ParseLine(const std::string& line, std::vector<Math::Vector3>& outVertex, std::vector<Math::Vector3>& outNormal, std::vector<Math::Vector2>& outUV, std::vector<ObjTriangle>& outTri, std::vector<std::pair<std::string, unsigned int>>& outMaterials)
		{
			std::vector<std::string> tokens;
			Utility::Tokenize(line, tokens);

			if(tokens.size() == 0)
				return;

			const std::string& type = tokens[0];

			if(type == "mtlib") //material file
			{
				const std::string& materialFile = tokens[1];
			}
			else if(type == "usemtrl") //user material
			{
				const std::string& materialName = tokens[1]; 
				if(outMaterials.size() == 0 || outMaterials.rbegin()->first != materialName)
					outMaterials.push_back(std::make_pair(materialName, outTri.size()));
			}
			else if(type == "f") //face			
			{
				//type, 0, 1, 2, etc만 해도 4개 혹은 5개니까?
				if(tokens.size() < 4 )
					return;

				ObjTriangle tri;
				auto ParseFaceIndex = [](ObjTriangle& out, const std::string& tokenDatas, int idx)
				{
					out.vertex[idx] = -1;
					out.uvs[idx] = -1;
					out.normal[idx] = -1;

					std::vector<std::string> faceTokens;
					Utility::Tokenize(tokenDatas, faceTokens, "/");

					if(faceTokens.size() == 3)
					{
						out.vertex[idx] = std::stoi(faceTokens[0]) - 1;
						out.uvs[idx] = std::stoi(faceTokens[1]) - 1;
						out.normal[idx] = std::stoi(faceTokens[2]) - 1;
					}
					else if(faceTokens.size() == 2)
					{
						out.vertex[idx] = std::stoi(faceTokens[0]) - 1;
						out.normal[idx] = std::stoi(faceTokens[1]) - 1;
					}
					else
						return false;

					return true;
				};

				bool find0 = ParseFaceIndex(tri, tokens[1], 0);
				bool find1 = ParseFaceIndex(tri, tokens[2], 1);
				if( find0 && find1 )
				{
					for(int i=3; i<tokens.size(); ++i)
					{
						if( ParseFaceIndex(tri, tokens[i], 2) )
							outTri.push_back(tri);
					}
				}
			}
			else if(type == "v" || type == "vn") //vertex or normal
			{
				if(tokens.size() != 4)
					return;

				Math::Vector3 value;
				value.x = std::stof(tokens[0]);
				value.y = std::stof(tokens[1]);
				value.z = std::stof(tokens[2]);

				if(type == "vn")
					outNormal.push_back(value);
				else if(type == "v")
					outVertex.push_back(value);
			}
			else if(type == "vt") //tex coord
			{
				if(tokens.size() != 3)
					return;

				Math::Vector2 uv;
				uv.x = std::stof(tokens[0]);
				uv.y = std::stof(tokens[1]);

				outUV.push_back(uv);
			}
		}

	public:
		bool Load(const std::string& fileName)
		{
			std::string path = ""; //나중에 Directory Manager 붙여야함
			path += fileName;

			if( fileName.find(".obj") == std::string::npos )
				path += ".obj";

			std::ifstream file;
			file.open(path);

			if(file.good() == false)
			{
				file.close();
				return false;
			}

			std::string buff;
			while(std::getline(file, buff))
			{

			}
			return true;
		}
	};
}