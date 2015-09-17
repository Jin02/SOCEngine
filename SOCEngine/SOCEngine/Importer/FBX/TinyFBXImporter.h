#pragma once

#include <fbxsdk.h>
#include <string>
#include "FBXTinyScene.h"
#include "VectorMap.h"

namespace Importer
{
	namespace FBX
	{
		class TinyFBXImporter
		{
		private:
			fbxsdk_2014_1::FbxImporter*			_importer;
			fbxsdk_2014_1::FbxManager*			_sdkManager;

		private:
			Structure::VectorMap<std::string, TinyFBXScene*>	_tinyScenes;

		public:
			TinyFBXImporter();
			~TinyFBXImporter();

		public:
			void Initialize();
			void Destroy();
			const TinyFBXScene* LoadScene(const std::string& filePath);
		};
	}
}