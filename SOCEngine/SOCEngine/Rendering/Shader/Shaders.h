#pragma once

#include "VertexShader.h"
#include "PixelShader.h"
#include "GeometryShader.h"

#include <assert.h>

namespace Rendering
{
	namespace Shader
	{
		enum class ShaderUsage : uint
		{
			RenderScene,
			DepthWrite,
			AlphaTestWithDiffuse,
			Num
		};

		struct ShaderGroup
		{
			std::shared_ptr<VertexShader>		vs;
			std::shared_ptr<PixelShader>		ps;
			std::shared_ptr<GeometryShader>		gs;

			ShaderGroup() : vs(nullptr), ps(nullptr), gs(nullptr)
			{

			}
			ShaderGroup(const std::shared_ptr<VertexShader>& _vs,
						const std::shared_ptr<PixelShader>& _ps,
						const std::shared_ptr<GeometryShader>& _gs)
				: vs(_vs), ps(_ps), gs(_gs)
			{

			}

			const bool ableRender() const
			{
				return (vs != nullptr) && (ps != nullptr);
			}
			const bool IsAllEmpty() const
			{
				return (!vs && !ps && !gs);
			}
		};

		template <typename Resource>
		class BindShaderData
		{
		public:
			BindShaderData() = default;
			BindShaderData(const Resource& _resource, uint _bindIndex, bool _useVS, bool _useGS, bool _usePS, bool _useCS)
				: resource(_resource), useVS(_useVS), useGS(_useGS), usePS(_usePS), useCS(_useCS)
			{

			}
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