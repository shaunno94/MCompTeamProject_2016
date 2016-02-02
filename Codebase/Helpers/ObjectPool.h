#pragma once

#include <stack>
#include <limits>
#include <cassert>

//If I need a more complex pool (with custom allocator), should check http://www.codeproject.com/Articles/746630/O-Object-Pool-in-Cplusplus
template<typename T>
class ObjectPool
{
private:
	std::stack<T*> m_Pool;

	const size_t m_MaxCapacity;

public:
	ObjectPool(size_t maxCapacity = UINT_MAX): m_MaxCapacity(maxCapacity)
	{
		assert(("ObjectPool maxCapacity can not be less than 1", maxCapacity >= 1));
	}

	~ObjectPool()
	{
		while (!m_Pool.empty())
		{
			delete m_Pool.top();
			m_Pool.pop();
		}
	}

	T* New()
	{
		T* result;
		if (!m_Pool.empty())
		{
			result = m_Pool.top();
			m_Pool.pop();
		}
		else
			result = new T();

		return result;
	}

	void Delete(T* obj)
	{
		if (m_Pool.size() < m_MaxCapacity)
		{
			obj->Delete();
			m_Pool.push(obj);
		}
		else
			delete obj;
	}
};
