/**
 * @file thermal_calibrated.c
 * @brief Implementation file of thermal_calibrated header
 *
 * @author Federico Jose Diaz
 * @date 26/10/2025
 *
 */

#include "thermal_calibrated.h"
#include "extended_tools.h"

#include <stdbool.h>
#include <stdio.h>

//////////////////////////////////////////

ThermalTelemetryCalibrated
thermal_to_calibrated
(
    const ThermalTelemetrySchema* thermal_schema_value,
    uint32_t timestamp
)
{
    ThermalTelemetryCalibrated out;

    int16_t cpu_T    = thermal_schema_value->CPU_C;
    int16_t mirror_T = thermal_schema_value->mirror_cell_C;
    uint32_t ts        = timestamp;

    // swap bytes if file is big endian
    if (IS_BIG_ENDIAN)
    {
        cpu_T       = byte16_swap(cpu_T);
        mirror_T    = byte16_swap(mirror_T);
        ts          = byte32_swap(ts);
    }

    out.thermal_telemetry_timestamp = ts;
    out.CPU_C         = TEMP_C_PHYSICAL_VALUE(cpu_T);
    out.mirror_cell_C = TEMP_C_PHYSICAL_VALUE(mirror_T);

    return out;
}

//////////////////////////////////////////

void thermal_calibrated_print(const ThermalTelemetryCalibrated* thermal)
{
    if (!thermal) return;
    printf("Thermal values { ts=%u, CPU=%.2f C, mirror=%.2f C }\n",
           thermal->thermal_telemetry_timestamp,
           thermal->CPU_C,
           thermal->mirror_cell_C);
}

//////////////////////////////////////////

int thermal_calibrated_to_csv_line(
    const void* data_element_ptr,
    char* buffer,
    size_t buffer_size,
    int precision
)
{
    const ThermalTelemetryCalibrated* thermal_data_calibrated_values = (const ThermalTelemetryCalibrated*)data_element_ptr;

    if (!thermal_data_calibrated_values || !buffer || buffer_size == 0) return 0;

    if (precision < 0) precision = 0;
    if (precision > 9) precision = 9;

    int written = snprintf(
                    buffer,
                    buffer_size,
                    "%u;%.*f;%.*f",
                    thermal_data_calibrated_values->thermal_telemetry_timestamp,
                    precision, thermal_data_calibrated_values->CPU_C,
                    precision, thermal_data_calibrated_values->mirror_cell_C);

    if (written < 0 || (size_t)written >= buffer_size) {
        // @note the defined buffer might be too small
        return -1;
    }

    return written;
}

//////////////////////////////////////////

int thermal_timestamp_comparator(const void *a, const void *b)
{
    const ThermalTelemetryCalibrated *x = (const ThermalTelemetryCalibrated*)a;
    const ThermalTelemetryCalibrated *y = (const ThermalTelemetryCalibrated*)b;
    if (x->thermal_telemetry_timestamp < y->thermal_telemetry_timestamp) return -1;
    if (x->thermal_telemetry_timestamp > y->thermal_telemetry_timestamp) return 1;
    return 0;
}
