
#pragma once

#include "stl_alloc.h"
#include "stl_construct.h"

namespace uSTL{

template<typename T>
class allocator{
    public:
        typedef T            value_type;
        typedef T*           pointer;
        typedef const T*     const_pointer;
        typedef T&           reference;
        typedef const T&     const_reference;
        typedef size_t       size_type;
        typedef ptrdiff_t    difference_type;
        template<typename U>
        struct rebind {
            typedef allocator<U> other;
        };

    public:
        pointer        allocate(size_type n, const void* = 0);
        void           deallocate(pointer ptr, size_type n);
        size_type      max_size() const;
        void           construct(pointer p, const T& x);
        void           destroy(pointer p);
        pointer        address(reference x);
        const_pointer  const_address(const_reference x);
};


        static T* allocate();
        static T* allocate(size_t n);
        static void deallocate(T* ptr);
        static void deallocate(T* ptr, size_t n);

        static void construct(T* ptr);
        static void construct(T* ptr, const T& value);
        static void destroy(T* ptr);
        static void destroy(T* first, T* last);
        static void destroy(T* ptr, size_t n);
};

template<class T>
allocator<T>::pointer allocate(size_type n, const void *_ptr = 0) {
    return static_cast<T*>(alloc::allocate(sizeof(T)));
}

template<class T>
T* allocator<T>::allocate(){
    return static_cast<T*>(alloc::allocate(sizeof(T)));
}

template<class T>
T* allocator<T>::allocate(size_t n){
    if (n <= 0) 
        return NULL;
    return static_cast<T*>(alloc::allocate(sizeof(T)*n));
}

template<class T>
void allocator<T>::deallocate(T* ptr) {
    alloc::deallocate(static_cast<void*>(ptr), sizeof(T));
}

template<class T>
void allocator<T>::deallocate(T* ptr, size_t n) {
    if (n == 0) return;
    alloc::deallocate(static_cast<void*>(ptr), sizeof(T)*n);
}

template<class T>
void allocator<T>::construct(T *ptr) {
    new(ptr)T();
}

template<class T>
void allocator<T>::construct(T *ptr, const T& value) {
    new(ptr)T(value);
}

template<class T>
void allocator<T>::destroy(T* ptr) {
    ptr->~T();
}

template<class T>
void allocator<T>::destroy(T *first, T *last) {
    while (first != last) 
        (first++)->~T();
}

template<class T>
void allocator<T>::destroy(T *first, size_t n) {
    size_t i;
    for (i = 0; i < n; ++i)
        (first++)->~T();
}

}

