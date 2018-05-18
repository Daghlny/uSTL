
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
string_base<charT, Allocator>::string_base(size_type _len):
    _M_str(nullptr), _M_len(0), _M_capacity(0), _M_ref(0)
{
    _M_str = Allocator::allocate(_Ch_size()*_len);
    _M_len = _len;
    _M_capacity = _len;
    _M_ref = 1;
}

template<class charT, class Allocator>
string_base<charT, Allocator>::~string_base()
{
    _M_deref();
    if (_M_str != nullptr)
        Allocator::deallocate(_M_str);
}

template<class charT, class Allocator>
typename string_base<charT, Allocator>::_Self*
string_base<charT, Allocator>::deattach_and_new()
{
    // FIXME: the change of _M_ref should be automatic
    _Self *new_base = new string_base();
    new_base->_M_str = Allocator::allocate(_M_capacity);
    ustl::uninitialized_copy_forward(_M_str, _M_str+_M_len, new_base->_M_str);
    new_base->_M_len = _M_len;
    new_base->_M_capacity = _M_capacity;
    new_base->_M_ref = 1;
    this->_M_deref();

    return new_base;
}

template<class charT, class Allocator>
void
string_base<charT, Allocator>::_M_deref()
{
    if (_M_ref > 0) 
        --_M_ref;
    else if (_M_ref == 0) {
        Allocator::deallocate(_M_str);
    }
}

template<class charT, class Allocator>
void
string_base<charT, Allocator>::_M_reserve_cap(size_type _cap)
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

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::_Rep*
basic_string<charT, Allocator>::_Rep::_S_create(size_type _capacity, size_type _old_capacity)
{
    if (_capacity > _S_max_size)
        throw "basic_string::_S_create";
    const size_type pagesize = 4096;
    const size_type malloc_header_size = 4*sizeof(void*);
    if (_capacity > _old_capacity && _capacity < 2*_old_capacity)
        _capacity = 2*_old_capacity;
    size_type _size = (_capacity+1)*sizeof(charT)+sizeof(_Rep);
    const size_type adj_size = _size + malloc_header_size;

    if (adj_size > pagesize && _capacity > _old_capacity)
    {
        const size_type extra = pagesize - adj_size%pagesize;
        _capacity += extra/sizeof(charT);
        if (_capacity > _S_max_size)
            _capacity = _S_max_size;
        _size = (_capacity+1)*sizeof(charT)+sizeof(_Rep);
    }
    void *_place = Allocator::allocate(_size);
    _Rep *p      = new (_plate) _Rep;
    p->_M_capacity = _capacity;

    p->_M_set_sharable();

    return _p;
}

template<class charT, class Allocator>
_charT*
basic_string<charT, Allocator>::_Rep::_M_clone(size_type _res)
{
    const size_type requested_cap = this->_M_length + _res;
    _Rep *r = _Rep::_S_create(requested_cap, this->_M_capacity);
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

