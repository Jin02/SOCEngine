#pragma once

#include <d3dcommon.h>

namespace Rendering
{
	namespace RenderState
	{
		enum class RasterizerState : unsigned int
		{
			CCWDisableCulling			= 0,
			CCWDefault,
			CWDisableCulling,
			CWDisableCullingWithClip,
			CWDefault,
			MAX
		};
		constexpr unsigned int MaxCountRasterizerState = static_cast<unsigned int>(RasterizerState::MAX);

		enum class BlendState : unsigned int
		{
			Opaque				= 0,
			AlphaToCoverage,
			Alpha,
			MAX
		};
		constexpr unsigned int MaxCountBlendState = static_cast<unsigned int>(BlendState::MAX);

		enum class DepthState : unsigned int
		{
			DisableDepthWrite					= 0,
			DisableDepthTest,
			Less,
			EqualAndDisableDepthWrite,
			Greater,
			GreaterAndDisableDepthWrite,	
			GreaterEqualAndDisableDepthWrite,
			LessEqual,
			MAX
		};
		constexpr unsigned int MaxCountDepthStencilState = static_cast<unsigned int>(DepthState::MAX);

		enum class SamplerState : unsigned int
		{
			Anisotropic			= 0,
			Linear,
			Point,
			ShadowLessEqualComp,
			ShadowGreaterEqualComp,
			ShadowLinear,
			ConeTracing,
			MAX
		};
		constexpr unsigned int MaxCountSamplerState = static_cast<unsigned int>(SamplerState::MAX);

		enum class PrimitiveTopology : unsigned int
		{
			TriangleList	= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			TriangleStrip	= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
		};
	}
}