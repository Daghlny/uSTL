
#pragma once

#include <cstddef>
#include <type_traits>

#include "stl_iterator.h"
#include "stl_allocator.h"
#include "stl_simple_allocator.h"
#include "algorithm.tcc"

namespace ustl{

#ifndef __USTL_DEQUE_BUF_SIZE
#define __USTL_DEQUE_BUF_SIZE 512
#endif

#ifndef __USTL_DEQUE_INIT_MAP_SIZE 
#define __USTL_DEQUE_INIT_MAP_SIZE 8
#endif

inline size_t __deque_buf_size(size_t __size) 
{
    return (__size < __USTL_DEQUE_BUF_SIZE ? size_t (__USTL_DEQUE_BUF_SIZE / __size) : size_t(1)); 
}

template<class T>
class __deque_iterator {
    public:
        typedef __deque_iterator<T> _Self;
        typedef T*                  elt_pointer; // element pointer
        typedef T**                 map_pointer; // M_map element pointer

        typedef __deque_iterator<T>         iterator;
        typedef __deque_iterator<const T>   const_iterator;
        
        /* for general iterator types */
        typedef ustl::random_access_iterator_tag    iterator_category;
        typedef T                                   value_type;
        typedef T*                                  pointer;
        typedef T&                                  reference;
        typedef size_t                              size_type;
        typedef ptrdiff_t                           difference_type;

        elt_pointer _m_cur;
        elt_pointer _m_first;
        elt_pointer _m_last;
        map_pointer _m_node;

        /* constructors */
        __deque_iterator() : _m_cur(), _m_first(), _m_last(), _m_node() {}
        __deque_iterator(elt_pointer x, map_pointer y) : 
            _m_cur(x), _m_first(y), _m_last(*y + __deque_buf_size(sizeof(T))), _m_node(y) {} 
        __deque_iterator(const _Self &x) : 
            _m_cur(x._m_cur), _m_first(x._m_first), _m_last(x._m_last), _m_node(x._m_node) {}

        iterator _const_cast() const { return iterator(_m_cur, _m_node); }

        reference operator*() { return *_m_cur; }
        pointer   operator->() { return _m_cur; }

        /* Operators */
        static size_t __buffer_size() { return __deque_buf_size(sizeof(T)); }
        static size_type _S_buffer_size() { return __deque_buf_size(sizeof(T)); }

        void _m_set_node(map_pointer new_node) 
        {
            _m_node = new_node;
            _m_first = *new_node;
            _m_last = _m_first + __deque_buf_size(sizeof(T));
        }

        _Self& operator++() 
        {
            ++_m_cur;
            if (_m_cur == _m_last) {
                _m_set_node(_m_node+1);
                _m_cur = _m_first;
            }
            return *this;
        }

        _Self operator++(int) 
        {
            _Self tmp = *this;
            ++(*this);
            return tmp;
        }

        _Self& operator--() 
        {
            --_m_cur;
            if (_m_cur < _m_first) {
                _m_set_node(_m_node-1);
                _m_cur = _m_last-1;
            }
            return *this;
        }

        _Self operator--(int)
        {
            _Self tmp = *this;
            --(*this);
            return tmp;
        }

        _Self& operator+=(difference_type _n) 
        {
            /* this implementation is complicated, for a concise version refer to G++'s codes */
            /*
            const difference_type curr_offset = _m_cur - _m_first;
            elt_pointer _tmp_new_pos = _m_cur + n;
            if (_tmp_new_pos < _m_last && _tmp_new_pos >= _m_first) {
                _m_cur = _tmp_new_pos;
            } else if (_tmp_new_pos < _m_first) {
                difference_type remain_elts = n - curr_offset;
                size_type skip_node_num = remain_elts / __buffer_size() + 1;
                size_type skip_elmt_num = remain_elts % __buffer_size();
                _m_set_node(_m_node - skip_node_num);
                _m_cur = _m_last - skip_elmt_num;
            } else {
                difference_type remain_elts = n - (_m_last-_m_cur);
                size_type skip_node_num = remain_elts / __buffer_size() + 1;
                size_type skip_elmt_num = remain_elts % __buffer_size();
                _m_set_node(_m_node + skip_node_num);
                _m_cur = _m_first + skip_elmt_num;
            }
            */
            const difference_type offset = _n + (_m_cur - _m_first);
            if (offset >= 0 && offset < difference_type(_S_buffer_size()))
                _m_cur += _n;
            else {
                const difference_type node_offset = offset > 0 ? 
                                                    offset / difference_type(_S_buffer_size()) :
                                                    -difference_type( (-offset-1) / _S_buffer_size() ) -1 ;
                _m_set_node(_m_node + node_offset);
                _m_cur = _m_first + (offset - node_offset * difference_type(_S_buffer_size()));
            }
            return *this;
        }

