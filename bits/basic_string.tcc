
#pragma once

#include <iostream>
#include <type_traits>
#include <limits>
#include <stdexcept>
#include <cstddef>
#include <atomic>   // for atomic_long used in \ref_type
#include <cassert>

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
    assert(this->_M_str == NULL);
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
        _M_str = NULL;
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
bool
string_base<charT, Allocator>::is_leaked()
{
    return (_M_ref == 0);
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
    _M_initialize(_other, 0, _other.size());
}

template<class charT, class Allocator>
basic_string<charT, Allocator>::basic_string(const _Self& _other, size_type _pos, size_type _count):
    _M_len(0)
{
    _M_initialize(_other, _pos, _count);    
}

template<class charT, class Allocator>
basic_string<charT, Allocator>::basic_string(const _Self& _other, size_type _pos):
    _M_len(0)
{
    size_type count = _other.size() - _pos;
    _M_initialize(_other, _pos, count);    
}

/* Destructor */

template<class charT, class Allocator>
basic_string<charT, Allocator>::~basic_string()
{
    if ( ! this->_M_is_local() ) {
        this->__base_deref();
    }
}

// FIXME:
// you should think clearly that the allocate and data copy is in which func, constructor or initialization ?
template<class charT, class Allocator>
void
basic_string<charT, Allocator>::_M_initialize(const char* _s, size_type _index, size_type _count)
{
    _M_ensure_writable(_M_len + _count);
    ustl::uninitialized_copy_forward(_s+_index, _s+_index+_count, _M_p);
    _M_len = _count;
}

template<class charT, class Allocator>
void
basic_string<charT, Allocator>::_M_initialize(const _Self& _other, size_type _index, size_type _count)
{
    _M_p = _M_local_buf;
    if (_other._M_is_local()) {
        const_pointer other_p = _other.data();
        ustl::uninitialized_copy_forward(other_p+_index, other_p+_index+_count, _M_p);
    } else {
        if (_index == 0 && _count == _other.size()) {
            _M_base = _other._M_base;
            _M_p = _M_base->_M_str;
            _M_base->_M_incref();
        } else {
            const_pointer other_p = _other.data();
            _M_ensure_writable(_count);
            ustl::uninitialized_copy_forward(other_p+_index, other_p+_index+_count, _M_p);
        }
    }
    _M_len = _count;
}

