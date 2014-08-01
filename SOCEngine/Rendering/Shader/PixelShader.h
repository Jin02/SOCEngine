#pragma once

#include "Shader.h"

namespace Rendering
{
	namespace Shader
	{
		class PixelShader : public Shader
		{
		private:
			ID3D11PixelShader* _shader;

		public:
			PixelShader(ID3DBlob* blob);
			~PixelShader(void);

		public:
			bool CreateShader();

			virtual void Begin();
			virtual void End();
		};
	}
}
