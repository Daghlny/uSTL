
#pragma once

namespace ustl {

enum RB_tree_color { S_red = false, S_black = true };

struct RB_tree_node_base 
{
    typedef RB_tree_node_base*          base_ptr;
    typedef const RB_tree_node_base*    const_base_ptr;

    RB_tree_color   _M_color;
    base_ptr        _M_parent;
    base_ptr        _M_left;
    base_ptr        _M_right;

    /*
     * _S_minimum() to get the minimum value as _x is the root
     * _S_maximum() to get the maximum value as _x is the root
     */
    static base_ptr         _S_minimum(base_ptr _x)
    {
        while (_x->_M_left != NULL) _x = _x->_M_left;
        return _x;
    }

    static const_base_ptr   _S_minimum(const_base_ptr _x)
    {
        while (_x->_M_left != NULL) _x = _x->_M_left;
        return _x;
    }

    static base_ptr         _S_maximum(base_ptr _x)
    {
        while (_x->_M_right != NULL) _x = _x->_M_right;
        return _x;
    }

    static const_base_ptr   _S_maximum(const_base_ptr _x)
    {
        while (_x->_M_right != NULL) _x = _x->_M_right;
        return _x;
    }
};

template<typename Val>
struct RB_tree_node : public RB_tree_node_base {
    typedef RB_tree_node<Val>*      link_type;

    Val _M_value_field;

    Val*        _M_valptr() { return &_M_value_field; }
    const Val*  _M_valptr() const { return &_M_value_field; }
};

/* 
 * these functions help iterator to move
 * implemented in tree.tcc
 */
RB_tree_node_base*          RB_tree_increment(RB_tree_node_base* _x);
const RB_tree_node_base*    RB_tree_increment(const RB_tree_node_base* _x);

RB_tree_node_base*          RB_tree_decrement(RB_tree_node_base* _x);
const RB_tree_node_base*    RB_tree_decrement(const RB_tree_node_base* _x);

template<class T>
struct RB_tree_iterator
{
    typedef T   value_type;
    typedef T&  reference;
    typedef T*  pointer;

    typedef ustl::bidirectional_iterator_tag    iterator_category;
    typedef ptrdiff_t                           difference_type;

    typedef RB_tree_iterator<T>                 _Self;
    typedef RB_tree_node_base::base_ptr         base_ptr;
    typedef RB_tree_node<T>*                    link_type;

    _Rb_tree_iterator() : _M_node() {}
    explicit RB_tree_iterator(base_ptr _x) : _M_node(_x) {}

    reference   operator*() const { return *static_cast<link_type>(_M_node)->_M_valptr(); }
    pointer     operator->() const { return static_cast<link_type>(_M_node)->_M_valptr(); }

    _Self&      operator++() { _M_node = RB_tree_increment(_M_node); return *this; }
    _Self&      operator++(int) { _Self tmp = *this; _M_node = RB_tree_increment(_M_node); return tmp; }
    _Self&      operator--() { _M_node = RB_ree_decrement(_M_node); return *this; }  
    _Self&      operator--(int) { _Self tmp = *this; _M_node = RB_tree_decrement(_M_node); return tmp; }
    bool        operator==(const _Self& _x) const { return _M_node == _x._M_node; }
    bool        operator!=(const _Self& _x) const { return _M_node != _x._M_node; }

    base_ptr    _M_node;
};

template<class T>
struct RB_tree_const_iterator
{
    typedef T           value_type;
    typedef const T&    reference;
    typedef const T*    pointer;

    // this is the difference with RB_tree_iterator
    typedef ustl::RB_tree_iterator<T>   iterator;

    typedef ustl::bidirectional_iterator_tag    iterator_category;
    typedef ptrdiff_t                           difference_type;

    typedef RB_tree_iterator<T>                       _Self;
    typedef RB_tree_node_base::const_base_ptr         base_ptr;
    typedef const RB_tree_node<T>*                    link_type;

    RB_tree_const_iterator() : _M_node() {}
    explicit RB_tree_const_iterator(base_ptr _x) : _M_node(_x) {}

    reference   operator*() const { return *static_cast<link_type>(_M_node)->_M_valptr(); }
    pointer     operator->() const { return static_cast<link_type>(_M_node)->_M_valptr(); }

    _Self&      operator++() { _M_node = RB_tree_increment(_M_node); return *this; }
    _Self&      operator++(int) { _Self tmp = *this; _M_node = RB_tree_increment(_M_node); return tmp; }
    _Self&      operator--() { _M_node = RB_ree_decrement(_M_node); return *this; }  
    _Self&      operator--(int) { _Self tmp = *this; _M_node = RB_tree_decrement(_M_node); return tmp; }
    bool        operator==(const _Self& _x) const { return _M_node == _x._M_node; }
    bool        operator!=(const _Self& _x) const { return _M_node != _x._M_node; }

    base_ptr    _M_node;
};

template<class K, class V, class Compare, class Allocator=ustl::allocator<V> >
class RB_tree 
{
    /* type alias */
    protected:
        typedef RB_tree_node_base*              Base_ptr;
        typedef const RB_tree_node_base*        const_Base_ptr;
        typedef RB_tree_node<V>*                link_type;
        typedef const RB_tree_node<V>*          const_link_type;

    public:
        typedef K                               key_type;
        typedef V                               value_type;
        typedef value_type*                     pointer;
        typedef const value_type*               const_pointer;
        typedef value_type&                     reference;
        typedef const value_type&               const_reference;
        typedef size_t                          size_type;
        typedef ptrdiff_t                       difference_type;
        
        typedef RB_tree_iterator<value_type>                iterator;
        typedef RB_tree_const_iteartor<value_type>          const_iterator;
        typedef ustl::reverse_iterator_t<iterator>          reverse_iterator;
        typedef ustl::reverse_iterator_t<const_iterator>    const_reverse_iterator;
        
};

}

