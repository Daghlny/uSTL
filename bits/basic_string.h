
#ifndef _USTL_BASIC_STRING_H_
#define _USTL_BASIC_STRING_H_

#include <type_traits>
#include <limits>
#include <stdexcept>
#include <cstddef>
#include <atomic>   // for atomic_long used in \ref_type

#include "stl_allocator.h"
#include "stl_uninitialized.h"

namespace ustl {

template<class charT, class Allocator> class basic_string;

/*
 * the real memory to store the charactors, use a lower-level to implemente COW
 */

template<class charT, class Allocator = ustl::allocator<charT> >
class string_base {
public:
    typedef size_t               size_type;
    typedef std::atomic_long     ref_type;
    typedef charT*               pointer;
    typedef string_base<charT>   _Self;

    friend class basic_string<charT, Allocator>;

    static size_type _Ch_size() { return sizeof(charT); }
    static size_type max_size() { return Allocator::max_size(); }

    string_base();
    string_base(size_type len);
    ~string_base();

protected:
    _Self*  deattach_and_new(size_type _len);
    void    _M_reserve(size_type new_cap, size_type len);
    pointer _M_insert(size_type index, size_type count, size_type len);
    void    _M_initialized_str(size_type len);
    void    _M_deref();
    void    _M_incref();

    bool    is_shared();

    pointer data();

private:
    pointer     _M_str;
    size_type   _M_capacity;
    ref_type    _M_ref;
};


template<class charT, class Allocator = ustl::allocator<charT> >
class basic_string {

public:
    typedef charT                                   value_type;
    typedef size_t                                  size_type;
    typedef ptrdiff_t                               difference_type;
    typedef charT&                                  reference;
    typedef const charT&                            const_reference;
    typedef charT*                                  pointer;
    typedef const charT*                            const_pointer;

    typedef charT*                                  iterator;
    typedef const charT*                            const_iterator;
    typedef reverse_iterator_t<iterator>            reverse_iterator;
    typedef reverse_iterator_t<const_iterator>      const_reverse_iterator;

    typedef basic_string<charT, Allocator>          _Self;

    

    static const size_type npos = static_cast<size_type>(-1);

    /* Constructors */
    basic_string();
    basic_string(size_type count, charT ch);
    basic_string(charT ch);

    basic_string(const _Self& other);
    basic_string(const _Self& other, size_type pos, size_type count); 
    basic_string(const _Self& other, size_type pos);

    basic_string(const charT* s, size_type count);
    basic_string(const charT* s);

    template<class InputIt> basic_string(InputIt first, InputIt last);

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
    const_reference     operator[](size_type pos) const;
    reference           front();
    const_reference     front() const;
    reference           back();
    const_reference     back() const;
    pointer             data();
    const_pointer       data() const;
    const_pointer       c_str() const;

    /* Iterator */
    iterator                    begin();
    const_iterator              cbegin() const;
    iterator                    end();
    const_iterator              cend() const;
    reverse_iterator            rbegin();
    reverse_iterator            rend();
    const_reverse_iterator      crbegin() const;
    const_reverse_iterator      crend() const;

    /* Capacity */
    bool        empty() const;
    size_type   size() const;
    size_type   length() const;
    size_type   max_size() const;
    void        reserve();
    size_type   capacity() const;
    void        shrink_to_fit();

    /* Modifiers */
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

protected:
    enum { _S_local_capacity = 15/sizeof(charT) };
    union {
        charT       _M_local_buf[_S_local_capacity+1];
        string_base<charT, Allocator> *_M_base;
    };
    pointer _M_p;
    size_type   _M_len;

    bool _M_is_local() const;


private:
    void _M_ensure_writable(size_type _requested_cap);

    /*** initialize ***/
    void _M_initialize(const char* s, size_type index, size_type count);
    void _M_initialize(const _Self& other, size_type index, size_type count);

    /*** insert ***/
    template<class InputIt> iterator insert_aux(iterator, InputIt, InputIt, std::true_type);
    template<class InputIt> iterator insert_aux(iterator, InputIt, InputIt, std::false_type);


    void __insert(size_type index, size_type count, charT ch);
    void __insert(size_type index, const charT* s, size_type s_index, size_type s_count);
    iterator __insert(iterator pos, size_type count, charT ch);
    template<class InputIt> iterator __insert(iterator pos, InputIt first, InputIt last, size_type count);


};

}

#endif


