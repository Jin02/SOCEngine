#include "MeshBasicData.h"

using namespace Rendering::Geometry;

MeshBasicTransportData::MeshBasicTransportData()
{
}

MeshBasicTransportData::~MeshBasicTransportData()
{
}

bool MeshBasicTransportData::CreateVertexDeclations(std::vector<D3D11_INPUT_ELEMENT_DESC>& out, const MeshBasicTransportData& datas)
{
	//그냥 하드코딩 함 ㅇㅇ.
	auto SetInputDesc = [](D3D11_INPUT_ELEMENT_DESC &out, const char* semanticName, unsigned int semanticIdx, DXGI_FORMAT foramt, unsigned int alignedByteOffset)
	{
		out.SemanticName = semanticName;
		out.SemanticIndex = semanticIdx;
		out.Format = foramt;
		out.InputSlot = 0;
		out.AlignedByteOffset = alignedByteOffset;
		out.InputSlotClass  = D3D11_INPUT_PER_VERTEX_DATA;
		out.InstanceDataStepRate = 0;
	};

	D3D11_INPUT_ELEMENT_DESC desc;

	if(datas.vertices == nullptr)
		return false;
	SetInputDesc(desc, "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0);
	out.push_back(desc);
	unsigned int size = sizeof(Math::Vector4);

	if(datas.normals)
	{
		SetInputDesc(desc, "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, size);
		size+=sizeof(Math::Vector3);
		out.push_back(desc);
	}

	if(datas.tangents)
	{
		SetInputDesc(desc, "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, size);
		size+=sizeof(Math::Vector3);
		out.push_back(desc);
	}

	if(datas.binormals)
	{
		SetInputDesc(desc, "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, size);
		size+=sizeof(Math::Vector3);
		out.push_back(desc);
	}

	if(datas.texcoords.first != 0)
	{
		int count = datas.texcoords.first;
		for(int i=0; i<count; ++i)
		{
			SetInputDesc(desc, "TEXCOORD", i, DXGI_FORMAT_R32G32B32_FLOAT, size);
			size+=sizeof(Math::Vector2);
			out.push_back(desc);
		}
	}

	if(datas.colors.first != 0)
	{
		int count = datas.texcoords.first;
		for(int i=0; i<count; ++i)
		{
			SetInputDesc(desc, "COLOR", i, DXGI_FORMAT_R32G32B32_FLOAT, size);
			size+=sizeof(Math::Vector4);
			out.push_back(desc);
		}
	}

	return true;
}
