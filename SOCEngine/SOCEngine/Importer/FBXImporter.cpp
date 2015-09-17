#include "FBXImporter.h"
#include "Utility.h"
#include "FBXMeshNode.h"

using namespace Importer;
using namespace Importer::FBX;
using namespace fbxsdk_2014_1;

TinyFBXImporter::TinyFBXImporter()
	: _importer(nullptr), _sdkManager(nullptr),
	_animLayer(nullptr), _animStack(nullptr),
	_fbxScene(nullptr)
{
}

TinyFBXImporter::~TinyFBXImporter()
{
}

void TinyFBXImporter::Initialize(const std::string& filePath)
{
	FBX_SAFE_DELETE(_sdkManager);
	_sdkManager = FbxManager::Create();

	FbxIOSettings* ioSetting = FbxIOSettings::Create(_sdkManager, IOSROOT);
	_sdkManager->SetIOSettings(ioSetting);

	FbxString path = FbxGetApplicationDirectory();
#if defined (FBXSDK_ENV_WIN)
	FbxString IExtension = "dll";
#elif defined (FBXSDK_ENV_MAC)
	FbxString IExtension = "dylib";				
#elif defined (FBXSDK_ENV_LINUX)
	FbxString IExtension = "so";
#endif

	_sdkManager->LoadPluginsDirectory(path.Buffer(), IExtension.Buffer());
	_importer = FbxImporter::Create(_sdkManager, "");
}

void TinyFBXImporter::LoadScene(const std::string& filePath)
{
	int fileFormat = -1;
	if(_sdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(filePath.c_str(), fileFormat))
	{
        // Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
		fileFormat = _sdkManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");
	}

	bool success = _importer->Initialize(filePath.c_str(), fileFormat);
	ASSERT_COND_MSG(success, "Error, cant init fbxsdk importer");

	_fbxScene = FbxScene::Create(_sdkManager, filePath.c_str());
	ASSERT_COND_MSG(_fbxScene, "Error, can't load fbx scene");

	success = _importer->Import(_fbxScene);
	ASSERT_COND_MSG(success, "Error, can't import fbx Scene");

#if defined(WIN32) && !defined(_USE_GL_DEFINES)
	FbxAxisSystem axis = FbxAxisSystem::DirectX;
#elif defined(__APPLE__) || defined(_USE_GL_DEFINES)
	FbxAxisSystem axis = FbxAxisSystem::OpenGL;
#endif

	FbxAxisSystem sceneAxis = _fbxScene->GetGlobalSettings().GetAxisSystem();
	if(sceneAxis != axis)
		axis.ConvertScene(_fbxScene);

	FbxSystemUnit sceneSystemUnit = _fbxScene->GetGlobalSettings().GetSystemUnit();
	if(sceneSystemUnit.GetScaleFactor() != 1.0f)
		FbxSystemUnit::cm.ConvertScene(_fbxScene);

	Triangulate( _fbxScene->GetRootNode() );

	if(_fbxScene->GetRootNode())
	{
		auto MakeMeshData = [&](FbxNode* fbxNode, const MeshNode* parentNode)
		{
			if(fbxNode == nullptr)
				return;

			MeshNode* node = new MeshNode(parentNode);
			node->SetName(fbxNode->GetName());

			FbxMesh* fbxMesh = fbxNode->GetMesh();
			if(fbxMesh)
			{
				int vtxCount = fbxMesh->GetControlPointsCount();
				if(vtxCount > 0)
				{
				}
			}

			int materialCount = fbxNode->GetMaterialCount();
			for(uint i=0; i<materialCount; ++i)
			{
				FbxSurfaceMaterial* fbxMaterial = fbxNode->GetMaterial(i);
				if(fbxMaterial == nullptr)
					continue;


			}
		};

		std::string fileName = "null";
		Utility::String::ParseDirectory(filePath, nullptr, &fileName, nullptr);

		//MakeMeshData(_fbxScene->GetRootNode(), fileName);
	}
}

