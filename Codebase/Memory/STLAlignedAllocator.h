#pragma once

#include <type_traits>
#include <memory>


template<typename T, unsigned _Alignment>
///	@ingroup Memory
///  <summary>
///	Stateless aligned allocator to use with STL collections like std::vector.
/// </summary>
/// <example>
/// std::vector<int, STLAlignedAllocator<int, 16>> vec;
/// </example>
class STLAlignedAllocator
{
public:
	typedef STLAlignedAllocator<T, _Alignment> other;
	typedef T value_type;

	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef void* void_pointer;
	typedef const void* const_void_pointer;

	typedef value_type& reference;
	typedef const value_type& const_reference;

	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef std::false_type propagate_on_container_copy_assignment;
	typedef std::false_type propagate_on_container_move_assignment;
	typedef std::false_type propagate_on_container_swap;

	template<typename _Other>
	struct rebind
	{
		// convert this type to allocator<_Other>
		typedef STLAlignedAllocator<_Other, _Alignment> other;
	};

	pointer address(reference _Val) const _NOEXCEPT
	{
		// return address of mutable _Val
		return (_STD addressof(_Val));
	}

	const_pointer address(const_reference _Val) const _NOEXCEPT
	{
		// return address of nonmutable _Val
		return (_STD addressof(_Val));
	}

	void deallocate(pointer p, size_type n)
	{
		_aligned_free(p);
		// deallocate object at _Ptr, ignore size
	}

	pointer allocate(size_type n)
	{
		return reinterpret_cast<T*>(_aligned_malloc(sizeof(T) * n, _Alignment));
	}

	pointer allocate(size_type _Count, const void*)
	{
		// allocate array of _Count elements, ignore hint
		return (allocate(_Count));
	}

	void construct(pointer _Ptr)
	{
		// default construct object at _Ptr
		::new (_Ptr) T();
	}

	void construct(pointer _Ptr, const_reference _Val)
	{
		// construct object at _Ptr with value _Val
		::new (_Ptr) T(_Val);
	}

	template<class _Objty, class... _Types>
	void construct(_Objty* _Ptr, _Types&& ... _Args)
	{
		// construct _Objty(_Types...) at _Ptr
		::new (_Ptr) _Objty(_STD forward<_Types>(_Args)...);
	}


	template<class _Uty>
	void destroy(_Uty* _Ptr)
	{
		// destroy object at _Ptr
		_Ptr->~_Uty();
	}

	size_t max_size() const _THROW0()
	{
		// estimate maximum array size
		return ((size_t)(-1) / sizeof(T));
	}
};

template <typename T, typename U, unsigned _Alignment>
inline bool operator == (const STLAlignedAllocator<T, _Alignment>&, const STLAlignedAllocator<U, _Alignment>&)
{
	return true;
}

template <typename T, typename U, unsigned _Alignment>
inline bool operator != (const STLAlignedAllocator<T, _Alignment>&, const STLAlignedAllocator<U, _Alignment>&)
{
	return false;
}
