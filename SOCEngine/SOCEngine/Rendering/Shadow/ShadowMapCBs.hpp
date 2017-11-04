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
			class ShadowMapCBs
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

				const auto& Get(uint index) const	{ return _constBuffers[index]; }
				auto& Get(uint index)				{ return _constBuffers[index]; }

			private:
				std::vector<ConstBufferType>	_preparedConstBuffers;
				std::vector<ConstBufferType>	_constBuffers;
			};
		}
	}
}
