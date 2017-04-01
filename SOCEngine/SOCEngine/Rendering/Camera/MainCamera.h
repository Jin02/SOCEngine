#pragma once

#include "ComputeShader.h"
#include "MeshCamera.h"
#include "ShaderManager.h"

namespace Rendering
{
	namespace Camera
	{
		class MainCamera final
		{
		public:
			struct TBRParam
			{	
				Math::Matrix 		invProjMat;
				Math::Matrix 		invViewProjMat;
				Math::Matrix 		invViewProjViewport;
				
				struct Packed
				{
					uint		packedViewportSize		= 0;
					uint 		packedNumOfLights		= 0;
					uint 		maxNumOfperLightInTile	= 0;

					bool operator == (const Packed& a)
					{
						return (packedViewportSize == a.packedViewportSize) && (packedNumOfLights == a.packedNumOfLights) && (maxNumOfperLightInTile == a.maxNumOfperLightInTile);
					}
					bool operator != (const Packed& a)
					{
						return !( (*this) == a );
					}
				};
				
				Packed			packedParam;
				float			gamma					= 2.2f;
			};

		DISALLOW_ASSIGN_COPY(MainCamera);

		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr);
			void Render(Device::DirectX& dx);

			const Size<uint> ComputeThreadGroupSize(const Size<uint>& size) const;
			uint CalcMaxNumLightsInTile(const Size<uint>& size) const;

		private:
			Shader::ComputeShader	_tbrShader;
			MeshCamera				_meshCam;
		};
	}
}

/*
FIX
카메라 관련 설계가 모두 바뀌어야 해
메인캠은 컴포넌트이되 한개만 존재해야해.
현재 매쉬캠이 다중으로 존재할 수 있게 해뒀지만 이래선 안되
역시 최종으로 할게 카메라이긴 하네.
LM이나 해주든가 아니면 SHADOW나 가든가..?
정리는 5시 30분정도부터.
SHADOW나 정리해보자. ㅇㅇ;
*/