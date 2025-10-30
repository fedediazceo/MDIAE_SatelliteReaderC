/**
 * @file sun_sensors_calibrated.c
 * @brief Implementation file of the sun_sensors_calibrated header
 *
 * @author Federico Jose Diaz
 * @date 26/10/2025
 *
 */

#include "sun_sensors_calibrated.h"
#include "extended_tools.h"

#include <stdbool.h>
#include <stdio.h>

//////////////////////////////////////////

SunSensorsTelemetryCalibrated
sun_sensors_to_calibrated(
                    const AOCSTelemetrySchema* aocs_schema_value,
                    uint32_t timestamp
                    )
{
    SunSensorsTelemetryCalibrated out;

    int16_t sun_vector_x    = aocs_schema_value->sunvectorX;
    int16_t sun_vector_y    = aocs_schema_value->sunvectorY;
    int16_t sun_vector_z    = aocs_schema_value->sunvectorZ;
    uint32_t ts             = timestamp;

    // swap bytes if file is big endian
    if (IS_BIG_ENDIAN)
    {
        sun_vector_x       = byte16_swap(sun_vector_x);
        sun_vector_y       = byte16_swap(sun_vector_y);
        sun_vector_z       = byte16_swap(sun_vector_z);
        ts                 = byte32_swap(ts);
    }

    out.sun_vector_x = SUN_SENSORS_PHYSICAL_VALUE(sun_vector_x);
    out.sun_vector_y = SUN_SENSORS_PHYSICAL_VALUE(sun_vector_y);
    out.sun_vector_z = SUN_SENSORS_PHYSICAL_VALUE(sun_vector_z);
    out.sun_sensors_telemetry_timestamp = ts;

    return out;
}

//////////////////////////////////////////

void sun_sensors_calibrated_print(const SunSensorsTelemetryCalibrated* sun_sensors_calibrated_values)
{
    if (!sun_sensors_calibrated_values) return;
    printf("Sun sensor (x,y,z) values { ts=%u, Sun Vector (%.2f,%.2f,%.2f}\n",
           sun_sensors_calibrated_values->sun_sensors_telemetry_timestamp,
           sun_sensors_calibrated_values->sun_vector_x,
           sun_sensors_calibrated_values->sun_vector_y,
           sun_sensors_calibrated_values->sun_vector_z);
}

//////////////////////////////////////////

int sun_sensors_calibrated_to_csv_line(
    const void* data_element_ptr,
    char* buffer,
    size_t buffer_size,
    int precision
)
{
    const SunSensorsTelemetryCalibrated* sun_sensors_calibrated_values = (const SunSensorsTelemetryCalibrated*)data_element_ptr;

    if (!sun_sensors_calibrated_values || !buffer || buffer_size == 0) return 0;
    if (precision < 0) precision = 0;
    if (precision > 9) precision = 9;

    int written = snprintf(
                    buffer,
                    buffer_size,
                    "%u;%.*f;%.*f;%.*f",
                    sun_sensors_calibrated_values->sun_sensors_telemetry_timestamp,
                    precision, sun_sensors_calibrated_values->sun_vector_x,
                    precision, sun_sensors_calibrated_values->sun_vector_y,
                    precision, sun_sensors_calibrated_values->sun_vector_z);

    if (written < 0 || (size_t)written >= buffer_size) {
        // @note the defined buffer might be too small
        return -1;
    }

    return written;
}

//////////////////////////////////////////

int sun_sensors_timestamp_comparator(const void *a, const void *b)
{
    const SunSensorsTelemetryCalibrated *x = (const SunSensorsTelemetryCalibrated*)a;
    const SunSensorsTelemetryCalibrated *y = (const SunSensorsTelemetryCalibrated*)b;
    if (x->sun_sensors_telemetry_timestamp < y->sun_sensors_telemetry_timestamp) return -1;
    if (x->sun_sensors_telemetry_timestamp > y->sun_sensors_telemetry_timestamp) return 1;
    return 0;
}
