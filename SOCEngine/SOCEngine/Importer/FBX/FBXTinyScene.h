#pragma once

#include <fbxsdk.h>
#include <string>
#include "FBXMaterial.h"
#include "FBXVertex.h"
#include "FBXUtilities.h"
#include <unordered_map>
#include "Common.h"
#include <unordered_map>
#include "Matrix.h"

namespace Importer
{
	namespace FBX
	{
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

				Math::Matrix				transform;

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
			std::unordered_map<unsigned int, CtrlPoint>		_controlPoints;

		public:
			TinyFBXScene(fbxsdk_2014_1::FbxImporter* importer, fbxsdk_2014_1::FbxManager* sdkManager);
			~TinyFBXScene();

		public:
			Object* LoadScene(const std::string& filePath);
			void Cleanup();

		private:
			void ComputeTransformMatrix(fbxsdk_2014_1::FbxNode* fbxNode, Object& obj);
			void Triangulate(fbxsdk_2014_1::FbxNode* fbxNode);
			void ProcessSkeletonHierarchy(fbxsdk_2014_1::FbxNode* inRootNode);
			void ProcessSkeletonHierarchyRecursively(fbxsdk_2014_1::FbxNode* inNode, int inDepth, int myIndex, int inParentIndex);
			void ProcessGeometry(fbxsdk_2014_1::FbxNode* inNode, Object* parent);
			void ProcessControlPoints(fbxsdk_2014_1::FbxNode* inNode);
			void ProcessJointsAndAnimations(fbxsdk_2014_1::FbxNode* inNode);
			unsigned int FindJointIndexUsingName(const std::string& inJointName);
			void ProcessMesh(fbxsdk_2014_1::FbxNode* inNode, Object& obj);
			void Optimize(Object& obj);
			void AssociateMaterialToMesh(fbxsdk_2014_1::FbxNode* inNode, Object& obj);
			void ProcessMaterials(fbxsdk_2014_1::FbxNode* inNode);
			void ProcessMaterialAttribute(fbxsdk_2014_1::FbxSurfaceMaterial* inMaterial, unsigned int inMaterialIndex);
			void ProcessMaterialTexture(fbxsdk_2014_1::FbxSurfaceMaterial* inMaterial, Material* ioMaterial);

			template <typename ElementType>
			bool ParseElements(ElementType e, int ctrlPointIdx, int vertexIdx, int *outIdx)
			{
				if(e == nullptr)
					return false;

				int index = -1;
				fbxsdk_2014_1::FbxLayerElement::EMappingMode mappingMode = e->GetMappingMode();
				fbxsdk_2014_1::FbxLayerElement::EReferenceMode refMode = e->GetReferenceMode();

				if(mappingMode == fbxsdk_2014_1::FbxLayerElement::eByControlPoint)
				{
					if(refMode == fbxsdk_2014_1::FbxLayerElement::eDirect)
						index = ctrlPointIdx;
					else if(refMode == Ffbxsdk_2014_1::bxLayerElement::eIndexToDirect)
						index = e->GetIndexArray().GetAt(ctrlPointIdx);
				}
				else if(mappingMode == fbxsdk_2014_1::FbxLayerElement::eByPolygonVertex)
				{
					if(refMode == fbxsdk_2014_1::FbxLayerElement::eDirect)
						index = vertexIdx;
					else if(refMode == fbxsdk_2014_1::FbxLayerElement::eIndexToDirect)
						index = e->GetIndexArray().GetAt(vertexIdx);
				}

				*outIdx = index;

				return index != -1;
			}
			bool ParseNormals(FbxLayer*& layer, int index, int vertexIdx, Math::Vector3& out);
			bool ParseUV(FbxLayer* layer, FbxMesh* fbxMesh, int ctrlPointIdx, int polygonIdx, int vertexIdx, int pointIdx, Math::Vector2& out);
			bool ParseTangents(FbxLayer* layer, int ctrlPointIdx, int vertexCount, Math::Vector3& out);
			bool ParseBinormals(FbxLayer* layer, int ctrlPointIdx, int vertexCount, Math::Vector3& out);

		public:
			GET_ACCESSOR(Skeleton, const Skeleton&, _skeleton);
			//			GET_ACCESSOR(AnimationLength, const FbxLongLong&, _animationLength);
			//			GET_ACCESSOR(AnimationName, const std::string&, _animationName);
			inline const std::unordered_map<unsigned int, Material*>& MaterialLookUp() const { return _materialLookUp; }
			//			GET_ACCESSOR(HasAnimation, bool, _hasAnimation);
		};
	}
}