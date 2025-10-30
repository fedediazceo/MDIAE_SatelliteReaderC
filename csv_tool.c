/**
 * @file csv_tool.c
 * @brief Implementation file of csv_tools header
 *
 *      Includes a helper not exposed function to print the header of the CSV file
 *
 * @author Federico Jose Diaz
 * @date 26/10/2025
 *
 */

#include "csv_tool.h"

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

//////////////////////////////////////////

/**
 * @brief Internal (not exposed in the header) helper function to write the header row using variable arguments
 */
int write_header(FILE* file, const char* first_column_name, va_list args) {
    int total_chars = 0;
    const char* current_column = first_column_name;

    if (current_column) {
        total_chars += fprintf(file, "%s", current_column);
    } else {
        return -1;
    }

    while ((current_column = va_arg(args, const char*)) != NULL) {
        total_chars += fprintf(file, "%s%s", SEPARATOR, current_column);
    }

    total_chars += fprintf(file, "\n");

    return total_chars;
}

//////////////////////////////////////////

int write_array_to_csv(
    const char* filename,
    const void* array_ptr,
    size_t array_length,
    size_t element_size,
    CsvLineFormatter formatter,
    int precision,
    const char* first_column_name,
    ...
)
{
    if (!array_ptr || !filename || !formatter || array_length == 0 || element_size == 0 || !first_column_name)
    {
        fprintf(stderr, "Error: Invalid argument(s) passed to write_array_to_csv.\n");
        return -1;
    }

    FILE *csv_file = fopen(filename, "w");

    if (csv_file == NULL)
    {
        fprintf(stderr, "Error: Cannot open file \"%s\" for writing\n", filename);
        return -1;
    }

    va_list args;
    va_start(args, first_column_name);

    int header_result = write_header(csv_file, first_column_name, args);

    va_end(args);

    if (header_result < 0)
    {
        fprintf(stderr, "warning: Failed to write CSV header.\n");
        fclose(csv_file);
        return -1;
    }

    char csv_line_buffer[MAX_LINE_BUFFER];
    const char* current_element_ptr = (const char*)array_ptr;

    for (size_t i = 0; i < array_length; ++i)
    {

        const void* element_ptr = current_element_ptr + (i * element_size);

        // Usage of the custom function to print the CSV line
        int len = formatter(element_ptr, csv_line_buffer, MAX_LINE_BUFFER, precision);

        if (len < 0)
        {
            fprintf(stderr, "Warning: Formatting failed for element %zu. Skipping.\n", i);
            continue;
        }

        int write_result = fprintf(csv_file, "%s\n", csv_line_buffer);

        if (write_result < len)
        {
            fprintf(stderr, "Error: Failed to write full line for element %zu.\n", i);
            fclose(csv_file);
            return -1;
        }
    }

    fclose(csv_file);
    return 1;
}
