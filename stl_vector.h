#pragma once

#include <stdexcept> // for out-of-range
#include <limits>    // for numeric_limits

#include "stl_allocator.h"
#include "stl_uninitialized.h"
#include "stl_reverseiterator.h"
#include "stl_algorithm.imph.h"


namespace ustl {

template<class T, class Allocator = allocator<T> >
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
    typedef allocator<T> data_allocator;
    
    iterator _start;
    iterator _finish;
    iterator _end_of_storage;

    void __insert_aux(iterator position, const T& value);
    void __insert_aux(iterator position, size_type count, const T& value);
    template<class InputIt> 
    void __insert_aux(iterator pos, InputIt first, InputIt last);

    void reallocate(size_type insert_count);


    void fill_initialize(size_type n, const T& value) {
        _start = allocate_and_fill(n, value);
        _finish = _start + n;
        _end_of_storage = _finish;
    }

public:
    /*** Constructors ***/
    vector() : _start(0), _finish(0), _end_of_storage(0) {}
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
        destroy(_start, _finish);
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
    size_type size() const { return size_type(cend() - cbegin()); }
    size_type capacity() const { return size_type(_end_of_storage - cbegin()); }
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

    /* Memory Conduction */
    iterator allocate_and_fill(size_type n, const T& value) {
        iterator result = data_allocator::allocate(n);
        uninitialized_fill_n(result, n, value);
        return result;
    }

    void strict_reallocate_copy(size_type insert_count);
    void destroy_and_deallocate();
    void deallocate() {
        data_allocator::deallocate(_start);
    }

};

}


