#include "LightForm.h"
#include "Object.h"

namespace Rendering
{
	namespace Light
	{
		LightForm::LightForm( )
		{
			range = 10.0f;
			specularPower = 20.0f;

			ambient = Color(0.1, 0.1, 0.1);
			diffuse = Color::white();
			specular = Color::white();
		}

		Vector3& LightForm::GetWorldPosition()
		{
			return ownerTransform->GetWorldPosition();
		}

		Vector3& LightForm::GetDirection()
		{
			return ownerTransform->GetForward();
		}

		LightForm::LightType LightForm::GetType()
		{
			return type;
		}

		void LightForm::Initialize()
		{

		}

		void LightForm::Destroy()
		{

		}
	}
}