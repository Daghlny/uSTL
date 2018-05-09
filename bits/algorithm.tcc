
#pragma once

//#include "stl_algorithm.h"

namespace ustl {

template<class T>
class less {
    public:
        bool operator()(const T& lhs, const T& rhs) {
            return lhs < rhs;
        }
};

template<class T>
class greater {
    public:
        bool operator()(const T& lhs, const T& rhs) {
            return lhs > rhs;
        }
};

/*
 * exchanges the given values
 */
template<class T>
void swap(T& a, T& b){
    T temp = a;
    a = b;
    b = temp;
}

template<class T>
T max(const T& lhs, const T& rhs)
{
    if (lhs < rhs)
        return rhs;
    else
        return lhs;
}

template<class T>
T min(const T& lhs, const T& rhs)
{
    if (lhs < rhs)
        return lhs;
    else
        return rhs;
}

/**********************************************/
/************** Copy Functions ****************/
/**********************************************/

/*
 * TODO: this function can be optimized by POD and different situations
 * copy the elements between [first, last) into [d_first, d_first+(last-first) )
 */
template<class InputIt, class OutputIt>
OutputIt copy(InputIt first, InputIt last, OutputIt d_first) {
    while (first != last) {
        *(d_first++) = *(first++);
    }
    return d_first;
}

/* 
 * TODO: this function can be optimized
 * copies the elements from the range, defined by [first, last), to another range
 * ending at d_last. The elements are copied in reverse order.
 * The behavior is undefined if d_last is within (first, last]. ustl::copy must be 
 * used instead of ustl::copy_backward in that case.
 */
template<class BidirIt1, class BidirIt2>
BidirIt2 copy_backward(BidirIt1 first, BidirIt1 last, BidirIt2 d_last) {
    BidirIt2 d_ret = d_last;
    last--, d_last--;
    while (last != first) {
        *(last--) = *(d_last--);
    }
    return d_ret;
}


/**********************************************/
/************** Fill Functions ****************/
/**********************************************/
template<class ForwardIt, class T>
void fill(ForwardIt first, ForwardIt last, const T& value) {
    for (; first != last; ++first)
        *first = value;
}

template<class OutputIt, class Size, class T>
OutputIt fill_n(OutputIt first, Size count, const T& value) {
    for (Size i = 0; i < count; ++i) {
        *first++ = value;
    }
    return first;
}

/**********************************************/
/************** Move Functions ****************/
/**********************************************/

template<class B1, typename B2>
inline B2
move(B1 _first, B1 _last, B2 _result)
{
    while (_first != _last)
        *(_result++) = *(_first++);
    return _result;
}

template<class B1, typename B2>
inline B2
move_backward(B1 _first, B1 _last, B2 _result) 
{
    while (_first != _last) 
        *(--_result) = *(--_last);
    return _result;
}

}

