#pragma once

#include <fbxsdk.h>
#include <string>
#include "FBXMaterialNode.h"

namespace Importer
{
	namespace FBX
	{
		class TinyFBXImporter
		{
		private:
			fbxsdk_2014_1::FbxImporter*			_importer;
			fbxsdk_2014_1::FbxManager*			_sdkManager;
			fbxsdk_2014_1::FbxAnimLayer*		_animLayer;
			fbxsdk_2014_1::FbxAnimStack*		_animStack;
			fbxsdk_2014_1::FbxScene*			_fbxScene;

		public:
			TinyFBXImporter();
			~TinyFBXImporter();

		public:
			void Initialize(const std::string& filePath);
			void LoadScene(const std::string& filePath);
			void Triangulate(fbxsdk_2014_1::FbxNode* fbxNode);
			void Fuck(const fbxsdk_2014_1::FbxSurfaceMaterial* fbxSurfaceMaterial, MaterialNode& outMaterialNode);
		};
	}
}