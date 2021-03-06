
#pragma once

#include <stdexcept> // for out-of-range
#include <limits>    // for numeric_limits
#include <type_traits> // for is_integral
#include <iostream>  // for debug

#include "stl_allocator.h"
#include "stl_uninitialized.h"
#include "stl_vector.h"
#include "algorithm.tcc"

using std::cout;
using std::endl;

namespace ustl {

#if __cplusplus >= 201103L
template<class T, class Allocator>
vector<T, Allocator>::vector(std::initializer_list<T> _initL):
    _start(0), _finish(0), _end_of_storage(0)
{
    cout << "enter the initlist constructor" << endl;
    reserve(_initL.size());
    ustl::uninitialized_copy(_initL.begin(), _initL.end(), _start);
    _finish = _start + _initL.size();
}

template<class T, class Allocator>
vector<T, Allocator>::vector(_Self&& _v):
    _start(_v._start), _finish(_v._finish), _end_of_storage(_v._end_of_storage)
{
    cout << "enter the move constructor" << endl;
}
#endif

template<class T, class Allocator>
vector<T, Allocator>::vector(const _Self& _v):
    _start(0), _finish(0), _end_of_storage(0)
{
    cout << "enter the copy constructor" << endl;
    reallocate(_v.capacity());
    ustl::uninitialized_copy(_v.cbegin(), _v.cend(), _start);
    _finish = _start + _v.size();
}

/* 
 * reallocates the memory of vector, the default size is 2*old_size 
 * but the new size will not smaller than \insert_count + old_size
 */
template<class T, class Allocator>
void vector<T, Allocator>::reallocate(size_type insert_count) {

    const size_type old_size = size();
    size_type len = old_size != 0 ? 2 * old_size : 1;
    len = len > (insert_count + size()) ? len : insert_count + size();

    iterator new_start = data_allocator::allocate(len);
    iterator new_finish = new_start;

    try {
        new_finish = uninitialized_copy(_start, _finish, new_start);
    }
    catch (...) {
        // "commit or rollback"
        data_allocator::destroy(new_start, new_finish);
        data_allocator::deallocate(new_start, len);
        throw;
    }

    data_allocator::destroy(begin(), end());
    deallocate();

    _start = new_start;
    _finish = new_finish;
    _end_of_storage = new_start + len;

}

template<class T, class Allocator>
void
vector<T, Allocator>::strict_reallocate_copy(size_type insert_count) {
    const size_type old_size = size();
    
    iterator new_start = data_allocator::allocate(insert_count);
    iterator new_finish = new_start;

    if (insert_count < old_size) {
        new_finish = uninitialized_copy(_start, _start + insert_count, new_start);
    }
    else {
        new_finish = uninitialized_copy(_start, _finish, new_start);
    }

    destroy_and_deallocate();

    _start = new_start;
    _finish = new_finish;
    _end_of_storage = new_start + insert_count;

    return _start;
}

template<class T, class Allocator>
void 
vector<T, Allocator>::destroy_and_deallocate() {
    if (capacity() != 0) {
        data_allocator::destroy(_start, _finish);
        data_allocator::deallocate(_start, capacity());
    }
}

/************************************/
/************ operators *************/
/************************************/

template<class T, class Allocator>
bool 
vector<T, Allocator>::operator==(const _Self& other) const 
{
    if (size() != other.size())
        return false;
    iterator ptr1 = _start;
    iterator ptr2 = other._start;
    size_type len = size();
    for (int i = 0; i < len; ++i)
        if (*ptr1 != *ptr2)
            return false;
    return true;
}

/************************************/
/***************iterators************/
/************************************/
template<class T, class Allocator>
vector<T, Allocator>&
vector<T, Allocator>::operator=(const vector<T, Allocator>& other) {
    cout << "enter operator=" << endl;
    if (this == &other)
        return *this;
}

/************************************/
/***********element access***********/
/************************************/
template<class T, class Allocator>
typename vector<T, Allocator>::reference
vector<T, Allocator>::at(size_type pos) {
    if (pos >= size()) {
        throw std::out_of_range("Out Of Range");
    }
    return *(_start + pos);
}

template<class T, class Allocator>
typename vector<T, Allocator>::const_reference
vector<T, Allocator>::at(size_type pos) const {
    if (pos >= size()) {
        throw std::out_of_range("Out Of Range");
    }
    return *(_start + pos);
}

/************************************/
/*************iterators**************/
/************************************/




/************************************/
/**************capacity**************/
/************************************/

/* if capacity is greater or equal to \new_cap, new storage is allocated
   otherwise the method does nothing */
template<class T, class Allocator>
void 
vector<T, Allocator>::reserve(size_type new_cap) {
    if (new_cap > max_size()) {
        throw std::length_error("new_cap is bigger than the maximum capacity");
    }
    if (new_cap <= capacity())
        return;
    reallocate(new_cap);
}

/* request the removal of unused capacity */
template<class T, class Allocator>
void 
vector<T, Allocator>::shrink_to_fit() {
    if (size() == capacity())
        return;
    size_type len = size();
    iterator new_start = data_allocator::allocate(len);
    iterator new_finish = new_start;

    try {
        new_finish = uninitialized_copy(_start, _finish, new_start);
    }
    catch (...) {
        // "commit or rollback"
        data_allocator::destroy(new_start, new_finish);
        data_allocator::deallocate(new_start, len);
        throw;
    }

    data_allocator::destroy(begin(), end());
    deallocate();

    _start = new_start;
    _finish = new_finish;
    _end_of_storage = new_start + len;
}

template<class T, class Allocator>
typename vector<T, Allocator>::size_type
vector<T, Allocator>::max_size() const {
    return (std::numeric_limits<size_t>::max)();
}



/************************************/
/*************modifiers**************/
/************************************/

/* erase all vertices in vector, but don't change capacity */
template<class T, class Allocator>
void
vector<T, Allocator>::clear() {
    erase(begin(), end());
}

template<class T, class Allocator>
typename vector<T, Allocator>::iterator 
vector<T,Allocator>::insert(iterator pos, const T& value) {
    return __insert_aux(pos, (size_type)1, value);
}

template<class T, class Allocator>
typename vector<T, Allocator>::iterator
vector<T,Allocator>::insert(iterator pos, size_type count, const T& value) {
    return __insert_aux(pos, count, value);
}

template<class T, class Allocator>
template<class InputIterator>
typename vector<T, Allocator>::iterator
vector<T, Allocator>::insert(iterator pos, InputIterator first, InputIterator last) {
    return __insert_aux_template(pos, 
           first, last, typename std::is_integral<InputIterator>::type());
}

template<class T, class Allocator>
template<class InputIterator>
typename vector<T, Allocator>::iterator
vector<T, Allocator>::__insert_aux_template(iterator pos, InputIterator first, InputIterator last, std::true_type) {
    return __insert_aux(pos, (size_type)first, (T)last);
}

template<class T, class Allocator>
template<class InputIterator>
typename vector<T, Allocator>::iterator
vector<T, Allocator>::__insert_aux_template(iterator pos, InputIterator first, InputIterator last, std::false_type) {
    return __insert_aux(pos, first, last);
}


template<class T, class Allocator>
typename vector<T, Allocator>::iterator
vector<T, Allocator>::__insert_aux(iterator position, size_type count, const T& value) {

    if (count <= 0) return position;

    size_type remain_storage = size_type(_end_of_storage - _finish);
    size_type index = static_cast<size_type>(position-_start);

    if (remain_storage < count) {
        /* we should allocate more memory to store new elements */
        reallocate(count);
        position = _start+index;
    }

    T value_copy = value;
    iterator old_finish = _finish;
    size_type later_elems = size_type(_finish - position);

    if (later_elems > count) {
        uninitialized_copy(_finish - count, _finish, _finish);
        _finish += count;
        copy_backward(position, old_finish - (later_elems-count), old_finish);
        fill(position, position + count, value_copy);
    }
    else {
        uninitialized_fill_n(_finish, count - later_elems, value_copy);
        _finish += count - later_elems;
        uninitialized_copy(position, old_finish, _finish);
        _finish += later_elems;
        fill(position, old_finish, value_copy);
    }

    return position;
}

template<class T, class Allocator>
template<class InputIt>
typename vector<T, Allocator>::iterator
vector<T, Allocator>::__insert_aux(iterator position, InputIt first, InputIt last) {

    if (first == last)
        return position;

    difference_type location_need = distance(first, last);
    difference_type location_left = _end_of_storage - _finish;

    size_type index = static_cast<size_type>(position-_start);

    if (location_need >= location_left) {

        reallocate(size() + location_need);
        position = _start + index;
    }

    if (_finish - position <= location_need) {
        /* the [position, _finish] will be moved after _finish */
        iterator temp_start = uninitialized_copy(first + (_finish - position), last, _finish);
        uninitialized_copy(position, _finish, temp_start);
        copy(first, first + (_finish - position), position);
    }
    else {
        uninitialized_copy(_finish - location_need, _finish, _finish);
        copy_backward(position, _finish - location_need, _finish);
        copy(first, first + (_finish - position), position);
    }

    _finish += location_need;

    return position;
}

template<class T, class Allocator>
void 
vector<T, Allocator>::push_back(const T& value) {
    if (_finish == _end_of_storage) {
        reallocate(1);
    }    
    data_allocator::construct(_finish, value);
    ++_finish;
}

template<class T, class Allocator>
void
vector<T, Allocator>::pop_back() {
    --_finish;
    data_allocator::destroy(_finish);
}

template<class T, class Allocator>
typename vector<T, Allocator>::iterator 
vector<T, Allocator>::erase(vector<T, Allocator>::iterator position)
{
    if (position + 1 != end())
        copy(position + 1, _finish, position);
    --_finish;
    data_allocator::destroy(_finish);
    return position;
}

template<class T, class Allocator>
typename vector<T, Allocator>::iterator 
vector<T, Allocator>::erase(iterator first, iterator last) {
    iterator i = copy(last, _finish, first);
    data_allocator::destroy(i, _finish);
    _finish = _finish - (last - first);
    return first;
}

template<class T, class Allocator>
void 
vector<T, Allocator>::resize(size_type new_size, const T& value)
{
    if (new_size < size())
        erase(begin() + new_size, end());
    else
        insert(end(), new_size - size(), value);
}

template<class T, class Allocator>
void 
vector<T, Allocator>::resize(size_type new_size)
{
    resize(new_size, T());
}

template<class T, class Allocator>
void
vector<T, Allocator>::swap(vector& other) {
    if (other == *this)
        return;
    ustl::swap(other._finish, _finish);
    ustl::swap(other._start, _start);
    ustl::swap(other._end_of_storage, _end_of_storage);
}

}// end of namespace ustl

