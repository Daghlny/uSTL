
#pragma once

#include <iostream>
#include <type_traits>
#include <limits>
#include <stdexcept>

#include "stl_allocator.h"
#include "stl_uninitialized.h"

using std::cout;
using std::endl;

namespace ustl {

template<class charT, class Allocator = ustl::allocator<charT>>
class __string_base {
public:
    typedef size_t               size_type;
    typedef int                  ref_type;
    typedef charT*               pointer;
    typedef __string_base<charT> _Self;

    static size_type _Ch_size() { return sizeof(charT); }

    __string_base(size_type len);
    ~__string_base();

protected:
    _Self*  copy();
    void    _M_initialized_str(size_type len);
    void    _M_reserve_cap(size_type cap);
    void    _M_deallocate();

private:
    charT*      _M_str;
    size_type   _M_len;
    size_type   _M_capacity;
    ref_type    _M_ref;
};


template<class charT>
class basic_string {

public:
    typedef charT                                   value_type;
    typedef int                                     size_type;
    typedef ptrdiff_t                               difference_type;
    typedef charT&                                  reference;
    typedef const charT&                            const_reference;
    typedef charT*                                  pointer;
    typedef const charT*                            const_pointer;

    typedef charT*                                  iterator;
    typedef const charT*                            const_iterator;
    typedef reverse_iterator_t<iterator>            reverse_iterator;
    typedef reverse_iterator_t<const_iterator>      const_reverse_iterator;

    typedef basic_string<charT>                     _Self;

    /* Constructors */
    basic_string();
    basic_string(size_type count, charT ch);
    basic_string(charT ch);
    basic_string(const _Self& other, size_type pos, size_type count);
    basic_string(const _Self& other, size_type pos);
    basic_string(const charT* s, size_type count);
    basic_string(const charT* s);
    template<class InputIt> basic_string(InputIt first, InputIt last);
    basic_string(const _Self& other);

    /* Assignments */
    basic_string&   assign(size_type count, charT ch);
    basic_string&   assign(charT ch);
    basic_string&   assign(const _Self& other, size_type pos, size_type count);
    basic_string&   assign(const _Self& other, size_type pos);
    basic_string&   assign(const charT* s, size_type count);
    basic_string&   assign(const charT* s);
    template<class InputIt> 
    basic_string&   assign(InputIt first, InputIt last);
    basic_string&   assign(const _Self& other);

    _Self&      operator=(const _Self& other);

    /* Element access */
    reference           at(size_type pos);
    const_reference     at(size_type pos) const;
    reference           operator[](size_type pos);
    const_reference     operator[](size_type pos);
    reference           front();
    const_reference     front() const;
    reference           back();
    const_reference     back() const;
    pointer             data();
    const_pointer       data() const;
    const_pointer       c_str() const;

    /* Iterator */
    iterator                    begin();
    const_iterator              cbegin();
    iterator                    end();
    const_iterator              cend();
    reverse_iterator            rbegin();
    reverse_iterator            rend();
    const_reverse_iterator      crbegin();
    const_reverse_iterator      crend();

    /* Capacity */
    bool        empty();
    size_type   size();
    size_type   length();
    size_type   max_size();
    void        reserve();
    size_type   capacity();
    void        shrink_to_fit();

    /* Operations */
    void        clear();
    _Self&      insert(size_type index, size_type count, charT ch);
    _Self&      insert(size_type index, const charT* s);
    _Self&      insert(size_type index, const charT* s, size_type count);
    _Self&      insert(size_type index, const _Self& other);
    _Self&      insert(size_type index, const _Self& other, size_type other_index, size_type count);

    iterator    insert(iterator pos, charT ch);
    iterator    insert(iterator pos, size_type count, charT ch);
    template<class InputIt>
    iterator    insert(iterator pos, InputIt first, InputIt last);


    _Self&      erase(size_type index = 0, size_type count = npos);
    iterator    erase(iterator pos);
    iterator    erase(iterator first, iterator last);

    void        push_back(charT ch);
    void        pop_back();
    _Self       substr(size_type pos = 0, size_type count = npos) const;

    // there should be many overloads of @c append() method
    _Self&      operator+=(const _Self& other);
    _Self&      operator+=(charT ch);
    _Self&      operator+=(const charT* s);

    /* Search */
    size_type   find(const _Self& str, size_type pos = 0) const;
    size_type   find(const charT* s, size_type pos, size_type count) const;
    size_type   find(const charT* s, size_type pos = 0) const;
    size_type   find(charT ch, size_type pos = 0) const;

    size_type   rfind(const _Self& str, size_type pos = npos) const;
    size_type   rfind(const charT* s, size_type pos, size_type count) const;
    size_type   rfind(const charT* s, size_type pos = npos) const;
    size_type   rfind(charT ch, size_type pos = npos) const;

    size_type   find_first_of(const _Self& str, size_type pos = 0) const;
    size_type   find_first_of(const charT* s, size_type pos, size_type count) const;
    size_type   find_first_of(const charT* s, size_type pos = 0) const;
    size_type   find_first_of(charT ch, size_type pos = 0) const;

    size_type   find_first_not_of(const _Self& str, size_type pos = 0) const;
    size_type   find_first_not_of(const charT* s, size_type pos, size_type count) const;
    size_type   find_first_not_of(const charT* s, size_type pos = 0) const;
    size_type   find_first_not_of(charT ch, size_type pos = 0) const;

private:
    __string_base *_M_ptr;


};

}

