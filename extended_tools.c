/**
 * @file extended_tools.c
 * @brief Implementation file of extended tools used
 *
 * @author Federico Jose Diaz
 * @date 26/10/2025
 *
 */

#include "extended_tools.h"

#include <string.h>

//////////////////////////////////////////

uint16_t byte16_swap(uint16_t value_to_swap)
{
    return (uint16_t)((value_to_swap >> 8) | (value_to_swap << 8));
}

//////////////////////////////////////////

uint32_t byte32_swap(uint32_t value_to_swap)
{
    return ((value_to_swap & 0x000000FFu) << 24) |
           ((value_to_swap & 0x0000FF00u) <<  8) |
           ((value_to_swap & 0x00FF0000u) >>  8) |
           ((value_to_swap & 0xFF000000u) >> 24);
}

//////////////////////////////////////////

size_t array_duplicate_removal
(
    void *array, const size_t n, const size_t size,
    int (*comparator)(const void*, const void*)
)
{
    if (!array || !comparator || size == 0 || n == 0) return 0;

    unsigned char *p = (unsigned char *)array;
    size_t keep_index = 1;  // next write index

    for (size_t i = 1; i < n; ++i) {
        unsigned char *current = p + i * size;
        unsigned char *previous = p + (keep_index - 1) * size;
        if (comparator(current, previous) != 0) {

            // different elements, keep them, and only shift them if they are not the same element
            if (keep_index != i)
            {
                memmove(p + keep_index * size, current, size);
            }

            ++keep_index;
        }
    }
    // return the new index. Left over elements following the new length are left as unused.
    return keep_index;
}
