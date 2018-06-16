
#pragma once

namespace ustl {

/*******************************
 *              _node
 *              /   \
 *             /     \
 *      left_node   right_node
 *
 *******************************/
static void
local_RB_tree_rotate_left(RB_tree_node_base* const _node, RB_tree_node_base*& _root)
{
    RB_tree_node_base* const right_node = _node->_M_right;

    _node->_M_right = right_node->_M_left;
    if (right_node->_M_left != NULL)
        right_node->_M_left->_M_parent = _node;
    right_node->_M_parent = _node->_M_parent;

    if (_node == _root)
        _root = right_node;
    else if (_node == _node->_M_parent->_M_left)
        _node->_M_parent->_M_left = right_node;
    else 
        _node->_M_parent->_M_right = right_node;
    right_node->_M_left = _node;
    _node->_M_parent = right_node;
}

static void
local_RB_tree_rotate_right(RB_tree_node_base* const _x, RB_tree_node_base*& _root)
{
    //FIXME: finish this function
}

void 
RB_tree_insert_and_rebalance(const bool _insert_left, RB_tree_node_base *_x, RB_tree_node_base *_p, RB_tree_node_base& _header)
{
    RB_tree_node_base *& root = _header._M_parent;

    _x->_M_parent = _p;
    _x->_M_left = 0;
    _x->_M_right = 0;
    _x->_M_color = _S_red;
}

/*
 * RB_tree_increment() and RB_tree_decrement() are used in iterator move
 */

// find successor of _x
RB_tree_node_base*
RB_tree_increment(RB_tree_node_base* _x)
{
    if (_x->_M_right != NULL) {
        // the right subtree is not empty
        _x = _M_right;
        while (_x->_M_left != NULL)
            _x = _x->_M_left;
        return _x;
    } else {
        // we should backtrack to node's parent
        RB_tree_node_base* parent = _x->_M_parent;
        while (_x != parent->_M_right) {
            _x = parent;
            parent = parent->_M_parent;
        }
        // it is a special case if the root node doesn't have a right subtree
        if (_x->_M_right != parent)
            return parent;
        else
            return _x;
    }
}

// find predecessor of _x
RB_tree_node_base*
RB_tree_decrement(RB_tree_node_base* _x)
{
    // if _x is the _M_header, then we should return the maximum value node of RB_tree
    if (_x->_M_color == S_red && _x->_M_parent->_M_parent == _x)
        return _x->_M_right;

    if (_x->_M_left != NULL) {
        //the preorder node is in left subtree
        _x = _M_left;
        while (_x->_M_right != NULL)
            _x = _x->_M_right;
        return _x;
    } else {
        RB_tree_node_base* parent = _x->_M_parent;
        while (_x != parent->_M_left) {
            _x = parent;
            parent = parent->_M_parent;
        }
        return parent;
    }
}

template<class K, class V, class KeyOfValue, class Compare, class Allocator>
void
RB_tree<K,V,KeyOfValue,Compare,Allocator>::_M_initialize()
{
    this->_M_header._M_color = _S_red;
    this->_M_header._M_parent = NULL;
    this->_M_header._M_left = &(this->_M_header);
    this->_M_header._M_right = &(this->_M_header);
}


template<class K, class V, class KeyOfValue, class Compare, class Allocator>
void 
RB_tree<K,V,KeyOfValue,Compare,Allocator>::_M_reset() {
    this->_M_header._M_parent = NULL;
    this->_M_header._M_left = &this->_M_header;
    this->_M_header._M_right = &this->_M_header;
    this->_M_node_count = 0;
}

/*
 * @_x is the insert node
 * @_p is the
 */
void
RB_tree_insert_and_rebalance(const bool _insert_left, RB_tree_node_base* _x, RB_tree_node_base* _p, RB_tree_node_base& _header)
{
    //FIXME: finish this method
}

// return value is a pair<A, B>
template<class K, class V, class KeyOfValue, class Compare, class Allocator>
pair<typename RB_tree<K,V,KeyOfValue,Compare,Allocator>::base_ptr, RB_tree<K,V,KeyOfValue,Compare,Allocator>::base_ptr> 
RB_tree<K,V,KeyOfValue,Compare,Allocator>::_M_get_insert_unique_pos(const key_type& _k)
{
    typedef pair<base_ptr, base_ptr> Res_type;
    link_type x = _M_begin();
    link_type y = _M_end();

    bool comp = true;

    while (x != 0) 
    {
        y = x;
        comp = _M_key_compare(_k, _S_key(x));
        x = comp ? x->_M_left : x->_M_right;
    }

    iterator it = iterator(y);
    if (comp) {
        if (it == begin()) 
            return Res_type(x, y);
        else
            --it;
    }

    if (_M_key_compare(_S_key(it._M_node), _k))
        return Res_type(x, y);
    return Res_type(it._M_node, 0);
}

template<class NodeGen> 
template<class K, class V, class KeyOfValue, class Compare, class Allocator>
typename RB_tree<K,V,KeyOfValue,Compare,Allocator>::iterator 
RB_tree<K,V,KeyOfValue,Compare,Allocator>::_M_insert(base_ptr _x, base_ptr _p, const value_type& _v, NodeGen& _node_gen)
{
    bool insert_left = (_x != NULL || _p == _M_end() || _M_key_compare(KeyOfValue()(_v), _S_key(_p)));
    link_type z = _node_gen(_v);
    RB_tree_insert_and_rebalance(insert_left, z, _p, this->_M_header);
    ++_M_node_count;
    return iterator(z);
}




}



















