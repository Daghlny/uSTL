
#pragma once

#if 0
#include <new>
#define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
#include <iostream.h>
#define __THROW_BAD_ALLOC cerr << "out of memory" << endl; exit(1)
#endif

#include <stdlib.h>

namespace ustl{

/* 
 * \brief the first level allocator that uses \malloc and \free directly
 *        this is only a class template, the real allocator is \malloc_alloc
 */
class __malloc_alloc {

    private:
        static void (* __malloc_alloc_oom_handler) ();      //the handler when malloc fails

    public:
        /* 
         * \brief allocate the memory of \n bytes 
         */
        static void *allocate(size_t n) {
            void *result = malloc(n);
            if (result == NULL) 
                result = __oom_malloc(n);
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

        static void *__oom_malloc(size_t n) {
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

        static void *__oom_realloc(void *p, size_t n) {
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

class __default_alloc_template {
    private:
        // adjust the nbytes into the closest __ALIGN's multiple
        static size_t round_up(size_t nbytes) {
            return (((nbytes) + __ALIGN-1) & ~(__ALIGN-1));
        }

    private:
        union __obj {
            union __obj *free_list_link;
            char client_data[1];
        }

        static __obj* volatile free_list[__NFREELISTS];

        // calculate the index of given \nbytes in free-lists
        static size_t freelist_index(size_t nbytes) {
            return (((nbytes) + __ALIGN-1) / __ALIGN - 1);
        }

        static void* __refill(size_t n);
        static char* __chunk_alloc(size_t size, int &n);

        static char *start_free;        // the begin of memory pool
        static char *end_free;          // the end of memory pool
        static size_t heap_size;        // the size of history memory space size added into memory pool

    public:
        static void *allocate(size_t n) {
            __obj* volatile *my_free_list;
            __obj* result;

            if (n > (size_t) __MAX_BYTES) {
                return (malloc_alloc::allocate(n));
            }

            my_free_list = free_list + freelist_index(n);
            result = *my_free_list;

            // if there is no more space in the certain freelist
            if (result == 0) {
                void *r = __refill(round_up(n));
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

            // insert the block into the head of certain freelist
            my_free_list = free_list + freelist_index(n);
            q->free_list_link = *my_free_list;
            *my_free_list = q;
        }

        static void *reallocate(void *p, size_t old_sz, size_t new_sz);

        //void* __refill(size_t n);
        //char* __chunk_alloc(size_t size, int& nobjs);
};

char *__default_alloc_template::start_free = 0;

char *__default_alloc_template::end_free = 0;

size_t __default_alloc_template::heap_size = 0;

__default_alloc_template::__obj *volatile
__default_alloc_template::free_lists[__NFREELISTS] = 
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


// \n must be __ALIGN's multiple
// __refill the freelist with the size of \n
void* __default_alloc_template::__refill(size_t n) {

    int nobjs = 20;
    char *chunk = __chunk_alloc(n, nobjs);

    __obj* volatile *my_free_list;
    __obj* result;
    __obj* current_obj, next_obj;
    int i;

    // if only alloc a block, then return it to caller
    if (1 == nobjs) 
        return chunk;

    my_free_list = free_list + freelist_index(n);
    result = (__obj*) chunk;

    next_obj = (__obj*)(chunk+n);
    *my_free_list = next_obj;

    // set the tail of allocated freelist is NULL(i.e, 0)
    for (i = 1; ; ++i) {
        current_obj = next_obj;
        next_obj = (__obj*)((char*)next_obj + n);
        if (nobjs-1 == i) {
            current_obj->free_list_link = 0;
            break;
        } else {
            current_obj->free_list_link = next_obj;
        }
    }
    return result;
}

// alloc \nobjs __obj with size of \size from memory pool
// Be Careful: the value of \nobjs maybe changed, it will return the real amount of allocated chunks
char* __default_alloc_template::__chunk_alloc(size_t size, int &nobjs) {

    char *result;
    size_t total_bytes = size*nobjs;
    size_t bytes_left  = end_free - start_free;

    // if (memory pool can satisfy all space need)
    // else if (memory pool can satisfy part of space need)
    // else (memory pool cannot satisfy even a block)
    if ( bytes_left >= total_bytes ) {
        result = start_free;
        start_free += total_bytes;
        return result;
    } else if ( bytes_left >= size ) {
        nobjs = bytes_left/size;
        total_bytes = size*nobjs;
        result = start_free;
        start_free += total_bytes;
        return result;
    } else {
        // \bytes_to_get is the memory size we will add into memory pool this time
        size_t bytes_to_get = 2*total_bytes + round_up(heap_size>>4);

        // make use of remaining memory in memory pool
        if (bytes_left > 0) {
            __obj* volatile *my_free_list = freelist+freelist_index(bytes_left);
            ((__obj*)start_free)->free_list_link = *my_free_list;
            *my_free_list = (__obj*) start_free;
        }

        start_free = (char*)malloc(bytes_to_get);

        if (0 == start_free) {
            // malloc fails
            int i;
            __obj *volatile *my_free_list, *p;
            for (i = size; i <= __MAX_BYTES; i += __ALIGN) {
                my_free_list = free_list  + freelist_index(i);
                p = *my_free_list;
                if ( 0 != p ) {
                    // if the size bigger than the required size have extra space
                    // then we will move it into memory pool than call __chunk_alloc again
                    *my_free_list = p->free_list_link;
                    start_free = (char*)p;
                    end_free   = start_free+i;
                    return (__chunk_alloc(size, nobjs));
                }
            }
            end_free = 0;
            start_free = (char*)malloc_alloc::allocate(bytes_to_get);
        }

        heap_size += bytes_to_get;
        end_free  = start_free + bytes_to_get;

        return __chunk_alloc(bytes, nobjs);
    }
}

}


















