#pragma once

#include <vector>
#include <array>

namespace Core
{
	template<typename T, uint size>
	class MultiLevelVector
	{
	private:
		std::array<std::vector<T>, size> _vectorArray;

	public:
		MultiLevelVector() : _vectorArray() {}	
		~MultiLevelVector() {}

	public:
		template<uint level>
		uint PushBack(const T& object)
		{
			static_assert(level < size, "Error, Out of Range!");

			auto& vector = _vectorArray[level];
			vector.push_back(object);
			return vector.size(); 
		}

		template<uint level>	
		void Insert(const T& object, uint index)
		{
			static_assert(level < size, "Error, Out of Range!");

			auto& vector = _vectorArray[level];
			vector.insert( vector.begin() + index, object );
		}

		template<uint fromLevel, uint toLevel>
		void ChangeStage(uint fromIndex)
		{
			static_assert(fromLevel < size, "Error, Out of Range!");
			static_assert(toLevel < size, "Error, Out of Range!");        

			auto& fromVector	= _vectorArray[fromLevel];
			auto& toVector		= _vectorArray[toLevel];

			toVector.push_back( std::move(fromVector[fromIndex]) );
			fromVector.erase( fromVector.begin() + fromIndex );
		}

		template<uint fromLevel, uint toLevel>	
		void ChangeStage(uint fromIndex, uint toIndex)
		{
			static_assert(fromLevel < size, "Error, Out of Range!");
			static_assert(toLevel < size, "Error, Out of Range!");        

			auto& fromVector	= _vectorArray[fromLevel];
			auto& toVector		= _vectorArray[toLevel];

			toVector.insert( toVector.begin() + toIndex, std::move(fromVector[fromIndex]) );
			fromVector.erase( fromVector.begin() + fromIndex );
		}

		template<uint level>
		void Delete(uint index)
		{
			static_assert(level < size, "Error, Out of Range!");

			auto& vector = _vectorArray[level];
			vector.erase(vector.begin() + index);
		}

		template<uint level>	
		void DeleteAll()
		{
			static_assert(level < size, "Error, Out of Range!");        
			_vectorArray[level].clear();
		}

		template<uint level>		
		inline const T& Get(uint index) const
		{
			static_assert(level < size, "Error, Out of Range!");
			return _vectorArray[level][index];
		}

		template<uint level>			
		inline T& Get(uint index)
		{
			return const_cast<T&>(
				static_cast<const MultiLevelVector<T, size>>(*this).Get<level>(index)
				);
		}

		template<uint level>
		inline size_t GetSize() const
		{
			static_assert(level < size, "Error, Out of Range!");        
			return _vectorArray[level].size();
		}

		static const uint MaxLevel = size;
	};
}