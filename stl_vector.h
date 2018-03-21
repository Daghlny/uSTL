#pragma once

#include "stl_alloc.h"
#include "stl_uninitialized.h"
#include "stl_reverseiterator.h"

namespace ustl {

template<class T, class Allocator = alloc>
class vector {
public:
    typedef T                              value_type;
    typedef value_type*                    pointer;
    typedef value_type*                    iterator;
    typedef const value_type*              const_iterator;
    typedef reverse_iterator_t<T*>         reverse_iterator;
    typedef reverse_iterator_t<const T*>   const_reverse_iterator;
    typedef value_type&                    reference;
    typedef const value_type&              const_reference;
    typedef size_t                         size_type;
    typedef ptrdiff_t                      difference_type;

protected:
    typedef simple_alloc<value_type, Allocator> data_allocator;
    
    iterator _start;
    iterator _finish;
    iterator _end_of_storage;

    void insert_aux(iterator position, const T& x);
    void deallocate();
    void fill_initialize(size_type n, const T& value);

public:
    /*** Constructors ***/
    vector() : start(0), finish(0), end_of_storage(0) {}
    vector(size_type n, const T& value) {
        fill_initialize(n, value);
    }
    vector(int n, const T& value) {
        fill_initialize(n, value);
    }
    vector(long n, const T& value) {
        fill_initialize(n, value);
    }
    explicit vector(size_type n) {
        fill_initialize(n, T());
    }

    /*** Destructors ***/
    ~vector() {
        destroy(start, finish);
        deallocate();
    }

    /* element access */
    reference front();
    reference back();
    reference at(size_type pos);
    const_reference at(size_type pos) const;
    T* data();
    const T* data() const;
    reference operator[](size_type n) {
        return *(begin() + n);
    }


    /* iterators */
    iterator begin();
    iterator end();

    const_iterator cbegin() const;
    const_iterator cend() const;

    reverse_iterator rbegin();
    reverse_iterator rend();

    const_reverse_iterator crbegin() const;
    const_reverse_iterator crend() const;


    /* capacity */
    size_type size() const;
    size_type capacity() const;
    bool empty() const;
    void reserve(size_type new_cap);
    void shrink_to_fit();
    size_type max_size() const;

    /* modifiers */
    void clear();
    iterator insert(iterator pos, const T& value);
    void push_back(const T& value);
    void pop_back();
    iterator erase(iterator position);
    void resize(size_type new_size, const T& value);
    void resize(size_type new_size);
    void swap(vector& other);

protected:
    iterator allocate_and_fill(size_type n, const T& value);
};

}
