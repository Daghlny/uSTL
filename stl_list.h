
#pragma once

namespace ustl {

template<class T>
struct __list_node {

    T data;
    __list_node<T> *prev;
    __list_node<T> *next;

    __list_node(const T& value): data(value){}
};

template<class T>
struct __list_iterator : public iterator<bidirectional_iterator_tag, T> {
    __list_node<T>* node_ptr;  
    explicit __list_iterator(__list_node<T>* _ptr):node_ptr(_ptr){}

    __list_iterator<T>& operator++(){
        node_ptr = node_ptr->next;
        return *this;
    }

    __list_iterator<T> operator++(int) {
        __list_iterator<T> res = *this;
        ++*this;
        return res;
    }

    __list_iterator<T>& operator--(){
        node_ptr = node_ptr->prev;
        return *this;
    }

    __list_iterator<T> operator--(int) {
        __list_iterator<T> res = *this;
        --*this;
        return res;
    }

    T& operator*(){
        return node_ptr->data;
    }

    T* operator->(){
        return &(operator*());
    }
};

}

