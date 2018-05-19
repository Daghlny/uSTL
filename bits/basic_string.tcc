
#pragma once

#include <iostream>
#include <type_traits>
#include <limits>
#include <stdexcept>
#include <cstddef>
#include <atomic>   // for atomic_long used in \ref_type

#include "stl_allocator.h"
#include "stl_uninitialized.h"

using std::cout;
using std::endl;


namespace ustl
{

template<class charT, class Allocator>
string_base<charT, Allocator>::string_base():
    _M_str(NULL), _M_capacity(0), _M_ref(1)
{
}

template<class charT, class Allocator>
string_base<charT, Allocator>::string_base(size_type _len):
    _M_str(NULL), _M_capacity(0), _M_ref(1)
{
    _M_str = Allocator::allocate(_Ch_size()*_len);
    _M_capacity = _len;
}

template<class charT, class Allocator>
string_base<charT, Allocator>::~string_base()
{
    _M_deref();
    if (_M_str != NULL)
        Allocator::deallocate(_M_str);
}

template<class charT, class Allocator>
typename string_base<charT, Allocator>::_Self*
string_base<charT, Allocator>::deattach_and_new(size_type _len)
{
    // FIXME: the change of _M_ref should be automatic
    _Self *new_base = this;
    if (this->is_shared()) {
        new_base = new string_base();

        new_base->_M_str = Allocator::allocate(_M_capacity);
        ustl::uninitialized_copy_forward(_M_str, _M_str+_len, new_base->_M_str);

        new_base->_M_capacity = _M_capacity;
        new_base->_M_ref = 1;

        this->_M_deref();
    }

    return new_base;
}

/*
 * reserves the capacity of string_base, the growth parameter is 1.5
 */
template<class charT, class Allocator>
void
string_base<charT, Allocator>::_M_reserve(size_type _cap, size_type _len)
{
    const size_type old_cap = _M_capacity;
    if (_cap > old_cap) {
        size_type new_cap = old_cap + (old_cap+1)/2;
        if (new_cap < _cap)
            new_cap = _cap;
        pointer new_str = Allocator::allocate(new_cap);
        memset(new_str, 0, sizeof(charT)*new_cap);
        ustl::uninitialized_copy_forward(_M_str, _M_str+_len, new_str);
        Allocator::deallocate(_M_str);
        _M_str = new_str;
        _M_capacity = new_cap;
    }
}

/*
 * reserves the capacity of _M_str to store _count charactors in [_index, _index+_count]
 * it will leave a hole inside _M_str, but it can avoid a redundant data copy
 */
template<class charT, class Allocator>
typename string_base<charT, Allocator>::pointer
string_base<charT, Allocator>::_M_insert(size_type _index, size_type _count, size_type _len)
{
    const size_type new_cap = _M_capacity+_count;
    if (new_cap > _M_capacity) {
        pointer new_str = Allocator::allocate(new_cap);
        ustl::uninitialized_copy_forward(_M_str, _M_str+_index, new_str);
        ustl::uninitialized_copy_forward(_M_str+_index, _M_str+_len, new_str+_index+_count);
        _M_str = new_str;
        _M_capacity = new_cap;
        Allocator::deallocate(_M_str);
    } else {
        ustl::copy_backward(_M_str+_index, _M_str+_len, _M_str+_len+_count);
    }
    return _M_str+_index;
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
string_base<charT, Allocator>::_M_incref()
{
    ++_M_ref;
}

template<class charT, class Allocator>
bool
string_base<charT, Allocator>::is_shared()
{
    return (_M_ref != 1);
}

template<class charT, class Allocator>
typename string_base<charT, Allocator>::pointer
string_base<charT, Allocator>::data()
{
    return _M_str;
}


/*******************************************/
/************ basic_string *****************/
/*******************************************/

/*********** Constructors ***********/

template<class charT, class Allocator>
basic_string<charT, Allocator>::basic_string():
    _M_len(0)
{
    _M_p = _M_local_buf;
}

template<class charT, class Allocator>
basic_string<charT, Allocator>::basic_string(size_type _count, charT _ch):
    _M_len(0)
{
    _M_p = _M_local_buf;
    _M_ensure_writable(_count);
    ustl::uninitialized_fill(_M_p, _M_p+_count, _ch);
    _M_len = _count;
}

template<class charT, class Allocator>
basic_string<charT, Allocator>::basic_string(charT _ch):
    _M_len(0)
{
    _M_p = _M_local_buf;
    _M_ensure_writable(1);
    ustl::uninitialized_fill(_M_p, _M_p+1, _ch);
    _M_len = 1;
}

template<class charT, class Allocator>
basic_string<charT, Allocator>::basic_string(const _Self& _other):
    _M_len(0)
{
    _M_p = _M_local_buf;
    if (_other._M_is_local()) {
        ustl::uninitialized_copy_forward(_other.data(), _other.data()+_other.size(), _M_p);
    } else {
        _M_base = _other._M_base;
        _M_p = _M_base->_M_str;
        _M_base->_M_incref();
    }
    _M_len = _other.size();
}

/*********** Assignments ************/

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::_Self&
basic_string<charT, Allocator>::assign(size_type _count, charT _ch)
{
    clear();
    insert(begin(), size_type(_count), charT(_ch));
}


/*********** Element access ************/

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::reference
basic_string<charT, Allocator>::at(size_type pos)
{
    if (pos >= _M_len)
        throw "Out of Length";
    return *(_M_p+pos);
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::const_reference
basic_string<charT, Allocator>::at(size_type pos) const
{
    if (pos >= _M_len)
        throw "Out of Length";
    return *(_M_p+pos);
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::reference
basic_string<charT, Allocator>::operator[](size_type pos)
{
    return *(_M_p+pos);
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::const_reference
basic_string<charT, Allocator>::operator[](size_type pos) const
{
    return *(_M_p+pos);
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::reference
basic_string<charT, Allocator>::front()
{
    return *_M_p;
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::const_reference
basic_string<charT, Allocator>::front() const
{
    return *_M_p;
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::reference
basic_string<charT, Allocator>::back()
{
    return *(_M_p+_M_len-1);
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::const_reference
basic_string<charT, Allocator>::back() const
{
    return *(_M_p+_M_len-1);
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::pointer
basic_string<charT, Allocator>::data()
{
    return _M_p;
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::const_pointer
basic_string<charT, Allocator>::data() const
{
    return _M_p;
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::const_pointer
basic_string<charT, Allocator>::c_str() const
{
    return _M_p;
}

/*********** Iterators ************/

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::iterator
basic_string<charT, Allocator>::begin()
{
    return _M_p;
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::iterator
basic_string<charT, Allocator>::end()
{
    return _M_p+_M_len;
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::const_iterator
basic_string<charT, Allocator>::cbegin() const
{
    return _M_p;
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::const_iterator
basic_string<charT, Allocator>::cend() const
{
    return _M_p + _M_len;
}

/*********** Capacity ************/


template<class charT, class Allocator>
bool
basic_string<charT, Allocator>::empty() const
{
    return _M_len == 0;
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::size_type
basic_string<charT, Allocator>::size() const
{
    return _M_len;
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::size_type
basic_string<charT, Allocator>::length() const
{
    return this->size();
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::size_type
basic_string<charT, Allocator>::capacity() const
{
    return _M_is_local() ? size_type(_S_local_capacity) : _M_base->_M_capacity;
}


/*********** Modifiers ************/

/*
 * Standard does't require that the capacity is unchanged by clear(), but most 
 * implementations don't change capacity
 */
template<class charT, class Allocator>
void
basic_string<charT, Allocator>::clear()
{
    _M_len = 0;
}


template<class charT, class Allocator>
typename basic_string<charT, Allocator>::_Self&
basic_string<charT, Allocator>::insert(size_type _index, size_type _count, charT _ch)
{
    _M_ensure_writable(_M_len+_count);
    __insert(_index, _count, _ch);
    return *this;
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::_Self&
basic_string<charT, Allocator>::insert(size_type _index, const charT* _s)
{
    size_type s_len = strlen(_s);
    _M_ensure_writable(_M_len+s_len);
    __insert(_index, _s, s_len);
    return *this;
}

/*
 * suppose there is enough capacity in basic_string
 */
template<class charT, class Allocator>
void
basic_string<charT, Allocator>::__insert(size_type _index, size_type _count, charT _ch)
{
    ustl::copy_backward(_M_p+_index, _M_p+_M_len, _M_p+_M_len+_count);
    ustl::fill_n(_M_p+_index, _count, _ch);
    _M_len += _count;
}

template<class charT, class Allocator>
void
basic_string<charT, Allocator>::__insert(size_type _index, const charT* _s, size_type _s_len)
{
    ustl::copy_backward(_M_p+_index, _M_p+_M_len, _M_p+_M_len+_s_len);
    ustl::copy(_s, _s+_s_len, _M_p+_index);
    _M_len += _s_len;
}



/*********** Search ************/

/*********** Inner ***********/

template<class charT, class Allocator>
bool 
basic_string<charT, Allocator>::_M_is_local() const 
{
    return _M_p == _M_local_buf;
}

/*
 * ensures that basic_string having enough memory(_requested_cap) to store 
 * also checks the shared string_base
 *
 * But this method doesn't update _M_len
 */
template<class charT, class Allocator>
void
basic_string<charT, Allocator>::_M_ensure_writable(size_type _requested_cap)
{
    if ( _M_is_local() ) 
    {
        if (_requested_cap >= _S_local_capacity) {
            string_base<charT, Allocator>* tmp_base = new string_base<charT, Allocator>(_requested_cap);
            ustl::copy(_M_local_buf, _M_local_buf+_M_len, tmp_base->_M_str);
            _M_base = tmp_base;
            _M_p = _M_base->_M_str;
        }
    } else {
        if ( _M_base->is_shared() ) {
            _M_base = _M_base->deattach_and_new(_M_len);
            _M_p = _M_base->_M_str;
        }
        _M_base->_M_reserve(_requested_cap, _M_len);
        _M_p = _M_base->_M_str;
    }
}


}

