#include "PhysicallyBasedMaterial.h"
#include "BindIndexInfo.h"

#undef min
#undef max

using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::Texture;
using namespace Rendering::Material;

void PhysicallyBasedMaterial::Initialize(Device::DirectX& dx)
{
	auto indexer = GetConstBuffers().GetIndexer();
	uint findIdx = indexer.Find(ParamCB::GetKey());

	// Error, param const buffer was already allocated
	assert(findIdx == ConstBuffers::IndexerType::FailIndex());

	BindConstBuffer bind;
	{
		ConstBuffer& paramCB = bind.resource;
		paramCB.Initialize(dx, sizeof(ParamCB));

		bind.bindIndex	= static_cast<uint>(ConstBufferBindIndex::PhysicallyBasedMaterial);
		bind.useCS		= false;
		bind.useGS		= false;
		bind.useVS		= false;
		bind.usePS		= true;
	}

	GetConstBuffers().Add(ParamCB::GetKey(), bind);
}

void PhysicallyBasedMaterial::Destroy()
{
}

void PhysicallyBasedMaterial::UpdateConstBuffer(Device::DirectX& dx)
{
	ParamCB param;

	param.mainColor_alpha		= (	(static_cast<uint>(_mainColor.r * 255.0f) & 0xff) << 24 |
									(static_cast<uint>(_mainColor.g * 255.0f) & 0xff) << 16 |
									(static_cast<uint>(_mainColor.b * 255.0f) & 0xff) << 8 |
									(static_cast<uint>(_mainColor.a * 255.0f) & 0xff)	);

	param.emissiveColor_Metallic = ((static_cast<uint>(_emissiveColor.r * 255.0f) & 0xff) << 24 |
									(static_cast<uint>(_emissiveColor.g * 255.0f) & 0xff) << 16 |
									(static_cast<uint>(_emissiveColor.b * 255.0f) & 0xff) << 8 |
									(static_cast<uint>(_metallic * 255.0f) & 0xff));

	uint scaledSpecularity	= static_cast<uint>(_specularity * 255.0f) & 0xff;
	uint scaledRoughness	= static_cast<uint>(_roughness * 255.0f) & 0xff;

	uint existTextureFlag = 0;
	{
		const auto& indexer = GetTextures().GetIndexer();

		existTextureFlag |= static_cast<uint>(indexer.Has(GetDiffuseMapKey()))		<< 0;
		existTextureFlag |= static_cast<uint>(indexer.Has(GetNormalMapKey()))		<< 1;
		existTextureFlag |= static_cast<uint>(indexer.Has(GetOpacityMapKey()))		<< 2;
		existTextureFlag |= static_cast<uint>(indexer.Has(GetHeightMapKey()))		<< 3;
		existTextureFlag |= static_cast<uint>(indexer.Has(GetMetallicMapKey()))		<< 4;
		existTextureFlag |= static_cast<uint>(indexer.Has(GetOcclusionMapKey()))	<< 5;
		existTextureFlag |= static_cast<uint>(indexer.Has(GetRoughnessMapKey()))	<< 6;
		existTextureFlag |= static_cast<uint>(indexer.Has(GetEmissionMapKey()))		<< 7;
	}

	uint resultFlag = (scaledRoughness << 24) | (scaledSpecularity << 16) | (existTextureFlag & 0xffff);
	param.roughness_specularity_existTextureFlag = resultFlag;
		
	param.iblMin_flag_ior =	(static_cast<uint>(std::min(_iblMin * 15.0f, 15.0f)) << 12)	|
							(static_cast<uint>(_flag) << 8)								|
							(static_cast<uint>(_ior * 255.0f));

	auto indexer	= GetConstBuffers().GetIndexer();
	uint findIndex	= indexer.Find(ParamCB::GetKey());
	assert(findIndex != decltype(indexer)::FailIndex());

	GetConstBuffers().Get(findIndex).resource.UpdateSubResource(dx, &param);
	_dirty = false;	
}

void PhysicallyBasedMaterial::RegistTexture(const std::string& key, TextureBindIndex bind, const Texture::Texture2D& tex)
{
	auto& textureBook = GetTextures();
	if (textureBook.Has(key) == false)
	{
		BindTextured2D bindTex2D;
		{
			bindTex2D.resource	= tex;
			bindTex2D.bindIndex	= static_cast<uint>(bind);
			bindTex2D.useVS		= bindTex2D.useGS = bindTex2D.useCS = false;
			bindTex2D.usePS		= true;
		}
		textureBook.Add(key, bindTex2D);
	}
	else
	{
		uint findIdx = textureBook.GetIndexer().Find(key);
		textureBook.Get(findIdx).resource = tex;
	}
}
