#pragma once


#include <cstdlib>

namespace ustl {

    class alloc {
        public:
        static void *allocate(size_t n);
        static void  deallocate(void *ptr, size_t n);
    };

    void *
    alloc::allocate(size_t n) {
        return malloc(n);
    }

    void 
    alloc::deallocate(void *ptr, size_t n) {
        free(ptr);
    }
}