        _Self operator+(difference_type n) const 
        {
            _Self tmp = *this;
            return (tmp+=n);
        }

        _Self& operator-=(difference_type n) 
        {
            return (*this += (-n));
        }

        _Self operator-(difference_type n) const 
        {
            _Self tmp = *this;
            return (tmp-=n);
        }

        reference operator[](difference_type n) const 
        { return *(*this+n); }

        bool operator==(const _Self& rhs) {
            return _m_cur == rhs._m_cur;
        }

        bool operator!=(const _Self& rhs) {
            return !(*this == rhs);
        }
};

template<class T>
inline typename __deque_iterator<T>::difference_type
operator-(const __deque_iterator<T>& _x, const __deque_iterator<T>& _y)
{
    return typename __deque_iterator<T>::difference_type(__deque_iterator<T>::_S_buffer_size()) *
           (_x._m_node - _y._m_node - 1) + (_x._m_cur - _x._m_first) + (_y._m_last - _y._m_cur);
}


template<class T, class Allocator = ustl::allocator<T> >
class deque {
    public:
        typedef allocator<T*>  map_allocator;
        typedef allocator<T>   elt_allocator;

        typedef deque<T, Allocator>              _Self;

        typedef T**                              map_pointer;
        typedef T*                               pointer;
        typedef T&                               reference;
        typedef size_t                           size_type;
        typedef T                                value_type;
        typedef std::ptrdiff_t                   difference_type;

        typedef __deque_iterator<T>              iterator;

        /* constructors */
        deque();
        explicit deque(size_t _elements_num) ;
        template<class InputIt> deque(InputIt first, InputIt last);
        deque(size_type count, const T& value);

        /* destructor */
        ~deque();

        /* Memory conductions */

        void _M_insert_begin_nodes(size_t elements_num);

        /* element access */
        reference at(size_type index);
        reference operator[](size_type index);
        reference front();
        reference back();

        /* Iterators */
        iterator begin();
        iterator end();
        iterator cbegin();
        iterator cend();
        iterator rbegin();
        iterator rend();
        iterator crbegin();
        iterator crend();

        /* Capacity */
        bool        empty();
        size_t      size() { return M_map_size; }
        size_type   max_size();
        void        shrink_to_fit();

        /* modifiers */
        void  clear();

        iterator                        insert(iterator pos, const T& value);
        void                            insert(iterator pos, size_type count, const T& value);
        template<class InputIt> void    insert(iterator pos, InputIt first, InputIt last);

        iterator erase(iterator pos);
        iterator erase(iterator first, iterator last);

        void push_back(const T& value);
        void pop_back();
        void push_front(const T& value);
        void pop_front();

        void resize(size_type count);
        void resize(size_type count, const T& value);
        void swap(_Self& other);

    protected:
        T**    M_map;
        size_t M_map_size;
        iterator M_start;       // the first element's iterator 
        iterator M_finish;      // the last element's iterator

    private:
        /* Memory */
        static size_t _S_buffer_size() { return __deque_buf_size(sizeof(T)); }
        void _M_reallocate_map(size_type __nodes_to_add, bool __add_at_front);
        // these four functions are definited in g++ stl_deque.h
        iterator _M_reserve_elements_at_front(size_type _n);
        iterator _M_reserve_elements_at_back (size_type _n);
        void _M_reserve_map_at_front(size_type __nodes_to_add = 1);
        void _M_reserve_map_at_back (size_type __nodes_to_add = 1);
        // both functions are definited in g++ deque.tcc
        void _M_new_elements_at_front(size_type __new_elems);
        void _M_new_elements_at_back (size_type __new_elems);

        void _M_initialize_M_map(size_t num_elements); 
        void _M_deallocate_M_map();

        void _M_create_nodes (map_pointer nstart, map_pointer nfinish);
        void _M_destroy_nodes(map_pointer nstart, map_pointer nfinish);


        /* inner insert */
        iterator                        _M_insert(iterator pos, const T& _x);
        template<class InputIt> void    _M_insert(iterator pos, InputIt _first, InputIt _last, std::true_type);
        template<class InputIt> void    _M_insert(iterator pos, InputIt _first, InputIt _last, std::false_type);

        void _M_insert_aux(iterator pos, size_type _n, const value_type& _x);
        template<class InputIt> void _M_range_insert_aux(iterator pos, InputIt _first, InputIt _last);
        template<class InputIt> void _M_insert_aux(iterator _pos, InputIt _first, InputIt _last, size_type _n);

        /* inner capacity check */
        bool __touch_start(size_type _insert_n);
        bool __touch_finish(size_type _insert_n);

};


}

