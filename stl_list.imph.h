
#pragma once

#include <cassert>
#include <iostream>
#include <type_traits>

#include "stl_list.h"
#include "stl_algorithm.imph.h"

using std::cout;
using std::endl;

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
    pointer res = data_allocator::new_value(value);
    res->next = res;
    res->prev = res;
    return res;
}

template<class T, class Allocator>
void
list<T, Allocator>::delete_node(iterator pos) 
{
    data_allocator::delete_value(pos.node_ptr);
}

template<class T, class Allocator>
void
list<T, Allocator>::delete_node(pointer dnode) 
{
    if (dnode->prev != NULL && dnode->next != NULL) {
        dnode->prev->next = dnode->next;
        dnode->next->prev = dnode->prev;
        data_allocator::delete_value(dnode);
    } else {
        data_allocator::delete_value(dnode);
    }
}

/*************************************************/
/******************* Basic ***********************/
/*************************************************/

/************ constructors ************/

template<class T, class Allocator>
list<T, Allocator>::list()
{
    _M_node = new_node();
}

template<class T, class Allocator>
list<T, Allocator>::list(size_type count, 
                         const T& value)
{
    _M_node = new_node();

    size_type cnt = 0;
    pointer prev = _M_node, curr = NULL;
    for (; cnt < count; ++cnt) {
        curr = new_node(value);
        curr->prev = prev;
        prev->next = curr;
        prev = curr;
    }
    curr->next = _M_node;
    _M_node->prev = curr;
}

template<class T, class Allocator>
template<class InputIt>
list<T, Allocator>::list(InputIt first, InputIt last)
{
    _M_node = new_node();
    insert(end(), first, last);
}

template<class T, class Allocator>
list<T, Allocator>::list(const list<T>& other)
{
    _M_node = new_node();
    insert(end(), other.cbegin(), other.cend());
}

/******************* destructor *****************/

template<class T, class Allocator>
list<T, Allocator>::~list() {
    pointer node = _M_node->next;
    while (node != _M_node) {
        pointer tmp = node->next;
        delete_node(node);
        node = tmp;
    }
    delete_node(_M_node);
}

/************************************************/

template<class T, class Allocator>
typename list<T, Allocator>::_self&
list<T, Allocator>::operator=(const _self& other)
{
    assign(other.cbegin(), other.cend());
}

template<class T, class Allocator>
void
list<T, Allocator>::assign(size_type count, const T& value)
{
    clear();
    insert(begin(), (size_type)count, value);
}

template<class T, class Allocator>
template<class InputIt>
void
list<T, Allocator>::assign(InputIt first, InputIt last)
{
    clear();
    insert(begin(), first, last);
}

/********************************************/
/*************  Element access **************/
/********************************************/
template<class T, class Allocator>
typename list<T, Allocator>::reference
list<T, Allocator>::front(){
    assert(_M_node->next != _M_node);
    return _M_node->data;
}

template<class T, class Allocator>
typename list<T, Allocator>::reference
list<T, Allocator>::back(){
    assert(_M_node->next != _M_node);
    return _M_node->prev->data;
}


/********************************************/
/**************** Iterators *****************/
/********************************************/
template<class T, class Allocator>
typename list<T, Allocator>::iterator
list<T, Allocator>::begin() {
    return iterator(_M_node->next);
}

template<class T, class Allocator>
typename list<T, Allocator>::iterator
list<T, Allocator>::end() {
    return iterator(_M_node);
}

template<class T, class Allocator>
typename list<T, Allocator>::const_iterator
list<T, Allocator>::cbegin() const {
    return const_iterator(_M_node->next);
}

template<class T, class Allocator>
typename list<T, Allocator>::const_iterator
list<T, Allocator>::cend() const {
    return const_iterator(_M_node);
}

template<class T, class Allocator>
typename list<T, Allocator>::reverse_iterator
list<T, Allocator>::rbegin() {
    return reverse_iterator(end());
}

template<class T, class Allocator>
typename list<T, Allocator>::reverse_iterator
list<T, Allocator>::rend() {
    return reverse_iterator(begin());
}

/********************************************/
/**************** Capacity ******************/
/********************************************/

template<class T, class Allocator> 
bool
list<T, Allocator>::empty()
{
    return (_M_node->next == _M_node);
}


/*
 * return the amount of elements in ustl::list
 * due to the constant time complexity of splice(), this function needs
 * O(n) complexity.
 */

template<class T, class Allocator>
typename list<T, Allocator>::size_type
list<T, Allocator>::size(){
    return distance(begin(), end());
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
    assert(pos.node_ptr != _M_node);
    delete_node(pos++);
    return pos;
}

template<class T, class Allocator>
typename list<T, Allocator>::iterator
list<T, Allocator>::__erase(iterator first, iterator last)
{
    while (first != last) {
        first = __erase(first);
    }
    return last;
}

