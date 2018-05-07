
#pragma once

#include <cstddef>
#include <type_traits>
#include <iostream>

#include "stl_iterator.h"
#include "stl_allocator.h"
#include "stl_simple_allocator.h"

namespace ustl {

/*
 * initialize _M_map with \num_elements elements
 * the initialized size of \_M_map is at least 8 defined by macro __USTL_DEQUE_INIT_MAP_SIZE
 */
template<class T, class Allocator>
void
__deque_base<T, Allocator>::_M_initialize_M_map(size_t num_elements) 
{
    size_type num_nodes = (num_elements / __deque_buf_size(sizeof(T)) + 1);
    M_map_size = ustl::max((size_t)__USTL_DEQUE_INIT_MAP_SIZE, size_t(num_nodes + 2 ));
    M_map = map_allocator.new_value(M_map_size, NULL);

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
__deque_base<T, Allocator>::_M_create_nodes(map_pointer nstart, map_pointer nfinish) 
{
    map_pointer cur;
    for (cur = nstart; cur != nfinish; ++cur)
        *cur = elt_allocator.new_value( __deque_buf_size(sizeof(T)), T() );
}

/*
 * destroies all elements in the M_map range [nstart, nfinish)
 */
template<class T, class Allocator>
void
__deque_base<T, Allocator>::_M_destroy_nodes(map_pointer nstart, map_pointer nfinish)
{
    map_pointer cur;
    for (cur = nstart; cur != nfinish; ++cur)
    {
        size_t cnt = 0;
        for (cnt = 0; cnt < __deque_buf_size(sizeof(T)), ++cnt)
            elt_allocator.delete_value(*cur+cnt);
    }
}

}