/*********** Assignments ************/

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::_Self&
basic_string<charT, Allocator>::assign(size_type _count, charT _ch)
{
    clear();
    insert(begin(), size_type(_count), charT(_ch));
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::_Self&
basic_string<charT, Allocator>::operator=(const _Self& other)
{
    if (this->_M_is_local() && other._M_is_local()) {
        ustl::copy(other.data(), other.data()+other.size(), _M_p);
    } else if (this->_M_is_local() && !other._M_is_local()) {
        _M_base = other._M_base;
        _M_base->_M_incref();
    } else if (!this->_M_is_local() && other._M_is_local()) {
        this->__base_deref();
        ustl::copy(_M_local_buf, _M_local_buf+other.size(), other.data());
    } else {
        this->__base_deref();
        _M_base = other._M_base;
        _M_base->_M_incref();
    }

    return *this;
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

template<class charT, class Allocator>
void
basic_string<charT, Allocator>::shrink_to_fit()
{
//FIXME: finish this function and add a private method to integrate the derefference
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
    __insert(_index, _s, 0, s_len);
    return *this;
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::_Self&
basic_string<charT, Allocator>::insert(size_type _index, const charT* _s, size_type _count)
{
    _M_ensure_writable(_M_len+_count);
    __insert(_index, _s, 0, _count);
    return *this;
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::_Self&
basic_string<charT, Allocator>::insert(size_type _index, const _Self& other)
{
    _M_ensure_writable(_M_len+other.size());
    __insert(_index, other.data(), 0, other.size());
    return *this;
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::_Self&
basic_string<charT, Allocator>::insert(size_type _index, const _Self& other, 
                                       size_type _a_index, size_type _count)
{
    _M_ensure_writable(_M_len+_count);
    __insert(_index, other.data(), _a_index, _count);
    return *this;
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::iterator
basic_string<charT, Allocator>::insert(iterator _pos, charT _ch)
{
    size_type _index = _pos - begin();
    _M_ensure_writable(_M_len+1);
    __insert(begin()+_index, 1, _ch);
    return begin()+_index;
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::iterator
basic_string<charT, Allocator>::insert(iterator _pos, size_type _count, charT _ch)
{
    size_type _index = _pos - begin();
    _M_ensure_writable(_M_len+_count);
    __insert(begin()+_index, _count, _ch);
    return begin()+_index;
}

template<class charT, class Allocator>
template<class InputIt>
typename basic_string<charT, Allocator>::iterator
basic_string<charT, Allocator>::insert(iterator _pos, InputIt _first, InputIt _last)
{
    return insert_aux(_pos, _first, _last, typename std::is_integral<InputIt>::type());
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::_Self&
basic_string<charT, Allocator>::erase(size_type _index, size_type _count)
{
    this->__erase(_index, _count);
    return *this;
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::iterator
basic_string<charT, Allocator>::erase(iterator _pos)
{
    size_type index = _pos-begin();
    this->__erase(index, 1);
    return begin()+index;
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::iterator
basic_string<charT, Allocator>::erase(iterator _first, iterator _last)
{
    size_type index = _first-begin();
    size_type count = ustl::distance(_first, _last);
    this->__erase(index, count);
    return begin()+index;
}

template<class charT, class Allocator>
void
basic_string<charT, Allocator>::push_back(charT ch)
{
    this->_M_ensure_writable(this->size() + 1);
    *(_M_p + _M_len) = ch;
    ++_M_len;
}

template<class charT, class Allocator>
void
basic_string<charT, Allocator>::pop_back()
{
    if (this->empty())
        throw "pop the empty string";
    this->_M_ensure_writable(this->size()-1);
    --_M_len;
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::_Self
basic_string<charT, Allocator>::substr(size_type pos, size_type count) const
{
    _Self res(*this, pos, count);
    return res;
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::_Self&
basic_string<charT, Allocator>::operator+=(const _Self& _other) 
{
    this->_M_ensure_writable(size()+_other.size());
    this->insert(end(), _other.cbegin(), _other.cend());
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::_Self&
basic_string<charT, Allocator>::operator+=(charT ch) 
{
    this->_M_ensure_writable(size()+1);
    *(_M_p+(_M_len++)) = ch;
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::_Self&
basic_string<charT, Allocator>::operator+=(const charT* s) 
{
    const size_type s_len = strlen(s);
    this->_M_ensure_writable(size()+s_len);
    int pos = 0;
    for (; pos < s_len; ++pos)
        this->push_back(s[pos]);
}

//////////////////////////
// inner erase methods //
//////////////////////////

template<class charT, class Allocator>
void
basic_string<charT, Allocator>::__erase(size_type _index, size_type _count)
{
    const size_type elems_after = size()-_index;
    this->_M_ensure_writable(_M_len - _count);
    if (elems_after < _count) {
        _count = elems_after;
    } else if (elems_after > _count){
        iterator pos = this->begin()+_index;
        ustl::copy(pos+_count, end(), pos);
    }
    _M_len -= _count;
}


//////////////////////////
// inner insert methods //
//////////////////////////
template<class charT, class Allocator>
template<class InputIt>
typename basic_string<charT, Allocator>::iterator
basic_string<charT, Allocator>::insert_aux(iterator _pos, InputIt _first, InputIt _last, std::true_type)
{
    return insert(_pos, size_type(_first), charT(_last));
}

template<class charT, class Allocator>
template<class InputIt>
typename basic_string<charT, Allocator>::iterator
basic_string<charT, Allocator>::insert_aux(iterator _pos, InputIt _first, InputIt _last, std::false_type)
{
    size_type count = ustl::distance(_first, _last);
    size_type index = _pos - begin();
    _M_ensure_writable(_M_len+count);
    __insert(begin()+index, _first, _last, count);
    return begin()+index;
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
basic_string<charT, Allocator>::__insert(size_type _index, const charT* _s, 
                                         size_type _s_index, size_type _s_count)
{
    ustl::copy_backward(_M_p+_index, _M_p+_M_len, _M_p+_M_len+_s_count);
    ustl::copy(_s+_s_index, _s+_s_index+_s_count, _M_p+_index);
    _M_len += _s_count;
}

////////// insert with iterator as parameter

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::iterator
basic_string<charT, Allocator>::__insert(iterator _pos, size_type _count, charT _ch)
{
    iterator old_end = end();
    ustl::copy_backward(_pos, _pos+_count, old_end+_count);
    ustl::fill_n(_pos, _count, _ch);
    _M_len += _count;
    return _pos;
}

template<class charT, class Allocator>
template<class InputIt>
typename basic_string<charT, Allocator>::iterator
basic_string<charT, Allocator>::__insert(iterator _pos, InputIt _first, InputIt _last, size_type _count)
{
    iterator old_end = end();
    ustl::copy_backward(_pos, old_end, old_end+_count);
    ustl::copy(_first, _last, _pos);
    _M_len += _count;
    return _pos;
}

/*********** Search ************/

template<class charT, class Allocator>
int
basic_string<charT, Allocator>::compare(const _Self& _str) const
{
    int len = ustl::min(size(), _str.size());
    int res = __compare(data(), _str.data(), len);
    if (res != 0)
        return res;
    else if (this->size() < _str.size())
        return -1;
    else if (this->size() >_str.size())
        return 1;
    return 0;
}

template<class charT, class Allocator>
int
basic_string<charT, Allocator>::compare(size_type _pos1, size_type _count1, const _Self& _str, size_type _pos2, size_type _count2) const
{
    int len = ustl::min(_count1, _count2);
    int res = __compare(data()+_pos1, _str.data()+_pos2, len);
    if (res != 0)
        return res;
    if (_count1 < _count2)
        return -1;
    if (_count1 > _count2)
        return 1;
    return 0;
}

template<class charT, class Allocator>
int
basic_string<charT, Allocator>::compare(const charT* s) const
{
    const size_type s_len = strlen(s);
    int res = __compare(data(), s, ustl::min(s_len, this->size()));
    if (res != 0)
        return res;
    if (this->size() < s_len)
        return -1;
    if (this->size() > s_len)
        return 1;
    return 0;
}

template<class charT, class Allocator>
int 
basic_string<charT, Allocator>::__compare(const charT* _s1, const charT* _s2, size_type _len) const
{
    int i = 0;
    for (; i < _len; ++i) {
        if (_s1[i] < _s2[i]) 
            return -1;
        else if (_s1[i] > _s2[i])
            return 1;
    }
    return 0;
}

template<class charT, class Allocator>
typename basic_string<charT, Allocator>::size_type 
basic_string<charT, Allocator>::find(const _Self& _other, size_type _pos) const
{

}

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

template<class charT, class Allocator>
void
basic_string<charT, Allocator>::__base_deref()
{
    _M_base->_M_deref();
    // when _M_base is leaked, it has already deallocated _M_str in it
    if (_M_base->is_leaked()) {
        delete _M_base;
        _M_base = NULL;
    }
}


}

