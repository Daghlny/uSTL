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

    void __insert_aux(iterator position, const T& value);
    void __insert_aux(iterator position, size_type count, const T& value);
    template<class InputIt> 
    void __insert_aux(iterator pos, InputIt first, InputIt last);

    void reallocate(size_type insert_count);
    void deallocate();


    void fill_initialize(size_type n, const T& value) {
        _start = allocate_and_fill(n, value);
        _finish = _start + n;
        _end_of_storage = finish;
    }

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
    reference front() { return *begin(); }
    reference back() { return *(end() - 1); }
    reference at(size_type pos);
    const_reference at(size_type pos) const;

    T* data();
    const T* data() const;

    /* operators */
    reference operator[](size_type n) {
        return *(begin() + n);
    }

    bool operator==(const vector<T, Allocator>& other) const {
        if (size() != other.size())
            return false;
        iterator ptr1 = _start;
        iterator ptr2 = other._start;
        size_type len = size();
        for (int i = 0; i < len; ++i)
            if (*ptr1 != *ptr2)
                return false;
        return true;
    }

    bool operator!=(const vector<T, Allocator>& other) const {
        return !(*this == other);
    }

    vector<T, Allocator>& operator=(const vector<T, Allocator>& other);


    /* iterators */
    iterator begin() { return _start; }
    iterator end() { return _finish; }

    const_iterator cbegin() const { return _start; }
    const_iterator cend() const { return _finish; }

    reverse_iterator rbegin();
    reverse_iterator rend();

    const_reverse_iterator crbegin() const;
    const_reverse_iterator crend() const;


    /* capacity */
    size_type size() const { return size_type(end() - begin()); }
    size_type capacity() const { return size_type(_end_of_storage - begin()); }
    bool empty() const { return begin() == end(); }
    void reserve(size_type new_cap);
    void shrink_to_fit();
    size_type max_size() const;

    /* modifiers */
    void clear();

    iterator insert(iterator pos, const T& value);
    void insert(iterator pos, size_type count, const T& value);
    template<class InputIt> void insert(iterator pos, InputIt first, InputIt last);

    void push_back(const T& value);
    void pop_back();
    iterator erase(iterator position);
    iterator erase(iterator first, iterator last);
    void resize(size_type new_size, const T& value);
    void resize(size_type new_size);
    void swap(vector& other);

protected:
    iterator allocate_and_fill(size_type n, const T& value) {
        iterator result = data_allocator::allocate(n);
        uninitialized_fill_n(result, n, value);
        return result;
    }

};

}
