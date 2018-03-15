
#pragma once

#if 0
#include <new>
#define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
#include <iostream.h>
#define __THROW_BAD_ALLOC cerr << "out of memory" << endl; exit(1)
#endif

namespace uSTL{

/* 
 * \brief the first level allocator that uses \malloc and \free directly
 */
class __malloc_alloc {

    private:
        static void (* __malloc_alloc_oom_handler) ();

    public:
        static void *allocate(size_t n) {
            void *result = malloc(n);
            if (result == NULL) 
                result = __oom_alloc(n);
            return result;
        }

        static void deallocate(void *p, size_t /* n */) {
            free(p);
        }

        static void *reallocate(void *p, size_t /*old_sz*/, size_t new_sz) {
            void *result = realloc(p, new_sz);
            if ( result == NULL )
                result = __oom_realloc(p, new_sz);
            return result;
        }

        static void (* set_malloc_handler(void (*f)()))() {
            void (*old) () = __malloc_alloc_oom_handler;
            __malloc_alloc_oom_handler = f;
            return old;
        }

        void (*__malloc_alloc::__malloc_alloc_oom_handler)() = 0;

        void *__oom_malloc(size_t n) {
            void (* my_alloc_handler)();
            void *result;
            while (true) {
                my_alloc_handler = __malloc_alloc_oom_handler;
                if (my_alloc_handler == 0) {
                    __THROW_BAD_ALLOC;
                }
                (*my_alloc_handler)();
                result = malloc(n);
                if (result) 
                    return result;
            }
        }

        void *__oom_realloc(void *p, size_t n) {
            void (*my_malloc_handler)();
            void *result;

            while (true) {
                my_malloc_handler = __malloc_alloc_oom_handler;
                if (my_malloc_handler == 0) {
                    __THROW_BAD_ALLOC;
                }
                (*my_malloc_handler)();
                result = realloc(p, n);
                if (result) 
                    return result;
            }
        }
};

typedef __malloc_alloc malloc_alloc;

/*
 * \brief the second level allocator that uses free lists and first level allocator
 */

enum {__ALIGN = 8};
enum {__MAX_BYTES = 128};
enum {__NFREELISTS = __MAX_BYTES/__ALIGN};

template<bool threads>
class __default_alloc_template {
    private:
        static size_t round_up(size_t nbytes) {
            return (((nbytes) + __ALIGN-1) & ~(__ALIGN-1));
        }

    private:
        union __obj {
            union __obj *free_list_link;
            char client_data[1];
        }

    private:
        static __obj* volatile free_list[__NFREELISTS];

        /*
         * \brief decide the index of free-lists should use
         */
        static size_t freelist_index(size_t nbytes) {
            return (((nbytes) + __ALIGN-1) / __ALIGN - 1);
        }

        static void* __refill(size_t n);
        static char* __chunk_alloc(size_t size, int &n);

        static char *start_free;
        static char *end_free;
        static size_t heap_size;

    public:
        static void *allocate(size_t n) {
            __obj* volatile *my_free_list;
            __obj* result;

            if (n > (size_t) __MAX_BYTES) {
                return (malloc_alloc::allocate(n));
            }

            my_free_list = free_list + freelist_index(n);
            result = *my_free_list;
            if (result == 0) {
                void *r = refill(round_up(n));
                return r;
            }

            *my_free_list = result->free_list_link;
            return (result);
        }

        static void deallocate(void *p, size_t n) {
            __obj *q = (__obj *)p;
            __obj *volatile *my_free_list;

            if (n > (size_t) __MAX_BYTES) {
                malloc_alloc::deallocate(p, n);
                return;
            }

            my_free_list = free_list + freelist_index(n);
            q->free_list_link = *my_free_list;
            *my_free_list = q;
        }
        static void *reallocate(void *p, size_t old_sz, size_t new_sz);
};

template<bool threads>
char *__default_alloc_template<threads>::start_free = 0;

template<bool threads>
char *__default_alloc_template<threads>::end_free = 0;

template<bool threads>
size_t __default_alloc_template<threads>::heap_size = 0;

template<bool threads>
__default_alloc_template<threads>::__obj *volatile
__default_alloc_template<threads>::free_lists[__NFREELISTS] = 
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

}

