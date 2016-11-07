#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "EngineMath.h"
#include "RenderTexture.h"
#include <array>

#include "Lightform.h"
#include "LightManager.h"

#include "BindIndexInfo.h"
#include "GlobalDefine.h"

namespace Rendering
{
	namespace Light
	{
		class LightCulling
		{
		public:
			struct TBRParam
			{	
				Math::Matrix 		invProjMat;
				Math::Matrix 		invViewProjMat;
				Math::Matrix 		invViewProjViewport;

				uint			packedViewportSize;
				uint 			packedNumOfLights;
				uint 			maxNumOfperLightInTile;
				uint			dummy;

				TBRParam(){}
				~TBRParam(){}
			};

		private:
			std::vector<Shader::ShaderForm::InputShaderResourceBuffer>		_inputBuffers;
			std::vector<Shader::ShaderForm::InputTexture>					_inputTextures;
			std::vector<Shader::ShaderForm::InputUnorderedAccessView>		_outputs;

			GPGPU::DirectCompute::ComputeShader*							_computeShader;

			bool															_useBlendedMeshCulling;

		public:
			LightCulling();
			virtual ~LightCulling();

		protected:
			void Initialize(const std::string& filePath, const std::string& mainFunc,
				const Texture::DepthBuffer* opaqueDepthBuffer, const Texture::DepthBuffer* blendedDepthBuffer,
				const std::vector<Shader::ShaderMacro>* opationalMacros = nullptr);

			void SetInputsToCS();

			void UpdateTBRCommonParam(const Device::DirectX* dx, const TBRParam* param);
			void UpdateThreadGroup(GPGPU::DirectCompute::ComputeShader::ThreadGroup* outThreadGroup, bool updateComputeShader = true);

			void Destroy();

			inline void SetOuputBuferToCS(const std::vector<Shader::ShaderForm::InputUnorderedAccessView>&  outputs) { _computeShader->SetUAVs(outputs); }

		protected:
			void AddInputBufferToList(uint idx, const Buffer::ShaderResourceBuffer* buffer);
			void AddTextureToInputTextureList(uint idx, const Texture::Texture2D* texture);

		public:	
			void Dispatch(const Device::DirectX* dx,
				const Buffer::ConstBuffer* tbrCB, const Buffer::ConstBuffer* mainCamCB,
				const std::vector<Shader::ShaderForm::InputConstBuffer>* additionalConstBuffers = nullptr);

		public:
			const Math::Size<unsigned int> CalcThreadGroupSize() const;
			static unsigned int CalcMaxNumLightsInTile();
		};
	}
}
