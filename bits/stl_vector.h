#pragma once

#include <stdexcept> // for out-of-range
#include <limits>    // for numeric_limits
#include <type_traits> // for is_integral
#include <iostream>  // for debug
#include <initializer_list> // for initializer_list constructor

#include "stl_allocator.h"
#include "stl_uninitialized.h"
#include "algorithm.tcc"


namespace ustl {

template<class T, class Allocator = ustl::allocator<T> >
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
    //typedef simple_alloc<value_type, Allocator> data_allocator;
    typedef Allocator data_allocator;
    typedef vector<T, Allocator> _Self;
    
    iterator _start;
    iterator _finish;
    iterator _end_of_storage;

    iterator __insert_aux(iterator position, const T& value);
    iterator __insert_aux(iterator position, size_type count, const T& value);
    template<class InputIt> 
    iterator __insert_aux(iterator pos, InputIt first, InputIt last);
    template<class InputIterator>
    iterator __insert_aux_template(iterator pos, InputIterator first, InputIterator last, std::true_type);
    template<class InputIterator>
    iterator __insert_aux_template(iterator pos, InputIterator first, InputIterator last, std::false_type);




public:
    /*** constructors ***/
    vector() : _start(0), _finish(0), _end_of_storage(0) {}
    vector(size_type n, const T& value) { fill_initialize(n, value); }
    explicit vector(size_type n) {  fill_initialize(n, T()); }
    vector(const _Self& vec);

#if __cplusplus >= 201103L
    /*** C++ 11 ***/
    vector(std::initializer_list<T> initL);
    vector(_Self&& rhs);
#endif

    /*** destructors ***/
    ~vector() {
        data_allocator::destroy(_start, _finish);
        deallocate();
    }

    /* element access */
    reference front() { return *begin(); }
    reference back() { return *(end() - 1); }
    const_reference front() const { return *cbegin(); }
    const_reference back() const { return *(cend() - 1); }
    reference at(size_type pos);
    const_reference at(size_type pos) const;

    T* data();
    const T* data() const;

    /* operators */
    reference operator[](size_type n) { return *(begin() + n); }

    bool operator==(const _Self& other) const;
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
    size_type size() const { return size_type(cend() - cbegin()); }
    size_type capacity() const { return size_type(_end_of_storage - cbegin()); }
    bool empty() const { return begin() == end(); }
    void reserve(size_type new_cap);
    void shrink_to_fit();
    size_type max_size() const;

    /* modifiers */
    void clear();

    iterator insert(iterator pos, const T& value);
    iterator insert(iterator pos, size_type count, const T& value);
    template<class inputit> iterator insert(iterator pos, inputit first, inputit last);

    void push_back(const T& value);
    void pop_back();
    iterator erase(iterator position);
    iterator erase(iterator first, iterator last);
    void resize(size_type new_size, const T& value);
    void resize(size_type new_size);
    void swap(vector& other);

protected:

    /* memory conduction */

    void fill_initialize(size_type n, const T& value) {
        _start = allocate_and_fill(n, value);
        _finish = _start + n;
        _end_of_storage = _finish;
    }

    iterator allocate_and_fill(size_type n, const T& value) {
        iterator result = data_allocator::allocate(n*sizeof(T));
        uninitialized_fill_n(result, n, value);
        return result;
    }

    void strict_reallocate_copy(size_type insert_count);

    void destroy_and_deallocate();

    void reallocate(size_type insert_count);

    void deallocate() {
        data_allocator::deallocate(_start);
    }

};

}


