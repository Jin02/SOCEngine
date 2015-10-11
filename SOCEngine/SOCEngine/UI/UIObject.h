#pragma once

#include "Common.h"
#include "Object.h"
#include "ConstBuffer.h"

//#include "DirectX.h"

#include <string>
#include <functional>

namespace UI
{
	class UIObject : protected Core::Object
	{
	protected:
		Rendering::Buffer::ConstBuffer*		_transformCB;

	protected:
		int _depth;

	public:
		UIObject(const std::string& name, Object* parent = NULL);
		virtual ~UIObject();

	protected:
		void InitConstBuffer();

	public:
		void Add(const std::string& key, UIObject* object);

		UIObject* Find(const std::string& key);
		UIObject* Get(uint index);

		void Delete(const std::string& key, bool dealloc = false);
		void DeleteAll(bool dealloc = false);

		void Update(float delta);
		void UpdateTransform(ID3D11DeviceContext* context, const Math::Matrix& viewProj);

	public:
		virtual void Render(ID3D11DeviceContext* context, const Math::Matrix& viewProjMat){};

	public:
		GET_ACCESSOR(Depth, int, _depth);
		GET_ACCESSOR(Name, const std::string&, _name);
		GET_ACCESSOR(Use, bool, _use);
	};
}