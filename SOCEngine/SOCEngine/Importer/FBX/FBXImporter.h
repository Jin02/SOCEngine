#pragma once

#include <fbxsdk.h>
#include <string>
#include "TinyFBXImporter.h"

namespace Importer
{
	namespace FBX
	{
		class FBXImporter
		{
		private:
			TinyFBXImporter* _importer;

		public:
			FBXImporter();
			~FBXImporter();

		public:
			void Initialize();
			void Destroy();

		public:
			void LoadOnlyMesh(const std::string& filePath);
		};
	}
}