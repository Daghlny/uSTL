
#pragma once

#include <cstddef>
#include <type_traits>

#include "stl_iterator.h"
#include "stl_allocator.h"
#include "stl_simple_allocator.h"

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
        __deque_iterator(_Self x) : 
            _m_cur(x._m_cur), _m_first(x._m_first), _m_last(x._m_last), _m_node(x._m_node) {}

        iterator _const_cast() const { return iterator(_m_cur, _m_node); }

        reference operator*() { return *_m_cur; }
        pointer   operator->() { return _m_cur; }

        /* Operators */
        static size_t __buffer_size() { return __deque_buf_size(sizeof(T)); }

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

        _Self& operator+=(difference_type n) 
        {
            /* this implementation is complicated, for a concise version refer to G++'s codes */
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


template<class T, class Allocator = ustl::allocator<T> >
class __deque_base {
    public:
        typedef simple_allocator< T*, ustl::allocator<T*> >  map_allocator;
        typedef simple_allocator< T, Allocator>              elt_allocator;

        typedef T**                              map_pointer;

        typedef __deque_iterator<T>              iterator;

        /* constructors */
        __deque_base(): M_map(NULL), len(0) {}
        explicit __deque_base(size_t __len) 
        {
            M_map = data_allocator::allocate(__len);
            ustl::fill(M_map, M_map+__len, NULL);
            len   = __len;
        }

        /* destructor */
        ~__deque_base() 
        {
            if ( M_map != NULL ) 
            {
                _M_destroy_nodes(M_start._m_node, M_finish._m_node + 1);
                map_allocator::
            }
        }

        size_t size() { return _len; }

        void _M_initialize_M_map(size_t num_elements); 
        void _M_create_nodes(map_pointer nstart, map_pointer nfinish);
        void _M_destroy_nodes(map_pointer nstart, map_pointer nfinish);

        

    protected:
        T**    M_map;
        size_t M_map_size;
        iterator M_start;       // the first element's iterator 
        iterator M_finish;      // the last element's iterator
};

template<class T, class Allocator>
class deque{
    public:
        typedef __deque_iterator<T> iterator; 
};

}

