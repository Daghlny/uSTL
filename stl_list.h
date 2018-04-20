
#pragma once

#include <cstddef>
#include <type_traits>

#include "stl_iterator.h"
#include "stl_allocator.h"
#include "stl_simple_allocator.h"

namespace ustl {

/* 
 * the node of ustl::list, it is the bidirectional linkedlist
 */
template<class T>
struct __list_node {

    T data;
    __list_node<T> *prev;
    __list_node<T> *next;

    __list_node(const T& value): data(value){}
};

/*
 * the iterator of ustl::list, it is bidirectional_iterator
 */
template<class T>
struct __list_iterator : public iterator<bidirectional_iterator_tag, T> {

    typedef __list_iterator<T> _self;

    __list_node<T>* node_ptr;  

    __list_iterator(__list_node<T>* _ptr):node_ptr(_ptr){}
    __list_iterator():node_ptr(NULL){}

    __list_iterator<T>&  operator++();
    __list_iterator<T>   operator++(int); 
    __list_iterator<T>&  operator--();
    __list_iterator<T>   operator--(int);

    const __list_iterator<T>&  operator++() const ;
    const __list_iterator<T>   operator++(int) const ; 
    const __list_iterator<T>&  operator--() const ;
    const __list_iterator<T>   operator--(int) const;

    const T& operator*() const;
    const T* operator->() const;

    T& operator*();
    T* operator->();

    bool operator==(const _self& other) const;
    bool operator!=(const _self& other) const;

};


template<typename T>
struct __list_const_iterator : public iterator<bidirectional_iterator_tag, T> {

    typedef __list_const_iterator<T>  _self;

    typedef T value_type;
    typedef __list_node<T> node;
    typedef __list_iterator<T> iterator;
    typedef const T* pointer;
    typedef const T& reference;

    __list_node<T> * node_ptr;

    __list_const_iterator() : node_ptr() {}
    explicit __list_const_iterator(node* _x) : node_ptr(_x) {}
    //explicit __list_const_iterator(__list_node<T>* const _x) : node_ptr(_x) {}
    __list_const_iterator(const iterator& _x) : node_ptr(_x.node_ptr) {}

    reference operator*() const {
        // in G++ version, this statement is 
        return node_ptr->data;
    }
    
    pointer   operator->() const {
        return &(node_ptr->data);
    }

    _self& operator++() {
        node_ptr = node_ptr->next;
        return *this;
    }

    _self operator++(int) {
        _self temp = *this;
        node_ptr = node_ptr->next;
        return temp;
    }

    _self& operator--() {
        node_ptr = node_ptr->prev;
        return *this;
    }

    _self operator--(int) {
        _self temp = *this;
        node_ptr = node_ptr->prev;
        return temp;
    }

    bool operator==(const _self& _x) const {
        return node_ptr == _x.node_ptr;
    }

    bool operator!=(const _self& _x) const {
        return node_ptr != _x.node_ptr;
    }

};


template<class T, class Allocator = allocator<__list_node<T> > >
class list{
    public:
        /* type alias */
        typedef list<T, Allocator>                  _self;
        typedef size_t                              size_type;
        typedef T                                   value_type;
        typedef std::ptrdiff_t                      difference_type; 
        typedef value_type&                         reference;
        typedef const value_type&                   const_reference;
        typedef __list_iterator<T>                  iterator;
        typedef __list_const_iterator<T>      const_iterator;
        typedef reverse_iterator_t<iterator>        reverse_iterator;
        typedef reverse_iterator_t<const_iterator>  const_reverse_iterator;

        typedef __list_node<T>                      list_node;
        typedef list_node*                          pointer;
        typedef __list_node<const T>                const_pointer;

        typedef simple_allocator<list_node, Allocator> data_allocator;

        /* Basic */
        list();
        explicit list(size_type count, const T& value = T());
        template<class InputIt> 
        list(InputIt first, InputIt last);
        list(const list<T>& other);

        ~list();

        _self& operator=(const _self& other);
        void assign(size_type count, const T& value);
        template<class InputIt>
        void assign(InputIt first, InputIt last);

        /* Element access */
        reference front();
        reference back();

        /* Iterators */
        iterator begin();
        const_iterator cbegin() const;
        iterator end();
        const_iterator cend() const;
        reverse_iterator rbegin();
        const_reverse_iterator crbegin();
        reverse_iterator rend();
        const_reverse_iterator crend();

        /* Capacity */
        bool empty();
        size_type size();
        size_type max_size();

        /* Modifiers */
        void clear();

        iterator insert(iterator pos, const T& value);
        iterator insert(iterator pos, size_type count, const T& value);
        template<class InputIt>
        iterator insert(iterator pos, InputIt first, InputIt last);

        iterator erase(iterator pos);
        iterator erase(iterator first, iterator last);

        void push_back(const T& value);
        void pop_back();
        void push_front(const T& value);
        void pop_front();
        void resize(size_type count);
        void resize(size_type count, const value_type& value);
        void swap(_self& other);

        /* Operations */
        void merge(_self& other);
        template<class Compare> void merge(_self& other);

        void splice(const_iterator pos, _self& other);
        void splice(const_iterator pos, _self& other, const_iterator it);
        void splice(const_iterator pos, _self& other, const_iterator first, const_iterator last);

        void remove(const T& value);
        template<class UnaryPredicate>
        void remove_if(UnaryPredicate p);

        void reverse();

        void unique();
        template<class BinaryPredicate> void unique(BinaryPredicate p);

        void sort();
        template<class Compare> void sort(Compare comp);

    protected:
        pointer  _M_node;

    private:
        /* Memory */
        pointer new_node(const T& value = T());
        void delete_node(iterator pos);
        void delete_node(pointer dnode);

        /* Modifiers */
        iterator __erase(iterator pos);
        iterator __erase(iterator first, iterator last);

        iterator __insert(iterator pos, const T& value);
        iterator __insert(iterator pos, size_type count, const T& value);
        template<class InputIt>
        iterator __insert(iterator pos, InputIt first, InputIt last);
        
        template<class InputIt>
        iterator __insert_template(iterator pos, InputIt first, InputIt last, std::true_type);
        template<class InputIt>
        iterator __insert_template(iterator pos, InputIt first, InputIt last, std::false_type);
        template<class InputIt>
        iterator __insert_aux(iterator pos, InputIt first, InputIt last);

        void __transfer(const_iterator pos, const_iterator first, const_iterator last);

};

}

#include "stl_list.imph.h"


