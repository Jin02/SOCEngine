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
			class ShadowMapCBPool
			{
			public:
				using ConstBufferType = Rendering::Buffer::ExplicitConstBuffer<Math::Matrix>;

			public:
				void PushConstBufferToQueue();

				void InitializePreparedCB(Device::DirectX& dx);

				void Delete(uint shadowIndex);
				
				void DeleteAll();
				
				template <class ShadowType>
				void UpdateSubResource(Device::DirectX& dx,
					const std::vector<ShadowType*>& dirtyShadows,
					const ShadowDatasIndexer& indexer)
				{
					for (auto shadow : dirtyShadows)
					{
						Core::ObjectID objID	= shadow->GetObjectID();
						uint shadowIndex		= indexer.Find(objID.Literal());

						_constBuffers[shadowIndex].UpdateSubResource(dx, shadow->GetViewProjectionMatrix());
					}
				}

				const auto& Get(uint shadowIndex) const	{ return _constBuffers[shadowIndex]; }
				auto& Get(uint shadowIndex)				{ return _constBuffers[shadowIndex]; }

			private:
				std::vector<ConstBufferType>	_preparedConstBuffers;
				std::vector<ConstBufferType>	_constBuffers;
			};
		}
	}
}
