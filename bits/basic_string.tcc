
#pragma once

#include <iostream>
#include <type_traits>
#include <limits>
#include <stdexcept>

#include "stl_allocator.h"
#include "stl_uninitialized.h"

using std::cout;
using std::endl;

namespace ustl
{

template<class charT, class Allocator>
__string_base<charT, Allocator>::__string_base(size_type _len)
{
    _M_str = Allocator::allocate(_Ch_size()*_len);
    _M_len = _len;
    _M_capacity = _len;
    _M_ref = 0;
}

template<class charT, class Allocator>
__string_base<charT, Allocator>::~__string_base()
{
    _M_deallocate();
}

template<class charT, class Allocator>
void
__string_base<charT, Allocator>::_M_deallocate()
{
    if (_M_ref > 0) 
        --_M_ref;
    else if (_M_ref == 0) {
        Allocator::deallocate(_M_str);
    }
}

}
