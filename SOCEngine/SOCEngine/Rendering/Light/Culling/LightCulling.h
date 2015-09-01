#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "EngineMath.h"
#include "RenderTexture.h"
#include <array>

#include "Lightform.h"
#include "LightManager.h"

#include "RenderingCommon.h"

namespace Rendering
{
	namespace Light
	{
		class LightCulling
		{
		public:
			static const uint TileSize					= 16;
			static const uint LightMaxNumInTile			= 272;

			enum class InputBufferShaderIndex : unsigned int
			{
				PointLightRadiusWithCenter		= 0,
				SpotLightRadiusWithCenter		= 2,
				SpotLightParam					= 4
			};

			enum class InputTextureShaderIndex : unsigned int
			{
				InvetedOpaqueDepthBuffer		= 11,
				InvetedBlendedDepthBuffer		= 12
			};

			enum class ConstBufferShaderIndex : unsigned int
			{
				TBRParam = 1
			};

		public:
			struct TBRChangeableParam
			{
				Math::Matrix		viewMat;
				Math::Matrix 		invProjMat;
				unsigned int 		lightNum;
			};

			struct TBRParam : TBRChangeableParam
			{	
				Math::Matrix 		invViewProjViewport;

				Math::Size<float>	screenSize;
				unsigned int 		maxNumOfperLightInTile;
			};

		private:
			std::vector<GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer>		_inputBuffers;
			std::vector<GPGPU::DirectCompute::ComputeShader::InputTexture>					_inputTextures;
			std::vector<GPGPU::DirectCompute::ComputeShader::Output>						_outputs;

			GPGPU::DirectCompute::ComputeShader*								_computeShader;

			bool																_useBlendedMeshCulling;

			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*		_inputPointLightTransformBuffer;
			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*		_inputSpotLightTransformBuffer;
			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*		_inputSpotLightParamBuffer;

		public:
			LightCulling();
			virtual ~LightCulling();

		protected:
			void Initialize(const std::string& filePath, const std::string& mainFunc,
				const Texture::DepthBuffer* opaqueDepthBuffer, const Texture::DepthBuffer* blendedDepthBuffer,
				RenderType renderType,
				const std::vector<Shader::ShaderMacro>* opationalMacros = nullptr);

			void SetInputsToCS();

			void UpdateTBRCommonParam(const Device::DirectX* dx, const TBRParam* param);
			void UpdateThreadGroup(GPGPU::DirectCompute::ComputeShader::ThreadGroup* outThreadGroup, bool updateComputeShader = true);

			void Destroy();

			inline void SetOuputBuferToCS(const std::vector<GPGPU::DirectCompute::ComputeShader::Output>&  outputs) { _computeShader->SetOutputs(outputs); }

		protected:
			void AddInputBufferToList(GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*& outBuffer, uint idx, const Buffer::ShaderResourceBuffer*& buffer);
			void _Set_InputTexture_And_Append_To_InputTextureList(GPGPU::DirectCompute::ComputeShader::InputTexture** outTexture,	uint idx, const Texture::Texture2D* texture);

		public:	
			void Dispatch(const Device::DirectX* dx, const Buffer::ConstBuffer* tbrConstBuffer);

		public:
			const Math::Size<unsigned int> CalcThreadGroupSize() const;
			static unsigned int CalcMaxNumLightsInTile();
		};
	}
}