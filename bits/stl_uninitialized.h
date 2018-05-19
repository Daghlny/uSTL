
#pragma once

#include <string.h>

#include "algorithm.tcc"
#include "stl_allocator.h"
#include "stl_type_traits.h"


namespace ustl {

/* uninitialized series functions are used to construct objects with a range */

/*
 * ***************************************************
 * ********* uninitialized_fill_n function ***********
 * ***************************************************
 */


/* 
 * according to whether the type \T is POD(Plain Old Data), we can call the diffierent fill functions
 * if \T is POD, the fourth parameter is \__true_type, otherwise, \__false_type
 * POD type has trivial ctor/dtor/copy/assignment
 */

template<class ForwardIterator, class Size, class T>
inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __true_type) 
{
    return fill_n(first, n, x);
}

template<class ForwardIterator, class Size, class T>
ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __false_type) 
{
    ForwardIterator cur = first;
    for (; n > 0; --n, ++cur)
        allocator<T>::construct(&*cur, x);
    return cur;
}

/*
 * This function will extract the type_trait of \T to check if it is a POD type
 */
template<class ForwardIterator, class Size, class T, class T1>
inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*) 
{
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    return __uninitialized_fill_n_aux(first, n, x, is_POD());
}

/*
 *  main function
 */
template<class ForwardIterator, class Size, class T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x)
{
    return __uninitialized_fill_n(first, n, x, __value_type(first));
}

/*
 * ***************************************************
 * ********* uninitialized_copy function *************
 * ***************************************************
 */


template<class InputIterator, class ForwardIterator>
inline ForwardIterator __uninitialized_copy_aux_forward(InputIterator first, InputIterator last, ForwardIterator result, __true_type) 
{
    return ustl::copy(first, last, result);
}

template<class InputIterator, class ForwardIterator>
inline ForwardIterator __uninitialized_copy_aux_forward(InputIterator first, InputIterator last, ForwardIterator result, __false_type) 
{
    ForwardIterator cur = result;
    for (; first != last; ++first, ++cur) 
        construct(&*cur, *first);
    return cur;
}

template<class InputIterator, class ForwardIterator, class T>
inline ForwardIterator __uninitialized_copy_forward(InputIterator first, InputIterator last, ForwardIterator result, T*) 
{
    typedef typename __type_traits<T>::is_POD_type is_POD;
    return __uninitialized_copy_aux_forward(first, last, result, is_POD());
}

template<class InputIterator, class ForwardIterator>
inline ForwardIterator __uninitialized_copy_aux_backward(InputIterator first, InputIterator last, ForwardIterator result, __true_type) 
{
    return ustl::copy_backward(first, last, result);
}

template<class InputIterator, class ForwardIterator>
inline ForwardIterator __uninitialized_copy_aux_backward(InputIterator first, InputIterator last, ForwardIterator result, __false_type) 
{
    while (first != last)
        construct(& (*(--result)), *(--last));
    return result;
}

template<class InputIterator, class ForwardIterator, class T>
inline ForwardIterator __uninitialized_copy_backward(InputIterator first, InputIterator last, ForwardIterator result, T*) 
{
    typedef typename __type_traits<T>::is_POD_type is_POD;
    return __uninitialized_copy_aux_backward(first, last, result, is_POD());
}


/*
 * copies the elements between [first, last) to the range[result, result+(last-first))
 */
template<class InputIterator, class ForwardIterator>
inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) 
{
    return __uninitialized_copy(first, last, result, __value_type(result));
}

/*
 * copies forwards
 */
template<class InputIterator, class ForwardIterator>
inline ForwardIterator uninitialized_copy_forward(InputIterator first, InputIterator last, ForwardIterator result)
{
    return __uninitialized_copy_forward(first, last, result, __value_type(result));
}

template<class InputIterator, class ForwardIterator>
inline ForwardIterator uninitialized_copy_backward(InputIterator first, InputIterator last, ForwardIterator result)
{
    return __uninitialized_copy_backward(first, last, result, __value_type(result));
}

inline char*
uninitialized_copy(const char* first, const char* last, char* result) 
{
    memmove(result, first, last-first);
    return result+(last-first);
}

inline wchar_t*
uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result)
{
    memmove(result, first, sizeof(wchar_t)*(last-first));
    return result+(last-first);
}

/*
 * ***************************************************
 * ********* uninitialized_fill function *************
 * ***************************************************
 */



template<class ForwardIterator, class T>
inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __true_type) 
{
    fill(first, last, x);
}

template<class ForwardIterator, class T>
inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __false_type) 
{
    ForwardIterator cur = first;
    for (; cur != last; ++cur)
        allocator<T>::construct(&*cur, x);

}

template<class ForwardIterator, class T, class T1>
inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, T1*)
{
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    __uninitialized_fill_aux(first, last, x, is_POD());
}

template<class ForwardIterator, class T>
inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x)
{
    __uninitialized_fill(first, last, x, __value_type(first));
}

}// end of namespace 

