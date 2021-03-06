#include "Mesh.h"
#include "DefaultRenderTypes.h"
#include "DefaultShaders.h"
#include "MeshManager.h"
#include "Utility.hpp"

using namespace Device;
using namespace Rendering;
using namespace Rendering::Geometry;
using namespace Rendering::Shader;
using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Core;
using namespace Utility;
using namespace Math;
using namespace Intersection;

void Mesh::Initialize(Device::DirectX& dx, BufferManager& bufferMgr, const CreateFuncArguments& args)
{
	uint vertexCount	= args.vertices.count;
	uint indexCount		= args.indices.size();

	_vbKey = Utility::String::MakeKey({ args.fileName, std::to_string(args.vbUserHashKey) });
	if (bufferMgr.GetPool<VertexBuffer>().Has(_vbKey) == false)
	{
		VertexBuffer::Desc param;
		{
			param.stride		= args.vertices.byteWidth;
			param.vertexCount	= args.vertices.count;
		}

		VertexBuffer vb;
		vb.Initialize(dx, param, args.vertices.data, args.useDynamicVB, args.semanticInfos);

		bufferMgr.GetPool<VertexBuffer>().Add(_vbKey, vb);
	}

	_ibKey = String::MakeKey({ args.fileName, args.ibPartID });
	if (bufferMgr.GetPool<IndexBuffer>().Has(_ibKey) == false)
	{
		IndexBuffer ib;
		ib.Initialize(dx, args.indices, _vbKey, args.useDynamicIB);

		bufferMgr.GetPool<IndexBuffer>().Add(_ibKey, ib);
	}	

	_bufferFlag = ComputeBufferFlag(args.semanticInfos);

	_transformCB.Initialize(dx);
}

void Mesh::Initialize(DirectX& dx, const VertexBuffer::Semantics& semantics, BaseBuffer::Key vbKey, BaseBuffer::Key ibKey)
{
	_vbKey = vbKey;
	_ibKey = ibKey;

	_bufferFlag = ComputeBufferFlag(semantics);

	_transformCB.Initialize(dx);
}

void Mesh::Destroy()
{
	_transformCB.Destroy();
}

uint Mesh::ComputeBufferFlag(const std::vector<VertexShader::SemanticInfo>& semantics, uint maxRecognizeBoneCount) const
{
	uint flag = 0;
	for (auto iter = semantics.begin(); iter != semantics.end(); ++iter)
	{
		const auto& semantic = *iter;

		if (semantic.name == "POSITION")		continue;
		else if (semantic.name == "TEXCOORD")
		{
			if (semantic.semanticIndex > 1)
				flag |= static_cast<uint>(DefaultVertexInputTypeFlag::USERS);
			else
				flag |= static_cast<uint>(DefaultVertexInputTypeFlag::UV0) << semantic.semanticIndex;
		}
		else if (semantic.name == "NORMAL")
			flag |= static_cast<uint>(DefaultVertexInputTypeFlag::NORMAL);
		else if (semantic.name == "TANGENT")
			flag |= static_cast<uint>(DefaultVertexInputTypeFlag::TANGENT);
		else if (semantic.name == "BINORMAL")
			flag |= static_cast<uint>(DefaultVertexInputTypeFlag::BINORMAL);
		else if (semantic.name == "COLOR")
			flag |= static_cast<uint>(DefaultVertexInputTypeFlag::COLOR);
		else if (semantic.name == "BLENDWEIGHT")
		{
			if (semantic.semanticIndex < maxRecognizeBoneCount)
				flag |= static_cast<uint>(DefaultVertexInputTypeFlag::BONE) << semantic.semanticIndex;
			else
				flag |= static_cast<uint>(DefaultVertexInputTypeFlag::USERS);
		}
		else
		{
			flag |= static_cast<uint>(DefaultVertexInputTypeFlag::USERS);
		}
	}

	// Error, You use undefined semantic in RenderManager::DefaultVertexInputTypeFlag.
	assert((flag & static_cast<uint>(DefaultVertexInputTypeFlag::USERS)) == 0);

	return flag;
}

void Mesh::CalcWorldSize(Math::Vector3& worldMin, Math::Vector3& worldMax, const Core::Transform& transform) const
{
	Vector3 extents		= _boundBox.GetExtents();
	Vector3 boxCenter	= _boundBox.GetCenter();

	const Matrix& worldMat	= transform.GetWorldMatrix();
	Vector3 worldPos		= Vector3(worldMat._41, worldMat._42, worldMat._43) + boxCenter;

	Vector3 worldScale = transform.GetWorldScale();

	Vector3 minPos = worldPos - (extents * worldScale);
	Vector3 maxPos = worldPos + (extents * worldScale);

	if (worldMin.x > minPos.x) worldMin.x = minPos.x;
	if (worldMin.y > minPos.y) worldMin.y = minPos.y;
	if (worldMin.z > minPos.z) worldMin.z = minPos.z;

	if (worldMax.x < maxPos.x) worldMax.x = maxPos.x;
	if (worldMax.y < maxPos.y) worldMax.y = maxPos.y;
	if (worldMax.z < maxPos.z) worldMax.z = maxPos.z;
}

void Mesh::UpdateTransformCB(DirectX& dx, const Transform& transform)
{
	assert(transform.GetObjectID() == _objectID);

	if(transform.GetDirty())
		_tfChangeState = TransformCB::ChangeState::HasChanged;

	assert(_tfChangeState != TransformCB::ChangeState::No);

	const Matrix& worldMat = transform.GetWorldMatrix();
	
	TransformCB tfCB;
	tfCB.world				= Matrix::Transpose(worldMat);
	tfCB.prevWorld			= Matrix::Transpose(_prevWorldMat);
	tfCB.worldInvTranspose	= Matrix::Inverse(worldMat);
	_transformCB.UpdateSubResource(dx, tfCB);

	_prevWorldMat = worldMat;
	_tfChangeState = TransformCB::ChangeState((static_cast<uint>(_tfChangeState) + 1) % static_cast<uint>(TransformCB::ChangeState::MAX));
}
