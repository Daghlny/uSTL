
#pragma once

#include <cassert>
#include <iostream>
#include <type_traits>
#include "stl_list.h"

namespace ustl {

template<class T>
__list_iterator<T>& 
__list_iterator<T>::operator++(){
    node_ptr = node_ptr->next;
    return *this;
}

template<class T>
__list_iterator<T>
__list_iterator<T>::operator++(int){
    __list_iterator<T> ret = *this;
    node_ptr = node_ptr->next;
    return ret;
}

template<class T>
__list_iterator<T>&
__list_iterator<T>::operator--(){
    node_ptr = node_ptr->prev;
    return *this;
}

template<class T>
__list_iterator<T>
__list_iterator<T>::operator--(int){
    __list_iterator<T> ret = *this;
    node_ptr = node_ptr->prev;
    return ret;
}

template<class T>
T&
__list_iterator<T>::operator*(){
    return node_ptr->data;
}

template<class T>
T*
__list_iterator<T>::operator->(){
    return &(node_ptr->data);
}

template<class T>
bool
__list_iterator<T>::operator==(const _self& other) const {
    return node_ptr == other.node_ptr;
}

template<class T>
bool
__list_iterator<T>::operator!=(const _self& other) const {
    return node_ptr != other.node_ptr;
}

}

/***********************************************
 *                ustl::list                   *
 ***********************************************/

namespace ustl{

template<class T, class Allocator>
typename list<T, Allocator>::pointer
list<T, Allocator>::new_node(const T& value)
{
    node_amount++;
    return data_allocator::new_value(value);
}

template<class T, class Allocator>
void
list<T, Allocator>::delete_node(iterator pos) 
{
    node_amount--;
    data_allocator::delete_value(pos.node_ptr);
}

/*************************************************/
/******************* Basic ***********************/
/*************************************************/

/************ constructors ************/

template<class T, class Allocator>
list<T, Allocator>::list(): node_amount(0)
{
    tail.node_ptr = head.node_ptr = new_node();
}

template<class T, class Allocator>
list<T, Allocator>::list(size_type count, 
                         const T& value):
                         node_amount(0)
{
    tail.node_ptr = head.node_ptr = new_node();

    size_type cnt = 0;
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
    }
    curr->next = tail.node_ptr;
    tail.node_ptr->prev = curr;
}

template<class T, class Allocator>
template<class InputIt>
list<T, Allocator>::list(InputIt first, InputIt last): node_amount(0)
{
    head.node_ptr = tail.node_ptr = new_node();
    insert(tail, first, last);
}

template<class T, class Allocator>
list<T, Allocator>::list(const list<T>& other):node_amount(0)
{
    head.node_ptr = tail.node_ptr = new_node();
    insert(head, other.cbegin(), other.cend());
}

/******************* destructor *****************/

template<class T, class Allocator>
list<T, Allocator>::~list() {
    while (head != tail) {
        delete_node(head++);
    }
    delete_node(head);
}

/********************************************/
/*************  Element access **************/
/********************************************/
template<class T, class Allocator>
typename list<T, Allocator>::reference
list<T, Allocator>::front(){
    assert(node_amount > 1);
    return head.node_ptr->data;
}

template<class T, class Allocator>
typename list<T, Allocator>::reference
list<T, Allocator>::back(){
    assert(node_amount > 1);
    return tail.node_ptr->prev->data;
}


/********************************************/
/**************** Iterators *****************/
/********************************************/
template<class T, class Allocator>
typename list<T, Allocator>::iterator
list<T, Allocator>::begin() {
    return head;
}

template<class T, class Allocator>
typename list<T, Allocator>::iterator
list<T, Allocator>::end() {
    return tail;
}

template<class T, class Allocator>
typename list<T, Allocator>::const_iterator
list<T, Allocator>::cbegin() const {
    return const_iterator(head.node_ptr);
}

template<class T, class Allocator>
typename list<T, Allocator>::const_iterator
list<T, Allocator>::cend() const {
    return const_iterator(tail.node_ptr);
}

/********************************************/
/**************** Capacity ******************/
/********************************************/

template<class T, class Allocator> 
bool
list<T, Allocator>::empty()
{
    return node_amount == 1;
}

