/**
 * @file thermal_calibrated.h
 * @brief Header file of the specific thermal calibration variables
 *
 *  Contains
 *      - a structure definition that holds the calibrated thermal values
 *      - a calibration function to obtain the proper engineering values
 *      - helper functions to print and export CSV lines based on the
 *          ThermalTelemetryCalibrated struct, and to compare values according to
 *          timestamp
 *
 * @author Federico Jose Diaz
 * @date 26/10/2025
 *
 */

#ifndef THERMAL_CALIBRATED_H_INCLUDED
#define THERMAL_CALIBRATED_H_INCLUDED

#include "beacon_frame_schema.h"
#include "extended_tools.h"

#include <stdbool.h>

//conversion value for temperature
#define TEMP_C_PHYSICAL_VALUE(value) ((float)(value) * 0.01f)

/**
 * @struct ThermalTelemetryCalibrated
 * @brief  Holds thermal calibrated values obtained from the stream.
 *
 * Includes the timestamp of the values obtained from the frame
 */
typedef struct THERMAL_PHYSICAL
{
    uint32_t    thermal_telemetry_timestamp;
    float       CPU_C;                           // C = value / 100.0
    float       mirror_cell_C;                   // C = value / 100.0
} ThermalTelemetryCalibrated;

/**
 * @brief Convert thermal schema values to calibrated [C] and adds a timestamp.
 *
 * Converts T raw fields (int16_t) into [C] (float) using conversion defined in documentation.
 * If the file encoding was big-endian, performs byte-swaps before scaling.
 *
 * @param[in] thermal_schema_value  Pointer to schema read from the frame.
 * @param[in] timestamp             Platform timestamp as read from PLATFORM section in schema.
 *
 * @return ThermalTelemetryCalibrated with timestamp and temperatures values corrected in [C].
 *
 */
ThermalTelemetryCalibrated
thermal_to_calibrated(
                    const ThermalTelemetrySchema* thermal_schema_value,
                    uint32_t timestamp
                    );


/**
 * @brief print thermal calibrated values to console
 *
 * @param[in] thermal   Pointer to a struct that holds the calibrated values
 *
 */
void thermal_calibrated_print(const ThermalTelemetryCalibrated* thermal);

/**
 * @brief print thermal calibrated values to CSV format
 *
 * @param[in]   data_element_ptr    Pointer to a struct that holds the calibrated values
 * @param[out]  char_buffer         Pointer to the char buffer to write
 * @param[in]   buffer_size         predefined line size to write the CSV line
 * @param[in]   decimals            precision on printing decimals
 *
 * @return size of buffer written to out parameter char_buffer
 */
int thermal_calibrated_to_csv_line
(
    const void* data_element_ptr,
    char* buffer,
    size_t buffer_size,
    int decimals
);

/**
 * @brief ThermalTelemetryCalibrated comparator via timestamps
 *
 * @param[in] a      Pointer to value a
 * @param[in] b      Pointer to value b
 *
 * @return int value indicating -1: a < b, 0: a == b, 1: a > b
 */
int thermal_timestamp_comparator(const void *a, const void *b);

#endif // THERMAL_CALIBRATED_H
