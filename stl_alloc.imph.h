#pragma once


#include <cstdlib>
#include <iostream>

using std::cout;
using std::endl;

namespace ustl {

    class alloc {
        public:
        static void *allocate(size_t n);
        static void  deallocate(void *ptr, size_t n);
    };

    void *
    alloc::allocate(size_t n) {
        void *res = NULL;
        while (res == NULL)
            res = malloc(n);
        return res;
    }

    void 
    alloc::deallocate(void *ptr, size_t n) {
        free(ptr);
    }
}

