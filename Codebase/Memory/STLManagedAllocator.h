#pragma once

#include <type_traits>
#include <memory>
#include "FixedSizeAllocatorManager.h"


/// <summary>
/// 
/// </summary>
/// <example>
/// std::vector<int, STLAllocator<int, 16>> vec;
/// </example>
template<typename T, unsigned _Alignment>
class STLManagedAllocator
{
private:
    unsigned* m_InstanceCount;
    FixedSizeAllocatorManager* m_allocManager;

    // generic allocator for objects of class _Ty
public:
    inline FixedSizeAllocatorManager* getAllocationManager() const
    {
        return m_allocManager;
	}
	inline unsigned* getGetInstanceCount() const
	{
		return m_InstanceCount;
	}

    typedef STLManagedAllocator<T, _Alignment> other;
    typedef T value_type;

    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef void* void_pointer;
    typedef const void* const_void_pointer;

    typedef value_type& reference;
    typedef const value_type& const_reference;

    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    typedef std::true_type propagate_on_container_copy_assignment;
    typedef std::true_type propagate_on_container_move_assignment;
    typedef std::true_type propagate_on_container_swap;

    STLManagedAllocator<T, _Alignment> select_on_container_copy_construction() const
    {
        // return this allocator
        return (*this);
    }

    template<typename _Other>
    struct rebind {
        // convert this type to allocator<_Other>
        typedef STLManagedAllocator<_Other, _Alignment> other;
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

    STLManagedAllocator() _THROW0()
    {
		m_InstanceCount = new unsigned(1);
        m_allocManager = new FixedSizeAllocatorManager(FixedSizeAllocator::DEFAULT_PAGE_SIZE, _Alignment);
    }

    STLManagedAllocator(const STLManagedAllocator<T, _Alignment>& other) _THROW0()
    {
		m_InstanceCount = other.m_InstanceCount;
        m_allocManager = other.m_allocManager;
		++(*m_InstanceCount);
    }

    template<class _Other>
    STLManagedAllocator(const STLManagedAllocator<_Other, _Alignment>& other) _THROW0()
    {
		m_InstanceCount = other.getGetInstanceCount();
        m_allocManager = other.getAllocationManager();
		++(*m_InstanceCount);
    }

    STLManagedAllocator(STLManagedAllocator<T, _Alignment>&& other) _THROW0()
	{
		m_InstanceCount = other.m_InstanceCount;
		m_allocManager = other.m_allocManager;
		++(*m_InstanceCount);
    }

    ~STLManagedAllocator() _THROW0()
    {
		if (--(*m_InstanceCount) == 0 && m_allocManager)
            delete m_allocManager;
        m_allocManager = nullptr;
    }

    template<class _Other>
    STLManagedAllocator<T, _Alignment>& operator=(const STLManagedAllocator<_Other, _Alignment>&)
    {
        // assign from a related allocator (do nothing)
        return (*this);
    }

    template<class _Other>
    STLManagedAllocator<T, _Alignment>& operator=(const STLManagedAllocator<_Other, _Alignment>&& other)
    {
        return (*this);
    }

    void deallocate(pointer p, size_type n)
    {
        m_allocManager->Free(p, sizeof(T) * n);
        // deallocate object at _Ptr, ignore size
    }

    pointer allocate(size_type n)
    {
        return reinterpret_cast<T*>(m_allocManager->Allocate(sizeof(T) * n));
    }

    pointer allocate(size_type _Count, const void*)
    {
        // allocate array of _Count elements, ignore hint
        return (allocate(_Count));
    }

    void construct(pointer _Ptr)
    {
        // default construct object at _Ptr
        ::new ((void*)_Ptr) T();
    }

    void construct(pointer _Ptr, const_reference _Val)
    {
        // construct object at _Ptr with value _Val
        ::new ((void*)_Ptr) T(_Val);
    }

    template<class _Objty, class... _Types>
    void construct(_Objty* _Ptr, _Types&& ... _Args)
    {
        // construct _Objty(_Types...) at _Ptr
        ::new ((void*)_Ptr) _Objty(_STD forward<_Types>(_Args)...);
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
inline bool operator == (const STLManagedAllocator<T, _Alignment>&, const STLManagedAllocator<U, _Alignment>&)
{
    return true;
}

template <typename T, typename U, unsigned _Alignment>
inline bool operator != (const STLManagedAllocator<T, _Alignment>&, const STLManagedAllocator<U, _Alignment>&)
{
    return false;
}
