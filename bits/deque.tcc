
#pragma once

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
deque<T, Allocator>::deque(): M_map(NULL), M_map_size(0) {}

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
    _M_initialize_M_map(__n);
    insert(first, last);
}

template<class T, class Allocator>
deque<T, Allocator>::deque(size_type count, const T& value)
{
    _M_initialize_M_map(count);
    insert(count, value);
}

template<class T, class Allocator>
deque<T, Allocator>::~deque() 
{
    if (M_map != NULL) {
        _M_destroy_nodes(M_start._m_node, M_finish._m_node + 1);
    }
    _M_deallocate_M_map();
}

/********* Memory Conductions *********/

/*
 * initialize _M_map with \num_elements elements
 * the initialized size of \_M_map is at least 8 defined by macro __USTL_DEQUE_INIT_MAP_SIZE
 */
template<class T, class Allocator>
void
deque<T, Allocator>::_M_initialize_M_map(size_t num_elements) 
{
    size_type num_nodes = (num_elements / __deque_buf_size(sizeof(T)) + 1);
    M_map_size = ustl::max((size_t)__USTL_DEQUE_INIT_MAP_SIZE, size_t(num_nodes + 2 ));
    M_map = map_allocator::allocate(M_map_size);
    //M_map = map_allocator::new_value(M_map_size, NULL);

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
deque<T, Allocator>::_M_insert_begin_nodes(size_t elements_num)
{
    size_type num_nodes = (elements_num / __deque_buf_size(sizeof(T)) + 1);
    map_pointer new_M_map = map_allocator::allocate(num_nodes+M_map_size);
    copy(new_M_map+num_nodes, M_map, M_map+M_map_size);
    M_start._m_node = new_M_map+num_nodes;
    M_finish._m_node = new_M_map+num_nodes+M_map_size;
    map_allocator::deallocate(M_map);
    M_map = new_M_map;
    M_map_size = num_nodes+M_map_size;
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
        elt_allocator::construct(*cur, *cur+__deque_buf_size(sizeof(T)));
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

/******** inner insert **********/

// can't call _M_insert_aux(iterator, size_type, value_type) direcly, because this function will return a iterator
// and _M_insert_aux may invalidate iterators
// this function is in deque.tcc:622
template<class T, class Allocator>
typename deque<T, Allocator>::iterator
deque<T, Allocator>::__insert(iterator pos, const T& _x)
{

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

    M_start._M_set_node(__new_nstart);
    M_finish._M_set_node(__new_nstart+__old_num_nodes-1);
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

}

