
#pragma once

#include "stl_deque.h"
#include "deque.tcc"

#include "stl_iterator.h"
#include "stl_simple_allocator.h"
#include "stl_allocator.h"
#include "algorithm.tcc"

namespace ustl {

template<class T, class Container = ustl::deque<T> >
class queue {
    public:
        typedef Container                                container_type;
        typedef typename Container::value_type           value_type;
        typedef typename Container::reference            reference;
        typedef typename Container::size_type            size_type;
        typedef typename Container::const_reference      const_reference;

        typedef queue<T, Container>             _Self;

        queue(): c() {}
        explicit queue(const Container& cont): c(cont) {}
        queue(const _Self& other): c(other.c) {}

        _Self& operator=(const _Self& other){
            c.assign(other.c);
        }

        reference front() { return c.front(); }
        const_reference front() const { return c.front(); }
        reference back() { return c.back(); }
        const_reference back() const { return c.back(); }

        bool empty() const { return c.empty(); }
        size_type size() const { return c.size(); }

        void push(const T& x) { c.push_back(x); }
        void pop() { c.pop_front(); }
        void swap(_Self& other) { c.swap(other.c); }

    protected:
        Container c;
};

}
