#include "TransformParameters.h"

namespace Rendering
{
	TransformParameters::TransformParameters()
		:	_worldMatrix(nullptr), _viewMatrix(nullptr), 
			_projMatrix(nullptr),  _viewProjMatrix(nullptr), 
			_worldViewProjMatrix(nullptr)
	{

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
		_worldMatrix = worldMat;
		_viewMatrix = viewMat;
		_projMatrix = projMat;
		_viewProjMatrix = viewProjMat;
		_worldViewProjMatrix = worldViewProjMat;
		_worldViewInvTns = worldViewInvTns;
	}

}