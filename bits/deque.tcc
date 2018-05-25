
#pragma once

#include <stdexcept> // for out-of-range
#include <cstddef>
#include <type_traits>
#include <iostream>

#include "stl_iterator.h"
#include "stl_allocator.h"
#include "stl_simple_allocator.h"
#include "stl_uninitialized.h"

#include "algorithm.tcc"

namespace ustl {

/*******************************************
 *                 deque
 *******************************************/

/****** constructors & destructors *********/
template<class T, class Allocator>
deque<T, Allocator>::deque(): M_map(NULL), M_map_size(0) {
    _M_initialize_M_map(0);
}

template<class T, class Allocator>
deque<T, Allocator>::deque(size_t _num_elements)
{
    _M_initialize_M_map(_num_elements);
}

template<class T, class Allocator>
template<class InputIt>
deque<T, Allocator>::deque(InputIt first, InputIt last)
{
    //TODO: finish this
    size_type __n = ustl::distance(first, last);
    _M_initialize_M_map(0);
    insert(begin(), first, last);
}

template<class T, class Allocator>
deque<T, Allocator>::deque(size_type count, const T& value)
{
    _M_initialize_M_map(0);
    insert(begin(), count, value);
}

template<class T, class Allocator>
deque<T, Allocator>::deque(const _Self& other)
{
    size_type length = other.size();
    _M_initialize_M_map(0);
    std::cout << size() << " " << length << std::endl;
    insert(begin(), other.cbegin(), other.cend());
}

template<class T, class Allocator>
deque<T, Allocator>::~deque() 
{
    if (M_map != NULL) {
        _M_destroy_nodes(M_start._m_node, M_finish._m_node + 1);
    }
    _M_deallocate_M_map();
}

/********* copy control ***********/
template<class T, class Allocator>
typename deque<T, Allocator>::_Self&
deque<T, Allocator>::operator=(const _Self& _rhs)
{
    clear();
    this->insert(begin(), _rhs.cbegin(), _rhs.cend());
    return *this;
}

template<class T, class Allocator>
void
deque<T, Allocator>::assign(size_type _count, const value_type& _x)
{
    clear();
    this->insert(begin(), size_type(_count), _x);
}

template<class T, class Allocator>
template<class InputIt>
void
deque<T, Allocator>::assign(InputIt _first, InputIt _last)
{
    clear();
    this->insert(begin(), _first, _last);
}

/********* Elements Access *********/
//reference at(size_type index);
//reference operator[](size_type index);
//reference front();
//reference back();

template<class T, class Allocator>
typename  deque<T, Allocator>::reference
deque<T, Allocator>::at(size_type _idx)
{
    if (_idx < 0 || _idx >= size())
        throw std::out_of_range("Out of Range(size:" + size() + ")");
    return this->operator[](_idx);
}

template<class T, class Allocator>
typename deque<T, Allocator>::reference
deque<T, Allocator>::operator[](size_type _idx)
{
    return *(M_start+_idx);
}

template<class T, class Allocator>
typename deque<T, Allocator>::reference
deque<T, Allocator>::front()
{
    return *M_start;
}

template<class T, class Allocator>
typename deque<T, Allocator>::reference
deque<T, Allocator>::back()
{
    return *(M_finish-1);
}

template<class T, class Allocator>
typename deque<T, Allocator>::const_reference
deque<T, Allocator>::front() const
{
    return *M_start;
}

template<class T, class Allocator>
typename deque<T, Allocator>::const_reference
deque<T, Allocator>::back() const
{
    return *(M_finish-1);
}

/********* Iterators *********/

template<class T, class Allocator>
typename deque<T, Allocator>::iterator
deque<T, Allocator>::begin()
{
    return M_start;
}

template<class T, class Allocator>
typename deque<T, Allocator>::iterator
deque<T, Allocator>::end()
{
    return M_finish;
}

template<class T, class Allocator>
typename deque<T, Allocator>::const_iterator
deque<T, Allocator>::cbegin() const
{
    return const_iterator(M_start._m_cur, M_start._m_node);
}

template<class T, class Allocator>
typename deque<T, Allocator>::const_iterator
deque<T, Allocator>::cend() const
{
    return const_iterator(M_finish._m_cur, M_finish._m_node);
}

template<class T, class Allocator>
typename deque<T, Allocator>::reverse_iterator
deque<T, Allocator>::rbegin()
{
    return reverse_iterator(end());
}

template<class T, class Allocator>
typename deque<T, Allocator>::reverse_iterator
deque<T, Allocator>::rend()
{
    return reverse_iterator(begin());
}

template<class T, class Allocator>
typename deque<T, Allocator>::const_reverse_iterator
deque<T, Allocator>::crbegin() const
{
    return const_reverse_iterator(cend());
}

template<class T, class Allocator>
typename deque<T, Allocator>::const_reverse_iterator
deque<T, Allocator>::crend() const
{
    return const_reverse_iterator(cbegin());
}

/********* Capacity *********/

template<class T, class Allocator>
void        
deque<T, Allocator>::shrink_to_fit()
{
    size_type map_length = M_finish._m_node - M_start._m_node + 1;
    map_pointer new_map = map_allocator::allocate(map_length);
    ustl::copy(M_start._m_node, M_finish._m_node, new_map);
    map_allocator::deallocate(M_map);
    M_map = new_map;
    M_start._m_node = M_map;
    M_finish._m_node = M_map+map_length-1;
    M_map_size = map_length;
}


/********* Modifiers ********/

template<class T, class Allocator>
typename deque<T, Allocator>::iterator
deque<T, Allocator>::insert(iterator _pos, const value_type& _x) 
{
    return _M_insert(_pos, _x);
}

template<class T, class Allocator>
void
deque<T, Allocator>::insert(iterator _pos, size_type _count, const value_type& _x)
{
    return _M_insert_aux(_pos, _count, _x);
}

template<class T, class Allocator>
template<class InputIt>
void
deque<T, Allocator>::insert(iterator _pos, InputIt _first, InputIt _last)
{
    return _M_insert(_pos, _first, _last, typename std::is_integral<InputIt>::type());
}


template<class T, class Allocator>
typename deque<T, Allocator>::iterator
deque<T, Allocator>::erase(iterator _pos) 
{
    return _M_erase(_pos);
}

template<class T, class Allocator>
typename deque<T, Allocator>::iterator
deque<T, Allocator>::erase(iterator _first, iterator _last)
{
    return _M_erase(_first, _last);
}

template<class T, class Allocator>
void
deque<T, Allocator>::clear()
{
    _M_erase_at_end(begin());
}

template<class T, class Allocator>
void 
deque<T, Allocator>::push_back(const value_type& _x)
{
    iterator new_finish = _M_reserve_elements_at_back(1);
    *M_finish = _x;
    M_finish = new_finish;
}

template<class T, class Allocator>
void
deque<T, Allocator>::pop_back()
{
    elt_allocator::destroy(end()-1, end());
    --M_finish;
}

template<class T, class Allocator>
void 
deque<T, Allocator>::push_front(const value_type& _x)
{
    M_start = _M_reserve_elements_at_front(1);
    *M_start = _x;
}

template<class T, class Allocator>
void
deque<T, Allocator>::pop_front()
{
    elt_allocator::destroy(M_start, M_start+1);
    ++M_start;
}

template<class T, class Allocator>
void
deque<T, Allocator>::resize(size_type _count)
{
    this->resize(_count, T());
}

template<class T, class Allocator>
void
deque<T, Allocator>::resize(size_type _count, const value_type& _x)
{
    size_type len = size();
    if (len < _count)
    {
        iterator new_finish = _M_reserve_elements_at_back(_count-len);
        ustl::uninitialized_fill_n(M_finish, _count-len, _x);
        M_finish = new_finish;
    } else if (len > _count) {
        _M_erase_at_end(M_start+_count);
    }
}

template<class T, class Allocator>
void
deque<T, Allocator>::swap(_Self& _other)
{
    ustl::swap(this->M_map, _other.M_map);
    ustl::swap(this->M_map_size, _other.M_map_size);
    ustl::swap(this->M_start, _other.M_start);
    ustl::swap(this->M_finish, _other.M_finish);
}

/********* Memory Conductions *********/

/*
 * initialize _M_map with \num_elements elements
 * the initialized size of \_M_map is at least 8 defined by macro __USTL_DEQUE_INIT_MAP_SIZE
 * not only allocate, but also construct
 */
template<class T, class Allocator>
void
deque<T, Allocator>::_M_initialize_M_map(size_t num_elements) 
{
    size_type num_nodes = (num_elements / __deque_buf_size(sizeof(T)) + 1);
    M_map_size = ustl::max((size_t)__USTL_DEQUE_INIT_MAP_SIZE, size_t(num_nodes + 2 ));
    M_map = map_allocator::allocate(M_map_size);

    map_pointer nstart = M_map + (M_map_size - num_nodes) / 2;
    map_pointer nfinish = nstart + num_nodes;

    // in G++, here needs a try-catch block
    _M_create_nodes(nstart, nfinish);

    M_start._m_set_node(nstart);
    M_finish._m_set_node(nfinish - 1);
    M_start._m_cur = M_start._m_first;
    M_finish._m_cur = M_finish._m_first + (num_elements % __deque_buf_size(sizeof(T)));
}

template<class T, class Allocator>
void
deque<T, Allocator>::_M_deallocate_M_map() 
{
    map_allocator::deallocate(M_map);
}

template<class T, class Allocator>
void
deque<T, Allocator>::_M_create_nodes(map_pointer nstart, map_pointer nfinish) 
{
    map_pointer cur;
    for (cur = nstart; cur != nfinish; ++cur) {
        *cur = elt_allocator::allocate( __deque_buf_size(sizeof(T)) );
        pointer it = *cur;
        while (it != *cur+_S_buffer_size())
            elt_allocator::construct(it++, T());
    }
}

/*
 * destroies all elements in the M_map range [nstart, nfinish)
 * and deallocate the memory
 */
template<class T, class Allocator>
void
deque<T, Allocator>::_M_destroy_nodes(map_pointer nstart, map_pointer nfinish)
{
    map_pointer cur;
    for (cur = nstart; cur != nfinish; ++cur)
    {
        size_t cnt = 0;
        elt_allocator::destroy(*cur, *cur+__deque_buf_size(sizeof(T)));
        elt_allocator::deallocate(*cur);
    }
}

template<class T, class Allocator>
void
deque<T, Allocator>::_M_destroy_data(iterator _first, iterator _last)
{
    for (map_pointer _node = _first._m_node; _node != _last._m_node; ++_node)
        elt_allocator::destroy(*_node, *_node+_S_buffer_size());

    if (_first._m_node != _last._m_node) {
        elt_allocator::destroy(_first._m_cur, _first._m_last);
        elt_allocator::destroy(_last._m_first, _last._m_cur);
    } else {
        elt_allocator::destroy(_first._m_cur, _last._m_cur);
    }
}

/******** inner insert **********/

// can't call _M_insert_aux(iterator, size_type, value_type) direcly, because this function will return a iterator
// and _M_insert_aux may invalidate iterators
// this function is in deque.tcc:622
template<class T, class Allocator>
typename deque<T, Allocator>::iterator
deque<T, Allocator>::_M_insert(iterator _pos, const T& _x)
{
    size_type index = _pos - begin();
    if (_pos == begin())
        push_front(_x);
    else if (_pos == end())
        push_back(_x);
    else {
        _M_insert_aux(_pos, 1, _x);
    }
    return begin()+index;
}

template<class T, class Allocator>
template<class InputIt>
void
deque<T, Allocator>::_M_range_insert_aux(iterator pos, InputIt _first, InputIt _last) 
{
    const size_type _n = ustl::distance(_first, _last);
    if (pos._m_cur == M_start._m_cur) {
        iterator new_start = _M_reserve_elements_at_front(_n);
        uninitialized_copy_forward(_first, _last, new_start);
        M_start = new_start;
    } else if (pos._m_cur == M_finish._m_cur) {
        iterator new_finish = _M_reserve_elements_at_back(_n);
        uninitialized_copy_forward(_first, _last, M_finish);
        M_finish = new_finish;
    } else 
        _M_insert_aux(pos, _first, _last, _n);
}

template<class T, class Allocator>
template<class InputIt>
void
deque<T, Allocator>::_M_insert_aux(iterator _pos, InputIt _first, InputIt _last, size_type _n) 
{
    const difference_type elems_before = _pos - M_start;
    const size_type       length      = size();
    if (size_type(elems_before) < length/2 ) {
        // move the front part
        iterator new_start = _M_reserve_elements_at_front(_n);
        iterator old_start = M_start;
        
        if (elems_before >= difference_type(_n))
        {
            iterator start_n = M_start + difference_type(_n);
            ustl::uninitialized_copy_forward(old_start, start_n, new_start);
            M_start = new_start;
            ustl::move(start_n, _pos, old_start);
            ustl::copy(_first, _last, _pos - difference_type(_n));
        } else {
            InputIt mid = _first;
            ustl::advance(mid, difference_type(_n) - elems_before);
            // G++ combines these two invocation to a __uninitialized_move_copy
            ustl::uninitialized_copy_forward(old_start, _pos, new_start);
            ustl::uninitialized_copy_forward(_first, mid, new_start + difference_type(elems_before));
            M_start = new_start;
            ustl::copy(mid, _last, old_start);
        }
    } else {
        // move the back part
        iterator new_finish = _M_reserve_elements_at_back(_n);
        iterator old_finish = M_finish;
        const difference_type elems_after = length - elems_before;

        // Attention: here is > , and above is >=
        if (elems_after > difference_type(_n)) 
        {
            iterator finish_n = _pos + difference_type(_n);
            ustl::uninitialized_copy_forward(finish_n, old_finish, old_finish);
            ustl::move(_pos, finish_n, finish_n);
            M_finish = new_finish;
            ustl::copy(_first, _last, _pos);
        } else {
            InputIt mid = _first;
            ustl::advance(mid, elems_after);
            ustl::uninitialized_copy_backward(_pos, old_finish, new_finish);
            ustl::uninitialized_copy_forward(mid, _last, old_finish);
            M_finish = new_finish;
            ustl::copy(_first, mid, _pos);
        }
    }
}

//_M_insert(_pos, _count, _x, typename std::is_integral<InputIt>::type());

template<class T, class Allocator>
template<class InputIt>
void
deque<T, Allocator>::_M_insert(iterator _pos, InputIt _first, InputIt _last, std::true_type)
{
    _M_insert_aux(_pos, size_type(_first), _last);
}

template<class T, class Allocator>
template<class InputIt>
void
deque<T, Allocator>::_M_insert(iterator _pos, InputIt _first, InputIt _last, std::false_type)
{
    _M_range_insert_aux(_pos, _first, _last);
}

template<class T, class Allocator>
void
deque<T, Allocator>::_M_insert_aux(iterator pos, size_type _n, const value_type& _x)
{
    const difference_type elems_before = pos - M_start;
    const size_type length = this->size();
    value_type x_copy = _x;

    if (elems_before < difference_type(length/2)) {
        // insert elements at front
        iterator new_start = _M_reserve_elements_at_front(_n);
        iterator old_start = M_start;
        if (elems_before >= difference_type(_n)) {
            /*    new_start --- old_start --- __start_n ------ pos */
            iterator __start_n = M_start + difference_type(_n);
            ustl::uninitialized_copy_forward(old_start, __start_n, new_start);
            M_start = new_start;
            ustl::move(__start_n, pos, old_start);
            ustl::fill(pos - difference_type(_n), pos, x_copy);
        } else {
            const size_type vacancies = _n - elems_before;
            iterator vacancies_it = ustl::uninitialized_copy_forward(old_start, pos, new_start);
            M_start = new_start;
            ustl::uninitialized_fill_n(vacancies_it, vacancies, x_copy);
            ustl::fill(old_start, pos, x_copy);
        }
    } else {
        // insert elements at back
        iterator new_finish = _M_reserve_elements_at_back(_n);
        iterator old_finish = M_finish;
        const difference_type elems_after = length - elems_before;
        if (elems_after > difference_type(_n)) {
            iterator __finish_n = M_finish - difference_type(_n);
            ustl::uninitialized_copy_forward(__finish_n, old_finish, old_finish);
            M_finish = new_finish;
            ustl::move_backward(pos, __finish_n, old_finish);
            ustl::fill(pos, pos+difference_type(_n), x_copy);
        } else {
            const difference_type vacancies = _n - elems_after;
            ustl::uninitialized_copy_forward(pos, old_finish, old_finish + vacancies);
            ustl::uninitialized_fill_n(old_finish, vacancies, x_copy);
            ustl::fill(pos, old_finish, x_copy);
            M_finish = new_finish;
        }
    }
}
/*************************************************
 * *************** private functions *************
 * ***********************************************/

/*
 * reallocates M_map to add __nodes_to_add nodes, the reallocation may not be occured if there is enough memory of M_map
 * __add_at_front indicates that whether adding elements at the front
 */
template<class T, class Allocator>
void
deque<T, Allocator>::_M_reallocate_map(size_type __nodes_to_add, bool __add_at_front)
{
    const size_type __old_num_nodes = M_finish._m_node - M_start._m_node+1;
    const size_type __new_num_nodes = __old_num_nodes + __nodes_to_add;

    map_pointer __new_nstart;

    if ( M_map_size > 2*__new_num_nodes ) {
        __new_nstart = M_map+(M_map_size-__new_num_nodes)/2+(__add_at_front ? __nodes_to_add : 0);
        if (__new_nstart < M_start._m_node) 
            ustl::copy(M_start._m_node, M_finish._m_node+1, __new_nstart);
        else
            ustl::copy_backward(M_start._m_node, M_finish._m_node+1, __new_nstart+__old_num_nodes);
    } else {
        size_type __new_map_size = M_map_size + ustl::max(M_map_size, __nodes_to_add)+2;
        map_pointer __new_map = map_allocator::allocate(__new_map_size);
        __new_nstart = __new_map + (__new_map_size - __new_num_nodes) / 2 + (__add_at_front ? __nodes_to_add : 0);
        ustl::copy(M_start._m_node, M_finish._m_node+1, __new_nstart);
        map_allocator::deallocate(M_map);
        M_map = __new_map;
        M_map_size = __new_map_size;
    }

    M_start._m_set_node(__new_nstart);
    M_finish._m_set_node(__new_nstart+__old_num_nodes-1);
}

/*
 * reserve the space at front or back of M_map
 * @_M_reserve_elements_at_front: return the iterator pointing to the first position forwards _n 
 * @_M_reserve_elements_at_back : return the iterator pointing to the last position backwards _n
 *
 * The calling path:
 * _M_reserve_elements_at_front -> _M_new_elements_at_front -> _M_reserve_map_at_front -> _M_reallocate_map
 * _M_reserve_elements_at_back  -> _M_new_elements_at_back  -> _M_reserve_map_at_back  -> _M_reallocate_map
 *
 * these methods will only allocate the space to store, won't construct the objects
 */
template<class T, class Allocator>
typename deque<T, Allocator>::iterator
deque<T, Allocator>::_M_reserve_elements_at_front(size_type _n)
{
    const size_type __vacancies = M_start._m_cur - M_start._m_first;
    if (_n > __vacancies) 
        _M_new_elements_at_front(_n - __vacancies);
    return M_start - difference_type(_n);
}

template<class T, class Allocator>
typename deque<T, Allocator>::iterator
deque<T, Allocator>::_M_reserve_elements_at_back(size_type _n)
{
    const size_type __vacancies = M_finish._m_last - M_finish._m_cur - 1;
    if (_n > __vacancies)
        _M_new_elements_at_back(_n - __vacancies);
    return M_finish + difference_type(_n);
}

/*
 * make sure M_map has space for new nodes
 * may invalidate M_map pointer
 */
template<class T, class Allocator>
void
deque<T, Allocator>::_M_reserve_map_at_back(size_type __nodes_to_add)
{
    if (__nodes_to_add+1 > size_type(M_map_size - (M_finish._m_node - M_map))) {
        _M_reallocate_map(__nodes_to_add, false);
    }
}

template<class T, class Allocator>
void
deque<T, Allocator>::_M_reserve_map_at_front(size_type __nodes_to_add)
{
    if (__nodes_to_add > size_type(M_start._m_node - M_map))
        _M_reallocate_map(__nodes_to_add, true);
}


/*
 * make enough new elments 
 */
template<class T, class Allocator>
void
deque<T, Allocator>::_M_new_elements_at_front(size_type __new_elems)
{
    const size_type __new_nodes =  ((__new_elems + _S_buffer_size() - 1) / _S_buffer_size());
    _M_reserve_map_at_front(__new_nodes);
    size_type __i;
    for (__i = 1; __i <= __new_nodes; ++__i) {
        *(M_start._m_node - __i) = elt_allocator::allocate(_S_buffer_size());
    }
}

template<class T, class Allocator>
void
deque<T, Allocator>::_M_new_elements_at_back(size_type __new_elems)
{
    const size_type __new_nodes = ((__new_elems + _S_buffer_size() - 1) / _S_buffer_size());
    _M_reserve_map_at_back(__new_nodes);
    size_type __i;
    for (__i = 1; __i <= __new_nodes; ++__i)
        *(M_finish._m_node + __i) = elt_allocator::allocate(_S_buffer_size());
}


template<class T, class Allocator>
typename deque<T, Allocator>::iterator 
deque<T,Allocator>::_M_erase(iterator _pos)
{
    iterator next = _pos;
    ++next;
    size_type index = _pos - begin();
    if ( index < (size()>>1) ) 
    {
        // move the front part
        ustl::move_backward(begin(), _pos, next);
        pop_front();
    } else {
        // move the back part
        ustl::move(next, end(), _pos);
        pop_back();
    }
    return begin()+index;
}

template<class T, class Allocator>
typename deque<T, Allocator>::iterator
deque<T, Allocator>::_M_erase(iterator _first, iterator _last)
{
    const difference_type n = _last - _first;
    const difference_type elems_before = _first-begin();
    if (elems_before <= (size() - n)/2) {
        // move the front part
        ustl::move_backward(begin(), _first, _last);
        _M_erase_at_begin(begin()+n);
    } else {
        // move the back part
        ustl::move(_last, end(), _first);
        _M_erase_at_end(end()-n);
    }
    return begin()+elems_before;

}

/*
 * erases all elements in [begin(), _pos) or [_pos, end())
 * and updates M_start or M_finish
 */
template<class T, class Allocator>
void
deque<T, Allocator>::_M_erase_at_begin(iterator _pos)
{
    _M_destroy_data(begin(), _pos);
    _M_destroy_nodes(M_start._m_node, _pos._m_node);
    M_start = _pos;
}

template<class T, class Allocator>
void
deque<T, Allocator>::_M_erase_at_end(iterator _pos)
{
    _M_destroy_data(_pos, end());
    _M_destroy_nodes(_pos._m_node+1, M_finish._m_node);
    M_finish = _pos;
}

}

