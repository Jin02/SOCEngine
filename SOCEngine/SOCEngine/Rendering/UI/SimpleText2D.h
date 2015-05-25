#pragma once

#include "Mesh.h"
#include "UIObject.h"

#define BASIC_FONT_COUNT 96

namespace UI
{
	class SimpleText2D : public UIObject
	{
	public:
		struct RectVertexInfo
		{
			Math::Vector3	position;
			Math::Vector2	uv;
		};

	private:
		Rendering::Mesh::MeshFilter*	_meshFilter;
		Rendering::Material*			_material;
		Math::Size<uint>				_screenSize;
		bool							_isOtherMaterial;
		uint							_maxLength;

	public:
		SimpleText2D(const std::string& name, const Core::Object* parent = nullptr);
		virtual ~SimpleText2D();

	public:
		void Initialize(uint maxLength, Rendering::Material* material = nullptr);
		void UpdateText(const std::string& text);

	public:
		virtual void Render(ID3D11DeviceContext* context, const Math::Matrix& viewProjMat);

	public:
		GET_ACCESSOR(Material, Rendering::Material*, _material);
	};
}

