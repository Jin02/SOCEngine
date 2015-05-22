#pragma once

#include "Mesh.h"
#include "UIObject.h"

namespace UI
{
	class SimpleImage2D : public UIObject
	{
	private:
		Rendering::Mesh::MeshFilter*		_meshFilter;
		Rendering::Material*				_material;
		
		bool								_isOtherMaterial;


	public:
		SimpleImage2D(const std::string& name, const Core::Object* parent = nullptr);
		virtual ~SimpleImage2D();

	public:
		void Create(Rendering::Material* material = nullptr);
		void UpdateMainImage(Rendering::Texture::Texture* tex);

	public:
		virtual void Render(const Math::Matrix& viewProjMat);

	public:
		GET_ACCESSOR(Material, Rendering::Material*, _material);
	};
}
