
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
__string_base<charT, Allocator>::__string_base(size_type _len):
    _M_str(nullptr), _M_len(0), _M_capacity(0), _M_ref(0)
{
    _M_str = Allocator::allocate(_Ch_size()*_len);
    _M_len = _len;
    _M_capacity = _len;
    _M_ref = 0;
}

template<class charT, class Allocator>
__string_base<charT, Allocator>::~__string_base()
{
    if (_M_str != nullptr)
        Allocator::deallocate(_M_str);
}

template<class charT, class Allocator>
typename __string_base<charT, Allocator>::_Self*
__string_base<charT, Allocator>::deattach_and_new()
{
    // FIXME: the change of _M_ref should be automatic
    _Self *new_base = new __string_base();
    new_base->_M_str = Allocator::allocate(_M_capacity);
    ustl::uninitialized_copy_forward(_M_str, _M_str+_M_len, new_base->_M_str);
    new_base->_M_len = _M_len;
    new_base->_M_capacity = _M_capacity;
    new_base->_M_ref = 1;

    return new_base;
}

template<class charT, class Allocator>
void
__string_base<charT, Allocator>::_M_deref()
{
    if (_M_ref > 0) 
        --_M_ref;
    else if (_M_ref == 0) {
        Allocator::deallocate(_M_str);
    }
}

template<class charT, class Allocator>
void
__string_base<charT, Allocator>::_M_reserve_cap(size_type _cap)
{
    if (_cap > _M_capacity) 
    {
        assert(_cap <= max_size());
        pointer new_str = Allocator::allocate(_cap);
        ustl::uninitialized_copy_forward(_M_str, _M_str+_M_len, new_str);
        Allocator::deallocate(_M_str);
        _M_str = new_str;
        _M_capacity = _cap;
    }
}

/*******************************************/
/************ basic_string *****************/
/*******************************************/

/*********** Constructors ***********/

template<class charT, class Allocator>
basic_string<charT, Allocactor>::basic_string()
{

}

/*********** Assignments ************/


/*********** Element access ************/

/*********** Iterators ************/

/*********** Capacity ************/

template<class charT, class Allocator>
typename basic_string<charT, Allocactor>::size_type
basic_string<charT, Allocactor>::capacity()
{
    return _M_is_local() ? size_type(_S_local_capacity) : _M_allocated_capacity;
}


/*********** Modifiers ************/

/*********** Search ************/

/*********** Inner ***********/

template<class charT, class Allocator>
bool 
basic_string<charT, Allocactor>::_M_is_local() const 
{
    return _M_data() == _M_local_data();
}

template<class charT, class Allocator>
void
basic_string<charT, Allocactor>::_M_data(pointer _p)
{
    _M_p = _p;
}

template<class charT, class Allocator>
typename basic_string<charT, Allocactor>::pointer 
basic_string<charT, Allocactor>::_M_data() const 
{
    return _M_p;
}

template<class charT, class Allocator>
void 
basic_string<charT, Allocactor>::_M_length(size_type _length)
{
    _M_string_length = _length;
}

template<class charT, class Allocator>
typename basic_string<charT, Allocactor>::pointer
basic_string<charT, Allocactor>::_M_local_date() 
{
    return pointer(_M_local_buf);
}

template<class charT, class Allocator>
void
basic_string<charT, Allocactor>::_M_capacity(size_type _capacity)
{
    _M_allocated_capacity = _capacity;
}

}