template<class T, class Allocator>
typename list<T, Allocator>::size_type
list<T, Allocator>::size(){
    return node_amount-1;
}

template<class T, class Allocator>
typename list<T, Allocator>::size_type
list<T, Allocator>::max_size() {
    return Allocator::max_size();
}

/********************************************/
/**************** Modifiers******************/
/********************************************/

/*
 * erases a node from list
 * consider if the node is begin and assert that the node isn't end
 */
template<class T, class Allocator>
typename list<T, Allocator>::iterator
list<T, Allocator>::__erase(iterator pos)
{
    assert(pos != tail);
    if (pos == head) {
        head++;
    }
    pointer curr = pos.node_ptr;
    curr->prev->next = curr->next;
    curr->next->prev = curr->prev;
    delete_node(pos++);
    return pos;
}

template<class T, class Allocator>
typename list<T, Allocator>::iterator
list<T, Allocator>::__erase(iterator first, iterator last)
{
    while (first != last) {
        __erase(first);
    }
    return last;
}

template<class T, class Allocator>
typename list<T, Allocator>::iterator
list<T, Allocator>::__insert(iterator pos, const T& value) 
{
    pointer insert_node = new_node(value);
    pointer curr_node = pos.node_ptr;

    if ( node_amount == 2 && pos == head ) {
        curr_node->prev = insert_node;
        insert_node->prev = curr_node;
        curr_node->next = insert_node;
        insert_node->next = curr_node;
        head--;
        return iterator(insert_node);
    }

    curr_node->prev->next = insert_node;
    insert_node->prev = curr_node->prev;
    curr_node->prev = insert_node;
    insert_node->next = curr_node;

    if (pos == head)
        head--;

    return iterator(insert_node);
}

template<class T, class Allocator>
typename list<T, Allocator>::iterator
list<T, Allocator>::__insert(iterator pos, size_type count, const T& value)
{
    int cnt = 0;
    pointer first_node = pos.node_ptr->prev;
    for(; cnt < count; ++cnt) {
        __insert(pos, value);
    }
    return iterator(first_node->next);
}

template<class T, class Allocator>
template<class InputIt>
typename list<T, Allocator>::iterator
list<T, Allocator>::__insert_template(iterator pos, InputIt first, InputIt last, std::true_type) {
   return __insert(pos, (size_type)first, T(last));
}

template<class T, class Allocator>
template<class InputIt>
typename list<T, Allocator>::iterator
list<T, Allocator>::__insert_template(iterator pos, InputIt first, InputIt last, std::false_type) {
   return __insert_aux(pos, first, last);
}

template<class T, class Allocator>
template<class InputIt>
typename list<T, Allocator>::iterator
list<T, Allocator>::__insert_aux(iterator pos, InputIt first, InputIt last)
{
    pointer first_node = pos.node_ptr->prev;
    while (first != last)
        __insert(pos, *(first++));
    return iterator(first_node->next);
}

template<class T, class Allocator>
template<class InputIt>
typename list<T, Allocator>::iterator
list<T, Allocator>::__insert(iterator pos, InputIt first, InputIt last)
{
    return __insert_template(pos, 
           first, last, 
           typename std::is_integral<InputIt>::type());
}

template<class T, class Allocator>
void
list<T, Allocator>::clear(){
    __erase(head, tail);
}

template<class T, class Allocator>
typename list<T, Allocator>::iterator
list<T, Allocator>::erase(iterator pos)
{
    return __erase(pos);
}

template<class T, class Allocator>
typename list<T, Allocator>::iterator
list<T, Allocator>::erase(iterator first, iterator last)
{
    return __erase(first, last);
}

template<class T, class Allocator>
typename list<T, Allocator>::iterator
list<T, Allocator>::insert(iterator pos, const T& value) 
{
    return __insert(pos, value);
}

template<class T, class Allocator>
typename list<T, Allocator>::iterator
list<T, Allocator>::insert(iterator pos, size_type count, const T& value)
{
    return __insert(pos, count, value);
}

template<class T, class Allocator>
template<class InputIt>
typename list<T, Allocator>::iterator
list<T, Allocator>::insert(iterator pos, InputIt first, InputIt last)
{
    return __insert(pos, first, last);
}

}
