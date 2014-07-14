#pragma once

#include "EngineMath.h"

namespace Rendering
{
	struct TransformParameters
	{
	public:
		const Math::Matrix *_worldMatrix;
		const Math::Matrix *_viewMatrix;
		const Math::Matrix *_projMatrix;

		const Math::Matrix *_viewProjMatrix;
		const Math::Matrix *_worldViewProjMatrix;

		const Math::Matrix *_worldViewInvTns;

	public:
		TransformParameters();
		~TransformParameters();

	public:
		void Set(const Math::Matrix* worldMat, const Math::Matrix* viewMat,
				 const Math::Matrix* projMat,  const Math::Matrix* viewProjMat,
				 const Math::Matrix* worldViewProjMat,
				 const Math::Matrix* worldViewInvTns);
	};
}