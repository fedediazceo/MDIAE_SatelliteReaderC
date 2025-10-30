/**
 * @file sun_sensors_calibrated.h
 * @brief Header file of the specific sun sensor calibration variables
 *
 *  Contains
 *      - a structure definition that holds the calibrated sunvector values
 *      - a calibration function to obtain the proper engineering values
 *      - helper functions to print and export CSV lines based on the
 *          SunSensorsTelemetryCalibrated struct, and to compare values according to
 *          timestamp
 *
 * @author Federico Jose Diaz
 * @date 26/10/2025
 *
 */

#ifndef SUN_SENSORS_CALIBRATED_H_INCLUDED
#define SUN_SENSORS_CALIBRATED_H_INCLUDED

#include "beacon_frame_schema.h"
#include "extended_tools.h"

#include <stdbool.h>

//conversion value for sun sensors
#define SUN_SENSORS_PHYSICAL_VALUE(value) ((float)(value) / 16384.0f)

/**
 * @struct SunSensorsTelemetryCalibrated
 * @brief  Holds sun sensor vector calibrated values obtained from the stream.
 *
 * Includes the timestamp of the values obtained from the frame
 */
typedef struct SUN_SENSORS_CALIBRATED
{
    uint32_t  sun_sensors_telemetry_timestamp;
    float  sun_vector_x;                            // value / 16384.0
    float  sun_vector_y;                           // value / 16384.0
    float  sun_vector_z;                           // value / 16384.0
} SunSensorsTelemetryCalibrated;

/**
 * @brief Convert sun sensors schema values to calibrated vector coordinates and adds a timestamp.
 *
 * Converts (x,y,z) raw fields (int16_t) into float values, using conversion defined in documentation.
 * If the file encoding was big-endian, performs byte-swaps before scaling.
 *
 * @param[in] aocs_schema_value     Pointer to schema read from the frame.
 * @param[in] timestamp             Platform timestamp as read from PLATFORM section in schema.
 *
 * @return SunSensorsTelemetryCalibrated with timestamp and float values representing x,y,z vector coordinates
 *
 */
SunSensorsTelemetryCalibrated
sun_sensors_to_calibrated(
                    const AOCSTelemetrySchema* aocs_schema_value,
                    uint32_t timestamp
                    );


/**
 * @brief print sun sensors calibrated values to console
 *
 * @param[in] sun_sensors  Pointer to a struct that holds the calibrated values
 *
 */
void sun_sensors_calibrated_print(const SunSensorsTelemetryCalibrated* sun_sensors_calibrated_values);

/**
 * @brief print sun sensors calibrated values to CSV format
 *
 *  Implements the structure of the CsvLineFormatter void pointer function for CSV printing
 *
 * @param[in]   data_element_ptr        Pointer to a struct that holds the calibrated values
 * @param[out]  char_buffer             Pointer to the char buffer to write
 * @param[in]   buffer_size             predefined line size to write the CSV line
 * @param[in]   decimals                precision on printing decimals
 *
 * @return size of buffer written to out parameter char_buffer
 */
int sun_sensors_calibrated_to_csv_line
(
    const void* data_element_ptr,
    char* buffer,
    size_t buffer_size,
    int precision
);

/**
 * @brief SunSensorsTelemetryCalibrated comparator via timestamps
 *
 * @param[in] a      Pointer to value a
 * @param[in] b      Pointer to value b
 *
 * @return int value indicating -1: a < b, 0: a == b, 1: a > b
 */
int sun_sensors_timestamp_comparator(const void *a, const void *b);

#endif //SUN_SENSORS_CALIBRATED_H
