
#pragma once

#include <cstddef>
#include <type_traits>

#include "stl_iterator.h"
#include "stl_allocator.h"
#include "stl_simple_allocator.h"

namespace ustl{

template<class T, class Allocator>
class __deque_base {
    public:
        __deque_base(size_t __len) {
            _data = Allocatlr::alloc(sizeof(T*)*__len);
            _len  = __len;
        }

    private:
        T** _data;
        size_t _len;
};

template<class T>
class __deque_iterator {
    public:


};

template<class T, class Allocator>
class deque{
    public:
        typedef __deque_iterator<T> iterator; 
};

}

