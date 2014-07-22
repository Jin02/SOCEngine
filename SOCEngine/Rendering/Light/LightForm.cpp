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

		LightForm::~LightForm()
		{
		}

		void LightForm::Initialize()
		{
			//null
		}

		void LightForm::Destroy()
		{
			//null
		}
	}
}