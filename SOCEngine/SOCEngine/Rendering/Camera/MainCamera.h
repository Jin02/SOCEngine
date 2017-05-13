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