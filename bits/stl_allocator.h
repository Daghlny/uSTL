
#pragma once

#include <iostream>
#include <climits>
#include "stl_alloc.imph.h"  // for alloc::allocate() and alloc::deallocate()
#include "stl_type_traits.h"
#include "stl_iterator.h"    // for value_type() function

namespace ustl{

template<typename T>
class allocator{
    public:
        typedef T            value_type;
        typedef T*           pointer;
        typedef const T*     const_pointer;
        typedef T&           reference;
        typedef const T&     const_reference;
        typedef size_t       size_type;
        typedef int          difference_type;

        template<typename U>
        struct rebind {
            typedef allocator<U> other;
        };

    public:
        static T* allocate(size_type n, const void* = 0);
        static T* allocate();

        static void deallocate(T* ptr, size_t n);
        static void deallocate(T* ptr);

        static void construct(T* ptr);
        static void construct(T* ptr, const T& value);

        static void destroy(T* ptr);
        static void destroy(T* first, T* last);
        static void destroy(T* ptr, size_type n);

        bool operator==(const allocator<T>& rhs) {
            return true;
        }

        size_type max_size() const {
            return size_type(UINT_MAX/sizeof(T));
        }
};

template<class T>
T* allocator<T>::allocate(size_type n, const void *_ptr) {
    if (n == 0) return 0;
    return static_cast<T*>(alloc::allocate(sizeof(T)*n));
}

template<class T>
T* allocator<T>::allocate(){
    return allocate(1);
}

template<class T>
void allocator<T>::deallocate(T* ptr, size_t n) {
    if (n == 0) return;
    alloc::deallocate(static_cast<void*>(ptr), sizeof(T)*n);
}

template<class T>
void allocator<T>::deallocate(T* ptr) {
    alloc::deallocate(static_cast<void*>(ptr), sizeof(T));
}

template<class T>
void allocator<T>::construct(T *ptr) {
    new(ptr) T();
}

template<class T>
void allocator<T>::construct(T *ptr, const T& value) {
    new(ptr) T(value);
}

/* 
 * this function is used in stl_uninitialization.h 
 */
template<class T>
void construct(T *first, T *last) {
    while (first != last)
        new (first++) T();
}

template<class T>
void allocator<T>::destroy(T* ptr) {
    ptr->~T();
}

template<class T>
void allocator<T>::destroy(T *first, T *last) {
    destroy(first, last);
}

template<class T>
void allocator<T>::destroy(T *first, size_type n) {
    size_t i;
    for (i = 0; i < n; ++i)
        (first++)->~T();
}
// If \T has a non-trivial destructor
template<class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type) {
    while (first != last) {
        destroy(&*first);
        first++;
    }
}

// If \T has a trivial destructor
template<class ForwardIterator>
inline void __destroy_aux(ForwardIterator, ForwardIterator, __true_type) {
    //pass
}

// check whether \T has a trivial destructor 
template <class ForwardIterator, class T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T *) {
    typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
    __destroy_aux(first, last, trivial_destructor());
}


template<class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
    __destroy(first, last, __value_type(first));
}

};

