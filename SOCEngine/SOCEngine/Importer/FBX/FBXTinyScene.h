#pragma once

#include <fbxsdk.h>
#include <string>
#include "FBXMaterial.h"
#include "FBXVertex.h"
#include "FBXUtilities.h"
#include <unordered_map>
#include "Common.h"
#include <unordered_map>

namespace Importer
{
	namespace FBX
	{
		typedef std::unordered_map<std::string, FbxAnimStack*> FbxAnimationStackMap; 

		class TinyFBXScene
		{
		public:
			struct Object
			{
				std::string					name;

				Object*						parent;
				std::vector<Object*>		childs;

				std::vector<Triangle>		triangles;
				std::vector<PNTIWVertex>	vertices;

				std::unordered_map<unsigned int, Material*>		materialLookUp;

				Object(Object* parent, const std::string& name);
				~Object();
			};

		private:
			fbxsdk_2014_1::FbxImporter*			_importer;
			fbxsdk_2014_1::FbxManager*			_sdkManager;
//			fbxsdk_2014_1::FbxAnimLayer*		_animLayer;
//			fbxsdk_2014_1::FbxAnimStack*		_animStack;
			fbxsdk_2014_1::FbxScene*			_fbxScene;

//			FbxAnimationStackMap				_animationStackMap;

		private:
//			bool	_hasAnimation;

			Skeleton					_skeleton;
//			FbxLongLong					_animationLength;
//			std::string					_animationName;

			std::unordered_map<unsigned int, Material*>		_materialLookUp;
			std::unordered_map<unsigned int, CtrlPoint*>	_controlPoints;

		public:
			TinyFBXScene(fbxsdk_2014_1::FbxImporter* importer, fbxsdk_2014_1::FbxManager* sdkManager);
			~TinyFBXScene();

		public:
			Object* LoadScene(const std::string& filePath);
			void Cleanup();

		private:
			void Triangulate(fbxsdk_2014_1::FbxNode* fbxNode);
			void ProcessSkeletonHierarchy(fbxsdk_2014_1::FbxNode* inRootNode);
			void ProcessSkeletonHierarchyRecursively(fbxsdk_2014_1::FbxNode* inNode, int inDepth, int myIndex, int inParentIndex);
			void ProcessGeometry(fbxsdk_2014_1::FbxNode* inNode, Object* parent);
			void ProcessControlPoints(fbxsdk_2014_1::FbxNode* inNode);
			void ProcessJointsAndAnimations(fbxsdk_2014_1::FbxNode* inNode);
			unsigned int FindJointIndexUsingName(const std::string& inJointName);
			void ProcessMesh(fbxsdk_2014_1::FbxNode* inNode, Object& obj);
			void ReadUV(fbxsdk_2014_1::FbxMesh* inMesh, int inCtrlPointIndex, int inTextureUVIndex, int inUVLayer, Math::Vector2& outUV);
			void ReadNormal(fbxsdk_2014_1::FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, Math::Vector3& outNormal);
			void ReadBinormal(fbxsdk_2014_1::FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, Math::Vector3& outBinormal);
			void ReadTangent(fbxsdk_2014_1::FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, Math::Vector3& outTangent);
			void Optimize(Object& obj);
			int  FindVertex(const PNTIWVertex& inTargetVertex, const std::vector<PNTIWVertex>& uniqueVertices);
			void AssociateMaterialToMesh(fbxsdk_2014_1::FbxNode* inNode, Object& obj);
			void ProcessMaterials(fbxsdk_2014_1::FbxNode* inNode);
			void ProcessMaterialAttribute(fbxsdk_2014_1::FbxSurfaceMaterial* inMaterial, unsigned int inMaterialIndex);
			void ProcessMaterialTexture(fbxsdk_2014_1::FbxSurfaceMaterial* inMaterial, Material* ioMaterial);

		public:
//			GET_ACCESSOR(Triangles, const std::vector<Triangle>&, _triangles);
//			GET_ACCESSOR(Vertices, const std::vector<PNTIWVertex>&, _vertices);
			GET_ACCESSOR(Skeleton, const Skeleton&, _skeleton);
//			GET_ACCESSOR(AnimationLength, const FbxLongLong&, _animationLength);
//			GET_ACCESSOR(AnimationName, const std::string&, _animationName);
			inline const std::unordered_map<unsigned int, Material*>& MaterialLookUp() const { return _materialLookUp; }
			inline const std::unordered_map<unsigned int, CtrlPoint*>& ControlPoints() const { return _controlPoints; }
//			GET_ACCESSOR(TriangleCount, uint, _triangleCount);
//			GET_ACCESSOR(HasAnimation, bool, _hasAnimation);
		};
	}
}