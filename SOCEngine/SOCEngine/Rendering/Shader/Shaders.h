#pragma once

#include "VertexShader.h"
#include "PixelShader.h"
#include "GeometryShader.h"
#include "HullShader.h"

#include <assert.h>

namespace Rendering
{
	namespace Shader
	{
		enum class ShaderUsage : unsigned int
		{
			RenderScene,
			DepthWrite,
			AlphaTestWithDiffuse,
			Num
		};

		struct ShaderGroup
		{
			VertexShader*		vs;
			PixelShader*		ps;
			GeometryShader*		gs;
			HullShader*			hs;

			ShaderGroup() : vs(nullptr), ps(nullptr), gs(nullptr), hs(nullptr) {}
			ShaderGroup(VertexShader* _vs, PixelShader* _ps, GeometryShader* _gs, HullShader* _hs) : vs(_vs), ps(_ps), gs(_gs), hs(_hs) {}
			~ShaderGroup() {}

			const bool ableRender() const
			{
				return (vs != nullptr) && (ps != nullptr);
			}
			const bool IsAllEmpty() const
			{
				return (!vs && !ps && !gs && !hs);
			}
		};
	}
}