void TinyFBXImporter::Fuck(const FbxSurfaceMaterial* fbxSurfaceMaterial, MaterialNode& outMaterialNode)
{
	ASSERT_COND_MSG(fbxSurfaceMaterial, "fbxSurfaceMaterial is null");
	
	FbxClassId cid = fbxSurfaceMaterial->GetClassId();
	if(cid.Is(FbxSurfaceLambert::ClassId))
		outMaterialNode.type = MaterialNode::MaterialType::Lambert;
	else if(cid.Is(FbxSurfacePhong::ClassId))
		outMaterialNode.type = MaterialNode::MaterialType::Phong;

	auto ParseProperty = [&](const char* propertyName, const char* factorPropertyName, MaterialNodeElement& outElement)
	{
		FbxDouble3 resultColor = FbxDouble3(0, 0, 0);

		const FbxProperty& thisMatProperty	 = fbxSurfaceMaterial->FindProperty(propertyName);
		const FbxProperty& thisMatFactorProp = fbxSurfaceMaterial->FindProperty(factorPropertyName);
		if(thisMatProperty.IsValid())
		{
			if(thisMatFactorProp.IsValid())
			{
				resultColor = thisMatProperty.Get<FbxDouble3>();
				double factor = thisMatFactorProp.Get<FbxDouble>();
				if(factor != 1.0f)
				{
					resultColor[0] *= factor;
					resultColor[1] *= factor;
					resultColor[2] *= factor;
				}

				outElement.type = MaterialNodeElement::Type::Color;
			}

			uint texCount = 0;

			// Texture
			{
				uint fbxTexCount = thisMatProperty.GetSrcObjectCount<FbxFileTexture>();
				for(uint i=0; i<fbxTexCount; ++i, ++texCount)
				{
					FbxFileTexture* fbxTexture = thisMatProperty.GetSrcObject<FbxFileTexture>(i);
					if(fbxTexture == nullptr)
						continue;

					FbxString fbxUVSetName = fbxTexture->UVSet.Get();
					std::string uvSetName = fbxUVSetName.Buffer();
					std::string filePath = fbxTexture->GetFileName();

					outElement.textures.Add(uvSetName, filePath);
				}
			}

			// layered texture
			{
				uint fbxTexCount = thisMatProperty.GetSrcObjectCount<FbxLayeredTexture>();
				for(uint i=0; i<fbxTexCount; ++i)
				{
					FbxLayeredTexture* fbxLayeredTexture = thisMatProperty.GetSrcObject<FbxLayeredTexture>(i);
					uint textureFileCount = fbxLayeredTexture->GetSrcObjectCount<FbxFileTexture>();

					for(uint j=0; j<textureFileCount; ++j, ++texCount)
					{
						FbxFileTexture* fbxTexture = fbxLayeredTexture->GetSrcObject<FbxFileTexture>(i);
						if(fbxTexture == nullptr)
							continue;

						FbxString fbxUVSetName = fbxTexture->UVSet.Get();
						std::string uvSetName = fbxUVSetName.Buffer();
						std::string filePath = fbxTexture->GetFileName();

						outElement.textures.Add(uvSetName, filePath);					
					}
				}

				if(texCount > 0)
				{
					if(outElement.type == MaterialNodeElement::Type::Color)
						outElement.type = MaterialNodeElement::Type::ColorWithTexture;
					else
						outElement.type = MaterialNodeElement::Type::Texture;
				}
			}
		}

		return resultColor;
	};

}

void TinyFBXImporter::Triangulate(fbxsdk_2014_1::FbxNode* fbxNode)
{
	FbxNodeAttribute* nodeAttr = fbxNode->GetNodeAttribute();

    if (nodeAttr)
    {
        if (nodeAttr->GetAttributeType() == FbxNodeAttribute::eMesh ||
            nodeAttr->GetAttributeType() == FbxNodeAttribute::eNurbs ||
            nodeAttr->GetAttributeType() == FbxNodeAttribute::eNurbsSurface ||
            nodeAttr->GetAttributeType() == FbxNodeAttribute::ePatch)
        {
			FbxGeometryConverter lConverter(fbxNode->GetFbxManager());
			lConverter.Triangulate( _fbxScene, true );
        }
    }

	const int count = fbxNode->GetChildCount();
    for (int i = 0; i < count; ++i)
        Triangulate( fbxNode->GetChild(i) );
}