namespace ustl {

/* 
 * reallocate the memory of vector, the default size is 2*old_size 
 * but the new size will not smaller than \insert_count + old_size
 */
template<class T, class Allocator>
void vector<T, Allocator>::reallocate(size_type insert_count) {

    const size_type old_size = size();
    size_type len = old_size != 0 ? 2 * old_size : 1;
    len = len > (insert_count + size()) ? len : insert_count + size();

    iterator new_start = data_allocator::allocate(len);
    iterator new_finish = new_start;

    try {
        new_finish = uninitialized_copy(_start, _finish, new_start);
    }
    catch (...) {
        // "commit or rollback"
        destroy(new_start, new_finish);
        data_allocator::deallocate(new_start, len);
        throw;
    }

    destroy(begin(), end());
    deallocate();

    _start = new_start;
    _finish = new_finish;
    _end_of_storage = new_start + len;

}

template<class T, class Allocator>
void
vector<T, Allocator>::strict_reallocate_copy(size_type insert_count) {
    const size_type old_size = size();
    
    iterator new_start = data_allocator::allocate(insert_count);
    iterator new_finish = new_start;

    if (insert_count < old_size) {
        new_finish = uninitialized_copy(_start, _start + insert_count, new_start);
    }
    else {
        new_finish = uninitialized_copy(_start, _finish, new_start);
    }

    destroy_and_deallocate();

    _start = new_start;
    _finish = new_finish;
    _end_of_storage = new_start + insert_count;

    return _start;
}

template<class T, class Allocator>
void 
vector<T, Allocator>::destroy_and_deallocate() {
    if (capacity() != 0) {
        data_allocator::destroy(_start, _finish);
        data_allocator::deallocate(_start, capacity());
    }
}



/************************************/
/***************iterators************/
/************************************/
template<class T, class Allocator>
vector<T, Allocator>&
vector<T, Allocator>::operator=(const vector<T, Allocator>& other) {
    if (this == &other)
        return *this;
    
}

/************************************/
/***********element access***********/
/************************************/
template<class T, class Allocator>
typename vector<T, Allocator>::reference
vector<T, Allocator>::at(size_type pos) {
    if (pos >= size()) {
        throw std::out_of_range("Out Of Range");
    }
    return *(_start + pos);
}

template<class T, class Allocator>
typename vector<T, Allocator>::const_reference
vector<T, Allocator>::at(size_type pos) const {
    if (pos >= size()) {
        throw std::out_of_range("Out Of Range");
    }
    return *(_start + pos);
}

/************************************/
/*************iterators**************/
/************************************/




/************************************/
/**************capacity**************/
/************************************/

/* if capacity is greater or equal to \new_cap, new storage is allocated
   otherwise the method does nothing */
template<class T, class Allocator>
void 
vector<T, Allocator>::reserve(size_type new_cap) {
    if (new_cap > max_size()) {
        throw std::length_error("new_cap is bigger than the maximum capacity");
    }
    if (new_cap <= capacity())
        return;
    reallocate(new_cap);
}

/* request the removal of unused capacity */
template<class T, class Allocator>
void 
vector<T, Allocator>::shrink_to_fit() {
    if (size() == capacity())
        return;
    size_type len = size();
    iterator new_start = data_allocator::allocate(len);
    iterator new_finish = new_start;

    try {
        new_finish = uninitialized_copy(_start, _finish, new_start);
    }
    catch (...) {
        // "commit or rollback"
        destroy(new_start, new_finish);
        data_allocator::deallocate(new_start, len);
        throw;
    }

    destroy(begin(), end());
    deallocate();

    _start = new_start;
    _finish = new_finish;
    _end_of_storage = new_start + len;
}

template<class T, class Allocator>
typename vector<T, Allocator>::size_type
vector<T, Allocator>::max_size() const {
    return (std::numeric_limits<size_t>::max)();
}



/************************************/
/*************modifiers**************/
/************************************/

/* erase all vertices in vector, but don't change capacity */
template<class T, class Allocator>
void
vector<T, Allocator>::clear() {
    erase(begin(), end());
}

template<class T, class Allocator>
typename vector<T, Allocator>::iterator 
vector<T,Allocator>::insert(iterator pos, const T& value) {
    return __insert_aux(pos, value);
}

template<class T, class Allocator>
void
vector<T,Allocator>::insert(iterator pos, size_type count, const T& value) {
    __insert_aux(pos, count, value);
}

template<class T, class Allocator>
template<class InputIterator>
void 
vector<T, Allocator>::insert(iterator pos, InputIterator first, InputIterator last) {
    __insert_aux(pos, first, last);
}

template<class T, class Allocator>
void 
vector<T, Allocator>::__insert_aux(iterator position, const T& value) {

    if (_finish == _end_of_storage) {
        /* if have enough memory */
        data_allocator::construct(_finish, *(_finish - 1));
        ++_finish;
        T value_copy = value;
        copy_backward(position, _finish - 2, _finish - 1);
        *position = value_copy;
    }
    else {
        const size_type old_size = size();
        const size_type len = old_size != 0 ? 2 * old_size : 1;

        iterator new_start = data_allocator::allocate(len);
        iterator new_finish = new_start;

        try {
            new_finish = uninitialized_copy(_start, position, new_start);
            data_allocator::construct(new_finish, value);
            +new_finish;
            new_finish = uninitialized_copy(position, _finish, new_finish);
        }
        catch (...) {
            // "commit or rollback"
            destroy(new_start, new_finish);
            data_allocator::deallocate(new_start, len);
            throw;
        }

        destroy(begin(), end());
        deallocate();

        _start = new_start;
        _finish = new_finish;
        _end_of_storage = new_start + len;
    }
}

template<class T, class Allocator>
void
vector<T, Allocator>::__insert_aux(iterator position, size_type count, const T& value) {

    if (count <= 0) return;

    size_type remain_storage = size_type(_end_of_storage - _finish);

    if (remain_storage < count) {
        /* we should allocate more memory to store new elements */
        size_type len = size() + count;
        reallocate(len);
    }

    T value_copy = value;
    iterator old_finish = _finish;
    size_type later_elems = size_type(_finish - position);

    if (later_elems > count) {
        uninitialized_copy(_finish - count, _finish, _finish);
        _finish += count;
        //TODO: the second parameter maybe wrong
        copy_backward(position, old_finish - (later_elems-count), old_finish);
        fill(position, position + count, value_copy);
    }
    else {
        uninitialized_fill_n(_finish, count - later_elems, value_copy);
        _finish += count - later_elems;
        uninitialized_copy(position, old_finish, _finish);
        _finish += later_elems;
        fill(position, old_finish, value_copy);
    }
}

template<class T, class Allocator>
template<class InputIt>
void
vector<T, Allocator>::__insert_aux(iterator position, InputIt first, InputIt last) {

    difference_type location_need = distance(first, last);
    difference_type location_left = _end_of_storage - _finish;

    if (location_need >= location_left) {
        reallocate(size() + location_need);
    }

    if (_finish - position <= location_need) {
        /* the [position, _finish] will be moved after _finish */
        iterator temp_start = uninitialized_copy(first + (_finish - position), last, _finish);
        uninitialized_copy(position, _finish, temp_start);
        ustl::copy(first, first + (_finish - position), position);
    }
    else {
        uninitialized_copy(_finish - location_need, _finish, _finish);
        ustl::copy_backward(position, _finish - location_need, _finish);
        ustl::copy(first, first + (_finish - position), position);
    }
    _finish += location_need;
}

template<class T, class Allocator>
void 
vector<T, Allocator>::push_back(const T& value) {
    if (_finish == _end_of_storage) {
        reallocate(1);
    }    
    data_allocator::construct(_finish, value);
    ++_finish;
}

template<class T, class Allocator>
void
vector<T, Allocator>::pop_back() {
    --_finish;
    destroy(_finish);
}

template<class T, class Allocator>
typename vector<T, Allocator>::iterator 
vector<T, Allocator>::erase(vector<T, Allocator>::iterator position)
{
    if (position + 1 != end())
        copy(position + 1, _finish, position);
    --_finish;
    data_allocator::destroy(_finish);
    return position;
}

template<class T, class Allocator>
typename vector<T, Allocator>::iterator 
vector<T, Allocator>::erase(iterator first, iterator last) {
    iterator i = copy(last, _finish, first);
    destroy(i, _finish);
    _finish = _finish - (last - first);
    return first;
}

template<class T, class Allocator>
void 
vector<T, Allocator>::resize(size_type new_size, const T& value)
{
    if (new_size < size())
        erase(begin() + new_size, end());
    else
        insert(end(), new_size - size(), value);
}

template<class T, class Allocator>
void 
vector<T, Allocator>::resize(size_type new_size)
{
    resize(new_size, T());
}

template<class T, class Allocator>
void
vector<T, Allocator>::swap(vector& other) {
    if (other == *this)
        return;
    ustl::swap(other._finish, _finish);
    ustl::swap(other._start, _start);
    ustl::swap(other._end_of_storage, _end_of_storage);
}


}
