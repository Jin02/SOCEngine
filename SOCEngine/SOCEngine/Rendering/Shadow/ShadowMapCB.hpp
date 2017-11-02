#pragma once

#include "ConstBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "GeometryShader.h"

#include <queue>

#include "ShadowBufferForm.hpp"

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
				void PushConstBufferToQueue()
				{
					_preparedConstBuffers.push_back(ConstBufferType());
				}

				void InitializePreparedCB(Device::DirectX& dx)
				{
					if (_preparedConstBuffers.empty())
						return;

					for(auto& iter : _preparedConstBuffers)
						iter.Initialize(dx);

					_constBuffers.insert(_constBuffers.end(), _preparedConstBuffers.begin(), _preparedConstBuffers.end());
					_preparedConstBuffers.clear();
				}

				void Delete(uint index)
				{
					auto iter = _constBuffers.begin() + index;
					_constBuffers.erase(iter);
				}
				
				void DeleteAll()
				{
					_constBuffers.clear();
					_preparedConstBuffers.clear();
				}
				
				void UpdateSubResource(Device::DirectX& dx,
					const std::vector<ShadowType*>& dirtyShadows,
					const ShadowDatasIndexer& indexer)
				{
					for (auto shadow : dirtyShadows)
					{
						Core::ObjectID objID	= shadow->GetObjectID();
						uint index				= indexer.Find(objID.Literal());

						_constBuffers[index].UpdateSubResource(dx, shadow->GetViewProjectionMatrix());
					}
				}
				void BindConstBuffer(Device::DirectX& dx, uint index, bool bindVS, bool bindGS, bool bindPS)
				{
					auto& cb = _constBuffers[index];

					if (bindVS) Shader::VertexShader::BindConstBuffer(dx,	ConstBufferBindIndex::OnlyPass, *cb);
					if (bindGS) Shader::GeometryShader::BindConstBuffer(dx, ConstBufferBindIndex::OnlyPass, *cb);
					if (bindPS) Shader::PixelShader::BindConstBuffer(dx,	ConstBufferBindIndex::OnlyPass, *cb);
				}
				void UnbindConstBuffer(Device::DirectX& dx, bool bindVS, bool bindGS, bool bindPS)
				{
					if (bindVS) Shader::VertexShader::UnBindConstBuffer(dx,		ConstBufferBindIndex::OnlyPass);
					if (bindGS) Shader::GeometryShader::UnBindConstBuffer(dx,	ConstBufferBindIndex::OnlyPass);
					if (bindPS) Shader::PixelShader::UnBindConstBuffer(dx,		ConstBufferBindIndex::OnlyPass);
				}

			private:
				std::vector<ConstBufferType>	_preparedConstBuffers;
				std::vector<ConstBufferType>	_constBuffers;
			};
		}
	}
}
