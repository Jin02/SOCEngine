//#pragma once
//
//#include "EngineMath.h"
//
//namespace Rendering
//{
//	namespace Light
//	{
//		struct LightParameters
//		{
//			const Math::Vector3*	ambient;
//			const Math::Vector3*	diffuse;
//			const Math::Vector3*	specular;
//
//			float					range;
//
//			const Math::Vector3*	lightPos;
//			const Math::Vector3*	lightDir;
//
//			float					spotAngle;
//			int						type;
//
//		public:
//			LightParameters();
//			LightParameters(const Math::Vector3* ambient,  const Math::Vector3* diffuse,
//				const Math::Vector3* specular, float range,
//				const Math::Vector3* lightPos, const Math::Vector3* lightdir,
//				float spotAngle, const int type);
//			~LightParameters();
//
//			void SetData(
//				const Math::Vector3* ambient,  const Math::Vector3* diffuse,
//				const Math::Vector3* specular, float range,
//				const Math::Vector3* lightPos, const Math::Vector3* lightdir,
//				float spotAngle, const int type);
//		};
//
//	}
//}