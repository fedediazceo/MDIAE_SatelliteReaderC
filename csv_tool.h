/**
 * @file csv_tool.c
 * @brief Implementation a generic CSV writer tool for data arrays
 *
 *   Includes a pointer definition that MUST be enforced when using this tool
 *
 * @author Federico Jose Diaz
 * @date 26/10/2025
 *
 */

#ifndef CSV_TOOL_H
#define CSV_TOOL_H

#include <stddef.h>
#include <stdio.h>

#define MAX_LINE_BUFFER 256 // A default max size for the buffer
#define SEPARATOR ";"

/**
 * @brief Type definition for the required callback function.
 *
 * This function take a pointer to a single element of the data array,
 * format it as a CSV line (no trailing newline), and store it
 * in the buffer.
 *
 * @param[in] data_element_ptr      A void* pointer to a single element from the array.
 * @param[in] buffer                A character buffer to write the formatted CSV line into.
 * @param[in] buffer_size           The maximum size of the buffer
 *
 * @return int                      The length of the string written to the buffer (excluding '\0'),
 *                                  or a negative value on error.
 */
typedef int (*CsvLineFormatter)
(
    const void* data_element_ptr,
    char* buffer,
    size_t buffer_size,
    int precision
);

/**
 * @brief Writes an array of data to a CSV file
 *
 * Handles opening the file, writing the header with variable arguments),
 * iterating through the data array, using a callback function
 * to format each line.
 *
 * @param[in] filename          The name of the file to create or overwrite.
 * @param[in] array_ptr         A void* pointer to the beginning of the data array.
 * @param[in] array_length      The number of elements in the array.
 * @param[in] element_size      The size of a single element in the array
 * @param[in] formatter         The callback function that converts an element to a CSV string.
 * @param[in] precision         The amount of decimals to print for float values
 * @param[in] first_column_name The first column name string (required to start the variable list).
 * @param[in] ...               Remaining column name strings (char*). The list must be terminated by a NULL pointer.
 *
 * @return int 1 on success, -1 on error.
 */
int write_array_to_csv
(
    const char* filename,
    const void* array_ptr,
    size_t array_length,
    size_t element_size,
    CsvLineFormatter formatter,
    int precision,
    const char* first_column_name,
    ... // variable number of column name strings, end with NULL
);

#endif // CSV_TOOL
