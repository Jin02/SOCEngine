#pragma once

#include "EngineMath.h"

namespace Core
{
	struct TransformParameters
	{
	public:
		const Math::Matrix* worldMatrix;
		const Math::Matrix* viewMatrix;
		const Math::Matrix* projMatrix;

		const Math::Matrix* viewProjMatrix;
		const Math::Matrix* worldViewProjMatrix;

		const Math::Matrix* worldViewInvTns;

	public:
		TransformParameters();
		TransformParameters(const Math::Matrix* worldMat, const Math::Matrix* viewMat,
				 const Math::Matrix* projMat,  const Math::Matrix* viewProjMat,
				 const Math::Matrix* worldViewProjMat,
				 const Math::Matrix* worldViewInvTns);
		~TransformParameters();

	public:
		void Set(const Math::Matrix* worldMat, const Math::Matrix* viewMat,
				 const Math::Matrix* projMat,  const Math::Matrix* viewProjMat,
				 const Math::Matrix* worldViewProjMat,
				 const Math::Matrix* worldViewInvTns);
	};
}