#pragma once

#include "Sphere.h"
#include "Frustum.h"

#include "MeshManager.hpp"
#include "ObjectManager.h"

namespace Rendering
{
	namespace Geometry
	{
		class MeshUtility final
		{
		public:
			static void Culling(const Intersection::Frustum& frustum, Manager::MeshManager& meshMgr, const Core::TransformPool& transformPool);
		};
	}
}