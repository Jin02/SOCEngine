#pragma once

#include "VertexShader.h"
#include "PixelShader.h"
#include "GeometryShader.h"

#include <assert.h>

namespace Rendering
{
	namespace Shader
	{
		struct ShaderGroup
		{
			std::shared_ptr<VertexShader>		vs = nullptr;
			std::shared_ptr<PixelShader>		ps = nullptr;
			std::shared_ptr<GeometryShader>		gs = nullptr;

			ShaderGroup() = default;
			ShaderGroup(const std::shared_ptr<VertexShader>& _vs,
				    const std::shared_ptr<PixelShader>& _ps,
				    const std::shared_ptr<GeometryShader>& _gs)
				: vs(_vs), ps(_ps), gs(_gs) { }

			inline bool ableRender() const { return (vs != nullptr) & (ps != nullptr); }
			inline bool IsAllEmpty() const { return (!vs & !ps & !gs); }
		};

		template <typename Resource>
		class BindShaderData
		{
		public:
			BindShaderData() = default;
			BindShaderData(const Resource& _resource, uint _bindIndex, bool _useVS, bool _useGS, bool _usePS, bool _useCS)
				: resource(_resource), useVS(_useVS), useGS(_useGS), usePS(_usePS), useCS(_useCS) { }
		public:
			Resource resource;

			uint bindIndex = 0;
			bool useVS = false;
			bool useGS = false;
			bool usePS = false;
			bool useCS = false;
		};
	}
}
