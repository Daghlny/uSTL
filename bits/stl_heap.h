
#pragma once

#include "stl_iterator.h"
#include "algorithm.tcc"

namespace ustl {

/*
 * @_first the head pointer of Array to validate a heap
 * @_holeindex the index of value pushed
 * @_topindex  the heap top index in @_first array
 * @_value     the value pushed
 * @_cmp       the compare function
 */
template<class RandomIt, class Distance, class T, class Compare>
void __push_heap(RandomIt _first, Distance _holeindex, Distance _topindex, T _value, Compare _cmp)
{
    Distance parent_index = (_holeindex-1)/2;
    while (_holeindex > _topindex && _cmp(*(_first+parent_index), _value))
    {
        *(_first + _holeindex) = *(_first + parent_index);
        _holeindex = parent_index;
        parent_index = (_holeindex - 1) / 2;
    }
    *(_first + _holeindex) = _value;
}

/*
 * pushes the element at _last-1 onto the valid heap over the range [_first, _last-1),
 * after completion, [_first, _last) is a valid heap.
 */
template<class RandomIt>
inline void
push_heap(RandomIt _first, RandomIt _last)
{
    typedef typename iterator_traits<RandomIt>::value_type      _value_type;
    typedef typename iterator_traits<RandomIt>::difference_type _distance_type;

    _value_type value = *(_last-1);
    ustl::__push_heap(_first, _distance_type((_last-_first)-1), _distance_type(0), value, ustl::less<_value_type>());
}

template<class RandomIt, class Compare>
inline void
push_heap(RandomIt _first, RandomIt _last, Compare _cmp)
{
    typedef typename iterator_traits<RandomIt>::value_type      _value_type;
    typedef typename iterator_traits<RandomIt>::difference_type _distance_type;

    _value_type value = *(_last-1);
    ustl::__push_heap(_first, _distance_type((_last-_first)-1), _distance_type(0), value, _cmp);
}

template<class RandomIt, class Distance, class T, class Compare>
void
__adjust_heap(RandomIt _first, Distance _holeindex, Distance _len, T _value, Compare _cmp)
{
    const Distance topindex = _holeindex;
    Distance secondchild = _holeindex;

    while (secondchild < (_len-1) / 2)
    {
        secondchild  = (secondchild+1)*2;
        if ( _cmp(*(_first+secondchild), *(_first+secondchild-1)) )
            --secondchild;
        _value = *(_first + secondchild);
        _holeindex = secondchild;
    }

    if ( (_len & 1) == 0 && secondchild == (_len-2)/2 )
    {
        secondchild = 2*(secondchild+1);
        *(_first + _holeindex) = *(_first + secondchild - 1);
        _holeindex = secondchild-1;
    }

    ustl::__push_heap(_first, _holeindex, topindex, _value, _cmp);
}

template<class RandomIt, class Compare>
inline void
__pop_heap(RandomIt _first, RandomIt _last, RandomIt _result, Compare _cmp)
{
    typedef typename iterator_traits<RandomIt>::value_type      _value_type;
    typedef typename iterator_traits<RandomIt>::difference_type _distance_type;
    _value_type value = *_result;
    *_result = *_first;
    ustl::__adjust_heap(_first, _distance_type(0), _distance_type(_last - _first), value, _cmp);
}

/*
 * move the first value onto last, and make [_first, _last-1) a valid heap again
 */
template<class RandomIt, class Compare>
inline void
pop_heap(RandomIt _first, RandomIt _last, Compare _cmp)
{
    if (_last - _first > 1)
    {
        --_last;
        ustl::__pop_heap(_first, _last, _last, _cmp);
    }
}

template<class RandomIt, class Distance, class Compare>
Distance
__is_heap_until(RandomIt _first, Distance _n, Compare _cmp)
{
    Distance parent = 0, child = 1;
    for (; child < _n; ++child)
    {
        if (_cmp(*(_first+parent), *(_first+child)))
            return child;
        if ((child&1) == 0)
            ++parent;
    }
    return _n;
}

template<class RandomIt>
inline bool
is_heap(RandomIt _first, RandomIt _last)
{
    typedef typename iterator_traits<RandomIt>::difference_type distance_type;
    typedef typename iterator_traits<RandomIt>::value_type      value_type;
    distance_type len = ustl::advance(_first, _last);
    return (__is_heap_until(_first, len, ustl::less<value_type>()) == len);
}

template<class RandomIt, class Compare>
inline bool
is_heap(RandomIt _first, RandomIt _last, Compare _cmp)
{
    typedef typename iterator_traits<RandomIt>::difference_type distance_type;
    distance_type len = ustl::advance(_first, _last);
    return (__is_heap_until(_first, len, _cmp) == len);
}

}