template<class T, class Allocator>
typename list<T, Allocator>::iterator
list<T, Allocator>::__insert(iterator pos, const T& value) 
{
    pointer insert_node = new_node(value);
    pointer next = pos.node_ptr, prev = pos.node_ptr->prev;

    insert_node->next = next;
    insert_node->prev = prev;
    next->prev = insert_node;
    prev->next = insert_node;

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
    __erase(begin(), end());
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

template<class T, class Allocator>
void
list<T, Allocator>::push_back(const T& value)
{
    insert(end(), value);
}

template<class T, class Allocator>
void
list<T, Allocator>::pop_back() 
{
    assert(_M_node->next != _M_node);
    iterator back_it(_M_node->prev);
    erase(back_it);
}

template<class T, class Allocator>
void
list<T, Allocator>::push_front(const T& value)
{
    insert(begin(), value);
}

template<class T, class Allocator>
void
list<T, Allocator>::pop_front()
{
    assert(_M_node != _M_node->next);
    erase(begin());
}

/*
 * resizes the container to contain \count elements
 * If the current size is less than \count
 * 1) additional default-inserted elements are appended
 * 2) additional copies of value are appended
 */
template<class T, class Allocator>
void
list<T, Allocator>::resize(size_type count)
{
    resize(count, T());
}

template<class T, class Allocator>
void 
list<T, Allocator>::resize(size_type count, const T& value)
{
    size_type node_amount = size();
    if (count <= node_amount)
        return ;
    insert(end(), (size_type)(count+1-node_amount), value);
}

template<class T, class Allocator>
void
list<T, Allocator>::swap(_self& other) 
{
    ustl::swap(_M_node, other._M_node);
}

/********************************************/
/****************Operations******************/
/********************************************/
template<class T, class Allocator>
void
list<T, Allocator>::__transfer(const_iterator pos, const_iterator first, const_iterator last)
{
    last.node_ptr->prev->next  = pos.node_ptr;
    first.node_ptr->prev->next = last.node_ptr;
    pos.node_ptr->prev->next   = first.node_ptr;

    pointer tmp = pos.node_ptr->prev;

    pos.node_ptr->prev = last.node_ptr->prev;
    last.node_ptr->prev = first.node_ptr->prev;
    first.node_ptr->prev = tmp;
}

template<class T, class Allocator>
template<class Compare>
void
list<T, Allocator>::merge(_self& other, Compare cmp)
{
    if (this != &other)
    {
        iterator _first1 = begin();
        iterator _first2 = other.begin();
        iterator _last1 = end();
        iterator _last2 = other.end();

        while (_first1 != _last1 && _first2 != _last2) {
            if (cmp(*_first2, *_first1)) {
                iterator _next = _first2;
                __transfer(_first1, _first2, ++_next);
                _first2 = _next;
            } else 
                ++_first1;
        }

        if (_first2 != _last2)
            __transfer(_last1, _first2, _last2);
    }
}

template<class T, class Allocator>
void
list<T, Allocator>::merge(_self& other)
{
    merge(other, ustl::less<T>());
}

template<class T, class Allocator>
void
list<T, Allocator>::splice(const_iterator pos, _self& other)
{
    if (!other.empty())
        __transfer(pos, other.begin(), other.end());
}

template<class T, class Allocator>
void
list<T, Allocator>::splice(const_iterator pos, _self& other, const_iterator it) 
{
    iterator __j = it._M_const_cast();
    ++__j;
    if (pos == it  || pos == __j) 
        return ;

    __transfer(pos, it, __j);
}

template<class T, class Allocator>
void
list<T, Allocator>::splice(const_iterator pos, _self& other, const_iterator first, const_iterator last) {
    if (!other.empty())
        __transfer(pos, first, last);
}

template<class T, class Allocator>
void
list<T, Allocator>::remove(const T& value)
{
    pointer node = _M_node->next;
    while (node != _M_node) {
        pointer tmp = node->next;
        if (node->data == value)
            delete_node(node);
        node = tmp;
    }
}

template<class T, class Allocator>
template<class UnaryPredicate>
void
list<T, Allocator>::remove_if(UnaryPredicate p)
{
    pointer node = _M_node->next;
    while (node != _M_node) {
        pointer tmp = node->next;
        if( p(node->data) )
            delete_node(node);
        node = tmp;
    }
}

template<class T, class Allocator>
void
list<T, Allocator>::reverse()
{
    pointer node = _M_node->next;
    while (node != _M_node) {
        ustl::swap(node->next, node->prev);
        node = node->prev;
    }
    ustl::swap(_M_node->next, _M_node->prev);
}

template<class T, class Allocator>
void
list<T, Allocator>::unique()
{
    pointer prev = _M_node->next, node = _M_node->next->next;
    if (prev == _M_node || node == _M_node)
        return;
    while ( node != _M_node ) {
        pointer tmp = node->next;
        if (node->data == prev->data) {
            delete_node(node);
        } else {
            prev = node;
        }
        node = tmp;
    }
}

template<class T, class Allocator>
template<class BinaryPredicate>
void
list<T, Allocator>::unique(BinaryPredicate p) 
{
    pointer prev = _M_node->next, node = _M_node->next->next;
    if (prev == _M_node || node == _M_node)
        return;
    while ( node != _M_node ) {
        pointer tmp = node->next;
        if (p(node->data, prev->data)) {
            delete_node(node);
        } else {
            prev = node;
        }
        node = tmp;
    }
}

/* 
 * The implementation is accepted by GNU STL
 * The simple idea is using non-recursive merge-sort
 */
template<class T, class Allocator>
template<class Compare>
void
list<T, Allocator>::sort(Compare cmp)
{
    if (this->_M_node->next != this->_M_node && 
        this->_M_node->next->next != this->_M_node)
    {
        // do nothing if there is only 0 or 1 length
        _self   __carry;
        _self   __tmp[64];
        _self*  __fill = &__tmp[0];
        _self*  __counter;

        do {
            __carry.splice(__carry.begin(), *this, begin());

            for (__counter = &__tmp[0]; 
                 __counter != __fill && !__counter->empty();
                 ++__counter)
            {
                __counter->merge(__carry, cmp);
                __carry.swap(*__counter);
            }
            __carry.swap(*__counter);

            if (__counter == __fill)
                ++__fill;
        }while( !empty() );

        for (__counter = &__tmp[1]; __counter != __fill; ++__counter) {
            __counter->merge(*(__counter - 1), cmp);
        }

        this->swap(* (__fill-1) );
    }
}

template<class T, class Allocator>
void
list<T, Allocator>::sort()
{
    this->sort(ustl::less<T>());
}



}

