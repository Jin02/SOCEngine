#include "ObjImporter.h"
#include <fstream>
#include "Utility.h"

#include <map>

using namespace Importer;
using namespace Rendering::Material;

ObjImporter::ObjImporter()
{
}

ObjImporter::~ObjImporter()
{
}

bool ObjImporter::LoadMaterials(const std::string& fileNameWithExtension, const std::string& fileFolderPath, Rendering::Material::MaterialManager& materialMgr, Rendering::Texture::TextureManager& textureMgr)
{
	std::ifstream file;
	file.open(fileFolderPath + fileNameWithExtension);

	if(file.good() == false)
	{
		file.close();
		return false;
	}

	std::string fileName;
	{
		std::vector<std::string> t;
		Utility::Tokenize(fileName, t,  ".");
		fileName = t[0];
	}
	std::map<std::string, Material::LightColor> lightColors;

	std::string buff;
	while(std::getline(file, buff))
	{
		const char* buffData = (buff.data()[0] == '\t') ? buff.data()+1 : buff.data();

		std::vector<std::string> tokens;
		Utility::Tokenize(buffData, tokens);

		const std::string& type = tokens[0];

		std::string currentMaterial;
		if(type == "newmtl")
		{
			currentMaterial = tokens[1];

			Material* material = new Material(currentMaterial);
			materialMgr.Add(fileName + ":" + currentMaterial, material, false);

			Material::LightColor color;	
			lightColors.insert(std::make_pair(currentMaterial, color));
		}
		else if(type == "Ka")
		{
			Rendering::Color color( std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]) );
			lightColors[currentMaterial].ambient = color;
		}
		else if(type == "Ns")
		{
			float exp = std::stof(tokens[1]);
			lightColors[currentMaterial].specularExponent = exp;
		}
		else if(type == "Kd")
		{
			Rendering::Color color( std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]) );
			lightColors[currentMaterial].diffuse = color;
		}
		else if(type == "Ks")
		{
			Rendering::Color color( std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]) );
			lightColors[currentMaterial].specular = color;
		}
		else if(type == "Ke")
		{
			Rendering::Color color( std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]) );
			lightColors[currentMaterial].emissive = color;
		}
		else if(type == "map_Kd" || type == "map_Ks" || type == "map_bump" || type == "map_d")
		{
			std::string textureFileNameWithExtension = tokens[1];
			std::string textureFileName;
			{
				std::vector<std::string> temp1, temp2;
				Utility::Tokenize(textureFileNameWithExtension, temp1, "/");
				Utility::Tokenize((*temp1.rbegin()), temp2, ".");
				textureFileName = temp2[0];
			}

			Rendering::Texture::Texture* tex = 
				textureMgr.LoadTextureFromFile(fileFolderPath, fileNameWithExtension, 
				currentMaterial + ":" + textureFileName);

			Material* material = materialMgr.Find( fileName + ":" + currentMaterial );
			if(material)
			{
				if(type == "map_Kd")
					material->UpdateDiffuseMap(tex);
				else if(type == "map_Ks")
					material->UpdateSpecularMap(tex);
				else if(type == "map_bump")
					material->UpdateNormalMap(tex);
				else if(type == "map_d")
					material->UpdateOpacityMap(tex);
			}
		}
	}

	for(auto iter = lightColors.begin(); iter != lightColors.end(); ++iter)
	{
		const std::string& currentMaterial = iter->first;
		const std::string key = fileName + ":" + currentMaterial;

		Material* material = materialMgr.Find(key);
		material->SetLightColor(iter->second);
	}

	return true;
}

bool ObjImporter::Load(const std::string& fileName, const std::string& folderPath, Rendering::Material::MaterialManager& materialMgr, Rendering::Texture::TextureManager& textureMgr)
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

	std::vector<Math::Vector3>	vertices;
	std::vector<Math::Vector3>	normals;
	std::vector<Math::Vector2>	uvs;
	std::vector<ObjTriangle>	objTri;
	std::vector<std::pair<std::string, unsigned int>> materials;

	std::string line;
	while(std::getline(file, line))
	{
		std::vector<std::string> tokens;
		Utility::Tokenize(line, tokens);

		if(tokens.size() == 0)
			continue;

		const std::string& type = tokens[0];

		if(type == "mtlib") //material file
		{
			const std::string& materialFile = tokens[1];
			LoadMaterials(materialFile, folderPath, materialMgr, textureMgr);
		}
		else if(type == "usemtrl") //user material
		{
			const std::string& materialName = tokens[1]; 
			if(materials.size() == 0 || materials.rbegin()->first != materialName)
				materials.push_back(std::make_pair(materialName, objTri.size()));
		}
		else if(type == "f") //face			
		{
			//type, 0, 1, 2, etc만 해도 4개 혹은 5개니까?
			if(tokens.size() < 4 )
				continue;

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
						objTri.push_back(tri);
				}
			}
		}
		else if(type == "v" || type == "vn") //vertex or normal
		{
			if(tokens.size() != 4)
				continue;

			Math::Vector3 value;
			value.x = std::stof(tokens[0]);
			value.y = std::stof(tokens[1]);
			value.z = std::stof(tokens[2]);

			if(type == "vn")
				normals.push_back(value);
			else if(type == "v")
				vertices.push_back(value);
		}
		else if(type == "vt") //tex coord
		{
			if(tokens.size() != 3)
				continue;

			Math::Vector2 uv;
			uv.x = std::stof(tokens[0]);
			uv.y = std::stof(tokens[1]);

			uvs.push_back(uv);
		}	
	}

	//unsigned int numVertex = objTri.size() * 3;


	return true;
}