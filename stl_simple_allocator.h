
#pragma once

#include "stl_allocator.h"
#include <cstddef> // for size_t

namespace ustl {

template<class T, class Allocator = allocator<T> >
class simple_allocator {

public:
    static T* new_value(const T& value) {
        T* ptr = Allocator::allocate(1);
        Allocator::construct(ptr, value);
    }

    /* 
     * @Attention:
     * this function is used only for continous memory
     */
    static T* new_value(size_t count, const T& value) {
        T* ptr = Allocator::allocate(count);
        size_t pos = 0;
        for (; pos < count; ++pos)
            Allocator::construct(ptr+pos, value);
        return ptr;
    }

    static void delete_value(T* ptr) {
        Allocator::destroy(ptr);
        Allocator::deallocate(ptr);
    }

};

}
