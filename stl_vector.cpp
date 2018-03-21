
#include "stl_vector.h"

namespace ustl {

template<class T, class Allocator>
vector<T, Allocator>::iterator erase(vector<T, Allocator>::iterator position)
{
    if (position + 1 != end())
        copy(position + 1, _finish, position);
    --_finish;
    destroy(_finish);
    return position;
}

template<class T, class Allocator>
void resize(size_type new_size, const T& value)
{
    if (newsize < size())
        erase(begin() + new_size, end());
    else
        insert(end(), new_size - size(), value);
}

template<class T, class Allocator>
vector<T, Allocator>::iterator allocate_and_fill(size_type n, const T& value)
{
    iterator result = data_allocator::allocate(n);
    uninitialized_fill_n(result, n, value);
    return result;
}

};