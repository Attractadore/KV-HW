#include "defs.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void* append(void** const buffer_p, size_t* const num_p, const size_t sz, void const* const elem, const size_t end) {
    assert(buffer_p && num_p);
    void* buffer = *buffer_p;
    size_t num = *num_p;
    assert(end <= num);

    if (end == num) {
        const size_t new_num = 2 * num;
        void* const new_buffer = realloc(buffer, new_num * sz);
        if (!new_buffer) {
            return NULL;
        }
        num = new_num;
        buffer = new_buffer;
    }

    memcpy((char*) buffer + end * sz, elem, sz);

    *num_p = num;
    *buffer_p = buffer;

    return buffer;
}
