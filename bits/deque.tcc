
#pragma once

#include <cstddef>
#include <type_traits>
#include <iostream>

#include "stl_iterator.h"
#include "stl_allocator.h"
#include "stl_simple_allocator.h"

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

template<class T, class Allocator>
typename deque<T, Allocator>::iterator
deque<T, Allocator>::__insert(iterator pos, const T& _x)
{
    size_type dis_to_start = distance(M_start, pos);
    size_type dis_to_finish = distance(pos, M_finish);

    if (dis_to_start < dis_to_finish) {
        // we should move [M_start, pos] forward
        __move_begin_forward(pos, 1);
        *pos = _x;
    } else {
        // we should move [pos, M_finish) backward
        __move_end_backward(pos, 1);
        *pos = _x;
    }
}

/*************************************************
 * *************** private functions *************
 * ***********************************************/

/*
 * reallocates M_map to add __nodes_to_add, the reallocation may not be occured if there is enough memory of M_map
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
 * reserve 
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

