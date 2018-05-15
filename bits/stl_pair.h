
#pragma once

namespace ustl {

template<class T1, class T2>
struct pair {

    typedef pair<T1, T2> _Self;

    T1 first;
    T2 second;

    pair(): first(), second() {}
    pair(T1 _a, T2 _b) : first(_a), second(_b) {}
    template<class U1, class U2> pair(const pair<U1, U2>& _x): first(_x.first), second(_x.second) {}

    _Self&  operator=(const _Self& _x) 
    {
        first = _x.first;
        second = _x.second;
        return *this;
    }

    template<class U1, class U2>
    _Self& operator=(const pair<U1, U2>& _x)
    {
        first = _x.first;
        second = _x.second;
        return *this;
    }
};

template<class T1, class T2>
inline bool
operator==(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    if (lhs.first == rhs.first && lhs.second == rhs.second)
        return true;
    else
        return false;
}

template<class T1, class T2>
inline bool
operator!=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    return !(lhs == rhs);
}

template<class T1, class T2>
inline bool
operator<(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    return lhs.first < rhs.first || (!(rhs.first < lhs.first) && lhs.second < rhs.second);
}

template<class T1, class T2>
inline bool
operator>(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    return rhs < lhs;
}

template<class T1, class T2>
inline bool
operator<=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    return !(lhs>rhs);
}

template<class T1, class T2>
inline bool
operator>=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    return !(lhs<rhs);
}

template<class T1, class T2>
inline pair<T1,T2>
make_pair(T1 _x, T2 _y)
{
    return pair<T1, T2>(_x, _y);
}

}
