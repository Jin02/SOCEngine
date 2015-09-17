#include "FBXMeshNode.h"

using namespace Importer::FBX;

MeshNode::MeshNode(const MeshNode* parent)
	: _parentMesh(parent)
{
}

MeshNode::~MeshNode()
{
}