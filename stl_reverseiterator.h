#pragma once

#include "stl_iterator.h"

namespace ustl {

template<class Iterator>
class reverse_iterator_t {
public:

private:
    Iterator _base;
    Iterator _cur;
};

}
