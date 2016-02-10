#pragma once

/** @ingroup Memory
*  @{
*/

template<unsigned _PaddingSize>
struct AlignmentWrapperBase {
	char padding[_PaddingSize];
};
template<>
struct AlignmentWrapperBase<0> {
};

template<typename T, unsigned _Alignment>
class AlignmentWrapper : public AlignmentWrapperBase<(_Alignment - (sizeof(T) % _Alignment)) % _Alignment>
{
public:

	AlignmentWrapper(const T& val) : value(val) {}
	AlignmentWrapper(T&& val) : value(val) {}
	AlignmentWrapper(const AlignmentWrapper<T, _Alignment>& val) : value(val.value) {}

    T value;
};


/** @} */