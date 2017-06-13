#pragma once

#include "ConstBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "GeometryShader.h"
#include "ShadowId.hpp"

namespace Rendering
{
	namespace Shadow
	{
		namespace Buffer
		{
			template <class ShadowType>
			class ShadowMapCB
			{
			public:
				using ConstBufferType = Rendering::Buffer::ExplicitConstBuffer<typename ShadowType::ViewProjMatType>;

			public:
				void Add(Shadow::ShadowId id)
				{
					_notInitializedCBs(&_vpCBs.Add(id, ConstBufferType()));
				}
				void Delete(Shadow::ShadowId id)
				{
					_vpCBs.Delete(id.Literal());
				}
				void UpdateSubResource(Device::DirectX& dx, std::vector<ShadowType*>& dirtyShadows)
				{
					for (auto shadow : dirtyShadows)
					{
						uint id = shadow->GetShadowId().Literal();
						auto find = _vpCBs.Find(id); assert(find);

						find->UpdateSubResource(dx, shadow->GetViewProjectionMatrix());
					}
				}
				void BindConstBuffer(Device::DirectX& dx, Shadow::ShadowId shadowId, bool bindVS, bool bindGS, bool bindPS)
				{
					auto cb = _vpCBs.Find(shadowId.Literal());
					assert(cb); // not found!

					if (bindVS) Shader::VertexShader::BindConstBuffer(dx, ConstBufferBindIndex::OnlyPass, *cb);
					if (bindGS) Shader::GeometryShader::BindConstBuffer(dx, ConstBufferBindIndex::OnlyPass, *cb);
					if (bindPS) Shader::PixelShader::BindConstBuffer(dx, ConstBufferBindIndex::OnlyPass, *cb);
				}
				void UnbindConstBuffer(Device::DirectX& dx, Shadow::ShadowId shadowId, bool bindVS, bool bindGS, bool bindPS)
				{
					if (bindVS) Shader::VertexShader::UnBindConstBuffer(dx, ConstBufferBindIndex::OnlyPass);
					if (bindGS) Shader::GeometryShader::UnBindConstBuffer(dx, ConstBufferBindIndex::OnlyPass);
					if (bindPS) Shader::PixelShader::UnBindConstBuffer(dx, ConstBufferBindIndex::OnlyPass);
				}
				void Init_NotInitedCBs(Device::DirectX& dx)
				{
					for (auto cb : _notInitializedCBs)
						cb->Initialize(dx);

					_notInitializedCBs.clear();
				}

			private:
				Core::VectorHashMap<Shadow::ShadowId::LiteralType, ConstBufferType> _vpCBs;
				std::vector<ConstBufferType*> _notInitializedCBs;
			};
		}
	}
}