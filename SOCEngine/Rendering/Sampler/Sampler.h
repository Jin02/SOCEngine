#pragma once

#include "DirectX.h"

namespace Rendering
{
	class Sampler
	{
	private:
		ID3D11SamplerState* _sampler;

	public:
		Sampler();
		~Sampler();

	public:
		void Create();
		void Destroy();
		
	public:
		GET_ACCESSOR(Sampler, ID3D11SamplerState*, _sampler);
	};
}