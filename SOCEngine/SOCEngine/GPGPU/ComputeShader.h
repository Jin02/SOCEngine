#pragma once

#include "BaseShader.h"
#include "CSInputBuffer.h"
#include "CSOutputBuffer_ReadCPU.h"
#include "Texture.h"

namespace GPGPU
{
	namespace DirectCompute
	{
		class ComputeShader : public Rendering::Shader::BaseShader
		{
		public:
			struct InputBuffer
			{
				unsigned int idx;
				CSInputBuffer* buffer;
				InputBuffer() : idx(0), buffer(nullptr){}
				~InputBuffer(){}
			};
			struct InputTexture
			{
				unsigned int idx;
				const Rendering::Texture::Texture* texture;
				InputTexture() : idx(0), texture(nullptr){}
				~InputTexture(){}
			};
			struct OutputBuffer
			{
				unsigned int idx;
				CSOutputBuffer* buffer;
				OutputBuffer() : idx(0), buffer(nullptr){}
				~OutputBuffer(){}
			};
			struct ThreadGroup
			{
				unsigned int x, y, z;
				ThreadGroup() : x(0), y(0), z(0){}
				ThreadGroup(unsigned int _x, unsigned int _y, unsigned _z) : x(_x), y(_y), z(_z){}
				~ThreadGroup(){}
			};

		private:
			std::vector<InputBuffer>	_inputBuffers;
			std::vector<InputTexture>	_inputTextures;

			std::vector<OutputBuffer>	_outputBuffers;

			ID3D11ComputeShader*		_shader;
			ThreadGroup					_threadGroup;

		public:
			ComputeShader(const ThreadGroup& threadGroup, ID3DBlob* blob);
			~ComputeShader(void);

		public:
			bool Create();
			void Dispatch(ID3D11DeviceContext* context);

		public:
			GET_SET_ACCESSOR(ThreadGroupInfo, const ThreadGroup&, _threadGroup);
			GET_SET_ACCESSOR(InputBuffers, const std::vector<InputBuffer>&, _inputBuffers);
			GET_SET_ACCESSOR(InputTextures, const std::vector<InputTexture>&, _inputTextures);

			GET_SET_ACCESSOR(OutputBuffers, const std::vector<OutputBuffer>&, _outputBuffers);
		};
	}
}
