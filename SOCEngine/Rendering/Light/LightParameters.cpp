//#include "LightParameters.h"
//
//namespace Rendering
//{
//	namespace Light
//	{
//		LightParameters::LightParameters()
//			:ambient(nullptr), diffuse(nullptr), specular(nullptr),
//			range(0), lightPos(nullptr), lightDir(nullptr),
//			spotAngle(0), type(0)
//		{
//
//		}
//
//		LightParameters::~LightParameters()
//		{
//		}
//
//		LightParameters::LightParameters(const Math::Vector3* ambient,  const Math::Vector3* diffuse,
//			const Math::Vector3* specular, float range,
//			const Math::Vector3* lightPos, const Math::Vector3* lightdir,
//			float spotAngle, const int type)
//		{
//			this->ambient = ambient;
//			this->diffuse = diffuse;
//			this->specular = specular;
//			this->range = range;
//			this->lightPos = lightPos;
//			this->lightDir = lightdir;
//			this->spotAngle = spotAngle;
//			this->type = type;
//		}
//
//		void LightParameters::SetData(
//			const Math::Vector3* ambient,  const Math::Vector3* diffuse,
//			const Math::Vector3* specular, float range,
//			const Math::Vector3* lightPos, const Math::Vector3* lightdir,
//			float spotAngle, const int type)
//		{
//			this->ambient = ambient;
//			this->diffuse = diffuse;
//			this->specular = specular;
//			this->range = range;
//			this->lightPos = lightPos;
//			this->lightDir = lightdir;
//			this->spotAngle = spotAngle;
//			this->type = type;
//		}
//
//	}
//}