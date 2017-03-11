#pragma once

#include <memory>
#include <utility>

template <typename Type>
class Container final
{
public:
	explicit Container(const Type& type)
		: _data(std::make_shared<Type>(type)) {}
	explicit Container(Type&& type)
		: _data(std::make_shared<Type>(std::forward<Type>(type))) {}
	explicit Container(const std::shared_ptr<Type>& sharedData)
		: _data(sharedData) {}
	explicit Container(std::shared_ptr<Type>&& sharedData)
		: _data(std::forward<Type>(sharedData)) {}

	inline auto& Get() { return *_data; }

private:
	std::shared_ptr<Type> _data;
};