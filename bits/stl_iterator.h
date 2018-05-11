
#pragma once

#include <cstddef>
#include <iostream>

/*
 * ***** Iterator Level ******
 *
 * InputIterator  OutputIterator
 *      |               |
 *       \             /
 *       ForwardIterator
 *             |
 *    BidirectionalIterator
 *             |
 *    RandomAccessIterator
 *
 *********************************
 */

namespace ustl {

/* indicate the type of iterator */
/* these tag classes is used as iterator's category */
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

/* it is used in inherition */
template<class Category, class T, class Distance = ptrdiff_t, class Pointer = T*, class Reference = T&>
struct iterator {
    typedef Category  iterator_category;
    typedef T         value_type;
    typedef Distance  difference_type;
    typedef Pointer   pointer;
    typedef Reference reference;
};

/* extract the traits of iterator */
template <class Iterator>
struct iterator_traits {
    typedef typename Iterator::iterator_category iterator_category;
    typedef typename Iterator::value_type        value_type;
    typedef typename Iterator::difference_type   difference_type;
    typedef typename Iterator::pointer           pointer;
    typedef typename Iterator::reference         reference;
};

template <class T>
struct iterator_traits<T*> {
    typedef random_access_iterator_tag iterator_category;
    typedef T                          value_type;
    typedef ptrdiff_t                  difference_type;
    typedef T*                         pointer;
    typedef T&                         reference;
};

template <class T>
struct iterator_traits<const T*> {
    typedef random_access_iterator_tag iterator_category;
    typedef T                          value_type;
    typedef ptrdiff_t                  difference_type;
    typedef const T*                   pointer;
    typedef const T&                   reference;
};

template <class Iterator>
inline typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator &) {
    typedef typename iterator_traits<Iterator>::iterator_category category;
    return category();
}

template <class Iterator>
inline typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator &) {
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

template<class Iterator>
inline typename iterator_traits<Iterator>::value_type*
__value_type(const Iterator &) {
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

/***** distance functions ******/
template<class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, input_iterator_tag) 
{
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last) {
        ++first;
        ++n;
    }

    return n;
}

template<class RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
__distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag)
{
    return last-first;
}

template<class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last)
{
    typedef typename iterator_traits<InputIterator>::iterator_category category;
    return __distance(first, last, category());
}

/******* advance functions *******/
template<class InputIterator, class Distance>
inline void __advance(InputIterator &i, Distance n, input_iterator_tag)
{
    while (n--) ++i;
}

template<class BidirectionalIterator, class Distance>
inline void __advance(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag) 
{
    if (n >= 0)
        while (n--) 
            ++i;
    else 
        while (n++)
            --i;
}

template<class RandomAccessIterator, class Distance>
inline void __advance(RandomAccessIterator& i, Distance n, random_access_iterator_tag)
{
    i += n;
}

template<class InputIterator, class Distance>
inline void advance(InputIterator& i, Distance n)
{
    __advance(i, n, iterator_category(i));
}

/*******************************************
 *           reverse_iterator_t              *
 *******************************************/
template<class _Iterator>
class reverse_iterator_t : public iterator<typename iterator_traits<_Iterator>::iterator_category, 
                                         typename iterator_traits<_Iterator>::value_type,
                                         typename iterator_traits<_Iterator>::difference_type,
                                         typename iterator_traits<_Iterator>::pointer,
                                         typename iterator_traits<_Iterator>::reference>
{
    protected:
        _Iterator current;

        typedef iterator_traits<_Iterator>    __traits_type;

    public:

        typedef _Iterator                   iterator_type;
        typedef typename __traits_type::difference_type difference_type;
        typedef typename __traits_type::pointer         pointer;
        typedef typename __traits_type::reference       reference;
        typedef reverse_iterator_t<_Iterator> _Self;

        reverse_iterator_t() : current() {}
        explicit reverse_iterator_t(iterator_type __x) : current(__x) {}
        reverse_iterator_t(const reverse_iterator_t& __x) : current(__x.current) {}

        iterator_type base() const {return current;}

        reference operator*() const {
            _Iterator __tmp = current;
            return *--__tmp;
        }

        pointer operator->() const {
            return &(operator*());
        }

        _Self& operator++() {
            --current;
            return *this;
        }

        _Self operator++(int) {
            reverse_iterator_t __tmp = *this;
            --current;
            return __tmp;
        }

        _Self& operator--() {
            ++current;
            return *this;
        }

        _Self operator--(int) {
            _Self __tmp = *this;
            ++current;
            return __tmp;
        }

        _Self operator+(difference_type __n) const {
            return reverse_iterator_t(current - __n);
        }

        _Self& operator+=(difference_type __n) {
            current -= __n;
            return *this;
        }

        _Self operator-(difference_type __n) const {
            return reverse_iterator_t(current + __n);
        }

        _Self& operator-=(difference_type __n) {
            current += __n;
            return *this;
        }

        reference operator[](difference_type __n) const {
            return *(*this+__n);
        }
};

template<typename _Iterator>
inline bool operator==(const reverse_iterator_t<_Iterator>& __x,
                       const reverse_iterator_t<_Iterator>& __y)
{
    return __x.base() == __y.base();
}

template<typename _Iterator>
inline bool operator!=(const reverse_iterator_t<_Iterator>& __x,
                       const reverse_iterator_t<_Iterator>& __y)
{
    return !(__x.base() == __y.base());
}

template<typename _Iterator>
inline bool operator<(const reverse_iterator_t<_Iterator>& __x,
                      const reverse_iterator_t<_Iterator>& __y)
{
    return __y.base() < __x.base();
}
  
template<typename _Iterator>
inline bool operator>(const reverse_iterator_t<_Iterator>& __x,
                      const reverse_iterator_t<_Iterator>& __y)
{
    return __y < __x;;
}

template<typename _Iterator>
inline bool operator<=(const reverse_iterator_t<_Iterator>& __x,
                      const reverse_iterator_t<_Iterator>& __y)
{
    return !(__y < __x);
}
    
template<typename _Iterator>
inline bool operator>=(const reverse_iterator_t<_Iterator>& __x,
                      const reverse_iterator_t<_Iterator>& __y)
{
    return !(__x < __y);
}

template<typename _Iterator>
inline typename reverse_iterator_t<_Iterator>::difference_type
operator-(const reverse_iterator_t<_Iterator>& __x,
          const reverse_iterator_t<_Iterator>& __y)
{
    return __y.base() - __x.base();
}

template<typename _Iterator>
inline reverse_iterator_t<_Iterator>
operator+(typename reverse_iterator_t<_Iterator>::difference_type __n,
          const reverse_iterator_t<_Iterator>& __x)
{
    return reverse_iterator_t<_Iterator>(__x.base()-__n);
}





};






