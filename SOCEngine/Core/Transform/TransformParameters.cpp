#include "TransformParameters.h"

namespace Core
{

	TransformParameters::TransformParameters()
		:	worldMatrix(nullptr), viewMatrix(nullptr), projMatrix(nullptr),
			viewProjMatrix(nullptr), worldViewProjMatrix(nullptr), worldViewInvTns(nullptr)
	{

	}

	TransformParameters::TransformParameters(
								  const Math::Matrix* worldMat,
								  const Math::Matrix* viewMat,
								  const Math::Matrix* projMat,
								  const Math::Matrix* viewProjMat,
								  const Math::Matrix* worldViewProjMat,
								  const Math::Matrix* worldViewInvTns)
	{
		this->worldMatrix = worldMat;
		this->viewMatrix = viewMat;
		this->projMatrix = projMat;
		this->viewProjMatrix = viewProjMat;
		this->worldViewProjMatrix = worldViewProjMat;
		this->worldViewInvTns = worldViewInvTns;
	}

	TransformParameters::~TransformParameters()
	{

	}

	void TransformParameters::Set(const Math::Matrix* worldMat,
								  const Math::Matrix* viewMat,
								  const Math::Matrix* projMat,
								  const Math::Matrix* viewProjMat,
								  const Math::Matrix* worldViewProjMat,
								  const Math::Matrix* worldViewInvTns)
	{
		this->worldMatrix = worldMat;
		this->viewMatrix = viewMat;
		this->projMatrix = projMat;
		this->viewProjMatrix = viewProjMat;
		this->worldViewProjMatrix = worldViewProjMat;
		this->worldViewInvTns = worldViewInvTns;
	}

}