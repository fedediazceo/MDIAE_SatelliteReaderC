/**
 * @file extended_tools.h
 * @brief Header of extended tools used
 *
 *  Contains a data definition for 3 byte non-standar types,
 *  a helper function to swap bytes, and a generic array deduplication tool
 *
 * @author Federico Jose Diaz
 * @date 26/10/2025
 *
 */

#ifndef EXTENDED_TOOLS_H_INCLUDED
#define EXTENDED_TOOLS_H_INCLUDED

#include <stdint.h>

/**
 * @struct uint24_t
 * @brief  defines a 24 bit value to use in non-standard accesses
 */
typedef struct
{
    uint8_t b[3];                   // The value is a 3-byte.
} uint24_t;


/**
 * @brief Swaps a 16 bit value
 * @param[in] x  value_to_swap (uint16_t)
 * @return Swapped value (uint16_t)
 */
uint16_t byte16_swap(uint16_t x);

/**
 * @brief Swaps a 32 bit value
 * @param[in] x value_to_swap (uint32_t)
 * @return Swapped value (uint32_t)
 */
uint32_t byte32_swap(uint32_t x);

/**
 * @brief array deduplication tool. Uses a qsort comparator style
 *
 * Keeps first element of the 2 duplicates. Requires a sorted array
 * The comparator must return 0 on equal values, returns the new logical length
 * Tail elements are not physically removed
 *
 * @param[out] array             Pointer to the array
 * @param[in] n                 Number of elements
 * @param[in] size              Size of one element in bytes
 * @param[in] comparator        int (*)(const void*, const void*), same signature as qsort
 *                          Must define equality by returning 0 for duplicates
 *
 * @return New logical length after duplicates removal.
 */
size_t array_duplicate_removal
(
    void *array, const size_t n, const size_t size,
    int (*comparator)(const void*, const void*)
);

#endif // EXTENDED_TOOLS_H
