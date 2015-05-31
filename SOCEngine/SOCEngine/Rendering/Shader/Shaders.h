#pragma once

#include "VertexShader.h"
#include "PixelShader.h"
#include "GeometryShader.h"
#include "HullShader.h"

namespace Rendering
{
	namespace Shader
	{
		enum class RenderShaderType : unsigned int
		{
			RenderScene,
			DepthWrite,
			AlphaTestWithDiffuse,
			Num
		};
		struct RenderShaders
		{
			VertexShader*		vs;
			PixelShader*		ps;
			GeometryShader*		gs;
			HullShader*			hs;

			RenderShaders(VertexShader* _vs = nullptr, PixelShader* _ps = nullptr, GeometryShader* _gs = nullptr, HullShader* _hs = nullptr)
				: vs(_vs), ps(_ps), gs(_gs), hs(_hs) {}
			~RenderShaders() {}

			const bool ableRender() const
			{
				return (vs != nullptr) && (ps != nullptr);
			}
		};
	}
}