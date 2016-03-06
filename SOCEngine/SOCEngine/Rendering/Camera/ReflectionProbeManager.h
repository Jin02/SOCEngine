#pragma once

#include "Structure.h"
#include "ReflectionProbe.h"

namespace Rendering
{
	namespace Manager
	{
		class ReflectionProbeManager : private Structure::VectorMap<address, Camera::ReflectionProbe*>
		{
		public:
			ReflectionProbeManager(void);
			~ReflectionProbeManager(void);

		public:
			void Add(Camera::ReflectionProbe* camera);
			void Delete(Camera::ReflectionProbe* camera);
			void DeleteAll();
			void Destroy();

		public:
			GET_ACCESSOR(ReflectionProbeVector, const std::vector<Camera::ReflectionProbe*>&, _vector);
		}; 
	}
}