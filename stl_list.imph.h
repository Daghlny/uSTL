
#pragma once

#include "stl_list.h"

namespace ustl{

template<class T, class Allocator>
typename list<T, Allocator>::pointer
list<T, Allocator>::new_node(const T& value)
{
    return data_allocator::new_value(value);
}

template<class T, class Allocator>
list<T, Allocator>::list(size_type count, 
                         const T& value)
{
    tail.node_ptr = head.node_ptr = new_node();

    size_type cnt = 0;
    node_amount = 0;
    pointer prev = NULL, curr = NULL;
    for (; cnt < count; ++cnt) {
        curr = new_node(value);
        curr->prev = prev;
        if (prev != NULL)
            prev->next = curr;
        else  {
            head.node_ptr = curr;
            curr->prev = tail.node_ptr;
        }
        prev = curr;
        node_amount += 1;
    }
    curr->next = tail.node_ptr;
    tail.node_ptr->prev = curr;
}

/*
 * TODO: finish this function
 */
template<class T, class Allocator>
list<T, Allocator>::~list() {

}

/********************************************/
/**************** Capacity ******************/
/********************************************/
template<class T, class Allocator>
typename list<T, Allocator>::size_type
list<T, Allocator>::size(){
    return node_amount;
}

}
