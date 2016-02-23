#pragma once

/** @ingroup Helpers
*  @{
*/

#include <vector>
#include <algorithm>


template<typename T>
/// <summary>
/// Helper function that swaps erasing element with the back element and then erases the back.
/// </summary>
/// <remarks>
/// This function helps avoid array rearrangements but is only useful for collections where element order does not matter.
/// </remarks>
/// <param name="v">Vector.</param>
/// <param name="index">Index of element to erase.</param>
void SwapErase(std::vector<T>& v, size_t index)
{
	std::swap(v[index], v.back());
	v.pop_back();
}

template<typename T>
/// <summary>
/// Helper function that swaps erasing element with the back element and then erases the back.
/// </summary>
/// <remarks>
/// This function helps avoid array rearrangements but is only useful for collections where element order does not matter.
/// </remarks>
/// <param name="v">Vector.</param>
/// <param name="val">Reference to a vector element.</param>
void SwapErase(std::vector<T>& v, T& val)
{
	std::swap(val, v.back());
	v.pop_back();
}

template<typename T>
/// <summary>
/// Helper function that swaps matched erasing element with the back element and then erases the back.
/// </summary>
/// <remarks>
/// This function helps avoid array rearrangements but is only useful for collections where element order does not matter.
/// </remarks>
/// <param name="v">Vector.</param>
/// <param name="val">Value for matching vector element.</param>
/// <returns>Indicator if a match was found (true if found and false otherwise)</returns>
bool FindSwapErase(std::vector<T>& v, const T& val)
{
	auto it = std::find(v.begin(), v.end(), val);
	if (it != v.end())
	{
		std::swap(*it, v.back());
		v.pop_back();
		return true;
	}
	return false;
}


template <class FwdIt, class Compare>
/// <summary>
/// Insertion sort algorithm.
/// </summary>
/// <param name="start">Start iterator.</param>
/// <param name="end">End iterator.</param>
/// <param name="cmp">Compare object.</param>
void InsertionSort(FwdIt start, FwdIt end, Compare cmp)
{
	if (start != end)
	{
		FwdIt originalStart = start;
		for (++start; start != end; ++start)
		{
			FwdIt current = start;
			FwdIt previous = start - 1;
			while (current != originalStart && cmp(*current, *previous))
			{
				std::iter_swap(current, previous);
				--current;
			}
		}
	}
}

template <class FwdIt>
/// <summary>
/// Insertion sort algorithm.
/// </summary>
/// <param name="start">Start iterator.</param>
/// <param name="end">End iterator.</param>
void InsertionSort(FwdIt start, FwdIt end)
{
	InsertionSort(start, end, std::less<typename std::iterator_traits<FwdIt>::value_type>());
}


/** @} */