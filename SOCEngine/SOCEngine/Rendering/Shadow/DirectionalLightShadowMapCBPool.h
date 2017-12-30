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
			class DirectionalLightShadowMapCBPool
			{
			public:
				using ConstBufferType = Rendering::Buffer::ExplicitConstBuffer<Math::Matrix>;
				struct ConstBuffers
				{
					ConstBufferType viewProjMatCB;
					ConstBufferType viewMatCB;
				};

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

						_constBuffers[shadowIndex].viewProjMatCB.UpdateSubResource(dx, shadow->GetTransposedVPMat());
						_constBuffers[shadowIndex].viewMatCB.UpdateSubResource(dx, shadow->GetTransposedViewMat());
					}
				}

				const auto& Get(uint shadowIndex) const	{ return _constBuffers[shadowIndex]; }
				auto& Get(uint shadowIndex)				{ return _constBuffers[shadowIndex]; }

			private:
				std::vector<ConstBuffers>	_preparedConstBuffers;
				std::vector<ConstBuffers>	_constBuffers;
			};
		}
	}
}
