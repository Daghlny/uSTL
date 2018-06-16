
#pragma once

namespace ustl{

template<class K, class Compare = ustl::less<K>, class Allocator=ustl::allocator<K> >
class set 
{
    public:
        typedef K       key_type;
        typedef K       value_type;
        typedef Compare key_compare;
        typedef Compare value_compare;
    
    private:
        typedef RB_tree<key_type, value_type, key_compare> _Rep_type;

        _Rep_type _M_t;     // the base RB_tree

    public:
        typedef typename _Rep_type::const_iterator           iterator;
        typedef typename _Rep_type::const_iterator           const_iterator;
        typedef typename _Rep_type::const_reverse_iterator   reverse_iterator;
        typedef typename _Rep_type::const_reverse_iterator   const_reverse_iterator;
        typedef typename _Rep_type::size_type                size_type;
        typedef typename _Rep_type::difference_type          difference_type;

        typedef set<K, Compare, Allocator> _Self;

        /* Constructors */
        set(): _M_t() {}
        template<class InputIt> set(InputIt first, InputIt last): _M_t() { _M_t._M_insert_unique(first, last); }
        set(const _Self& x): _M_t(x._M_t) {}

        /* Operators */
        _Self& operator=(const _Self& x) { _M_t = x._M_t; return *this; }

        /* Access */
        iterator begin() const { return _M_t.begin(); }
        iterator end()   const { return _M_t.end(); }
        iterator rbegin() const { return _M_t.rbegin(); }
        iterator rend()   const { return _M_t.rend(); }
        iterator cbegin() const { return _M_t.begin(); }
        iterator cend()   const { return _M_t.end(); }
        iterator crbegin() const { return _M_t.rbegin(); }
        iterator crend()   const { return _M_t.rend(); }

        /* Capacity */
        bool empty() const {return _M_t.empty();}
        size_type size() const { return _M_t.size(); }
        size_type max_size() const { return _M_t.max_size(); }

        /* Operations */
        ustl::pair<iterator, bool> insert(const value_type &x) { 
            ustl::pair<typename _Rep_type::iterator, bool> p = _M_t._M_insert_unique(x);
            return ustl::pair<iterator, bool>(p.first, p.second);
        }

        template<class InputIt> void insert(InputIt first, InputIt last) {
            _M_t._M_insert_unique(first, last);
        }

        iterator erase(const_iterator position) {
            return _M_t.erase(position);
        }

        iterator erase(iterator position) {
            return _M_t.erase(position);
        }

        size_type erase(const key_type& x) { 
            return _M_t.erase(x);
        }

        iterator erase(const_iterator first, const_iterator last) {
            return _M_t.erase(first, last);
        }

        iterator erase(iterator first, iterator last) {
            return _M_t.erase(first, last);
        }

        void clear() {
            _M_t.clear();
        }

        /* search */
        size_type count(const key_type& x) const {
            return _M_t.find(x) == _M_t.end() ? 0 : 1;
        }

        iterator find(const key_type& x) { return _M_t.find(x); }
        const_iterator find(const key_type& x) const { return _M_t.find(x); }

        // FIXME: ustl::set should have lower_bound(), upper_bound() and equal_range()


};

}
