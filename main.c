/**
 * @file main.c
 * @brief Entry point of reading satellite data frame sample SATELLITE_TELEMETRY_DATA_FILENAME
 *
 *  Logic for opening, reading, sorting, deduplicating, and exporting to SUN_SENSOR_DATA_CSV_FILENAME and
 *  SATELLITE_TELEMETRY_DATA_FILENAME, the data for the sunvectors[x,y,z] and the temperature,
 *  using custom built library functions
 *
 * @author Federico Jose Diaz
 * @date 26/10/2025
 *
 * @note In this implementation, the read frames are loaded in memory
 *       to deduplicate, and sort via the column "rtc_s", as the frames could come out of order
 */

#include "beacon_frame_schema.h"
#include "thermal_calibrated.h"
#include "sun_sensors_calibrated.h"
#include "csv_tool.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define THERMAL_DATA_CSV_FILENAME "thermal_data.csv"
#define SUN_SENSOR_DATA_CSV_FILENAME "sun_sensor_data.csv"
#define SATELLITE_TELEMETRY_DATA_FILENAME "TITAraw_tlmy.bin"

#define CSV_DECIMAL_PRECISION 2

int process_thermal_data(ThermalTelemetryCalibrated* thermal_telemetry_array, size_t *thermal_length);
int process_sun_sensors_data(SunSensorsTelemetryCalibrated* sun_sensors_telemetry_array, size_t *sun_sensors_length);

// @note Because this is a code::blocks project, I opted for not using console parameters
// even that it's easy to configure, for simplicity, not used.
int main()
{
    FILE *file = fopen(SATELLITE_TELEMETRY_DATA_FILENAME, "rb");

    if (!file)
    {
        perror("fopen");
        return 1;
    }

    // the header for each frame
    BeaconHeader header = { .beacon_id = { {0xFF,0xFF,0xF0} } };

    // a struct to hold each frame read from the file
    BeaconFrame frame;

    ThermalTelemetryCalibrated *thermal_telemetry_array = NULL;
    size_t thermal_length = 0;
    size_t thermal_capacity = 0;

    SunSensorsTelemetryCalibrated *sun_sensor_telemetry_array = NULL;
    size_t sun_sensor_length = 0;
    size_t sun_sensor_capacity = 0;


    printf("[EXEC] file frame reading... \n");
    ReadFileReturnType read_state = read_data_frame(file, header, &frame);
    //@note I ended up reading both the thermal and sunsensor data.
    //      Using the same architecture logic, I could maintain a somewhat cohesive structure
    while (read_state == READ_OK)
    {
        /* THERMAL SECTION */
        ThermalTelemetryCalibrated thermal_telemetry;
        thermal_telemetry = thermal_to_calibrated(&frame.thermal, frame.platform.rtc_s);

        if (thermal_length == thermal_capacity)
        {
            // initialize with 128 capacity so we are not incrementing all the time
            // if not enough space, increase capacity by adding 128
            size_t new_capacity = thermal_capacity ? thermal_capacity + 128 : 128;
            void *temp_ptr = realloc(thermal_telemetry_array, new_capacity * sizeof *thermal_telemetry_array);
            if (!temp_ptr)
            {
                perror("realloc");
                free(thermal_telemetry_array);
                free(sun_sensor_telemetry_array);
                fclose(file);
                return 1;
            }
            thermal_telemetry_array = (ThermalTelemetryCalibrated*)temp_ptr;
            thermal_capacity = new_capacity;
        }
        thermal_telemetry_array[thermal_length++] = thermal_telemetry;
        /* END THERMAL SECTION */

        /* SUN VECTOR SECTION */
        SunSensorsTelemetryCalibrated sun_sensor_telemetry;
        sun_sensor_telemetry = sun_sensors_to_calibrated(&frame.aocs, frame.platform.rtc_s);

        if (sun_sensor_length == sun_sensor_capacity)
        {
            // initialize with 128 capacity so we are not incrementing all the time
            // if not enough space, increase capacity by adding 128
            size_t new_capacity = sun_sensor_capacity ? sun_sensor_capacity + 128 : 128;
            void *temp_ptr = realloc(sun_sensor_telemetry_array, new_capacity * sizeof * sun_sensor_telemetry_array);
            if (!temp_ptr)
            {
                perror("realloc");
                free(thermal_telemetry_array);
                free(sun_sensor_telemetry_array);
                fclose(file);
                return 1;
            }
            sun_sensor_telemetry_array = (SunSensorsTelemetryCalibrated*)temp_ptr;
            sun_sensor_capacity = new_capacity;
        }
        sun_sensor_telemetry_array[sun_sensor_length++] = sun_sensor_telemetry;
        /* END SUN VECTOR SECTION */

        read_state = read_data_frame(file, header, &frame);
    }

    if (read_state == READ_FAIL)
    {
        fprintf(stderr, "Something went wrong with the file read: READ_FAIL \n");
        free(thermal_telemetry_array);
        free(sun_sensor_telemetry_array);
        fclose(file);
        return 1;
    }

    fclose(file);

    if (thermal_length == 0 ||sun_sensor_length == 0)
    {
        fprintf(stderr, "No frames in file \n");
        free(thermal_telemetry_array);
        free(sun_sensor_telemetry_array);
        return 1;
    }

    printf("[CHCK] thermal data packets: %d \n",thermal_length);
    printf("[CHCK] SUN data packets: %d \n",sun_sensor_length);

    printf("[EXEC] thermal data processing... \n");
    if(!process_thermal_data(thermal_telemetry_array, &thermal_length))
    {
        fprintf(stderr, "ERROR: could not process the thermal data for some reason \n");
    }
    printf("[EXEC] sun sensor data processing... \n");
    if(!process_sun_sensors_data(sun_sensor_telemetry_array, &sun_sensor_length))
    {
        fprintf(stderr, "ERROR: could not process the sun sensor data for some reason \n");
    }

    printf("[CHCK] thermal data packets post process: %d \n",thermal_length);
    printf("[CHCK] SUN data packets post process: %d \n",sun_sensor_length);

    free(thermal_telemetry_array);
    free(sun_sensor_telemetry_array);
    return 0;
}


int process_thermal_data(ThermalTelemetryCalibrated* thermal_telemetry_array, size_t *thermal_length)
{
    //PROCESS THERMAL VALUES (NOT NEEDED BUT ALREADY DONE)
    // sort by timestamp using qsort
    qsort(thermal_telemetry_array, *thermal_length, sizeof *thermal_telemetry_array, thermal_timestamp_comparator);

    //Update the new array size with duplicates removed
    *thermal_length = array_duplicate_removal(thermal_telemetry_array, *thermal_length, sizeof *thermal_telemetry_array, thermal_timestamp_comparator);

    printf("[EXEC] generating CSV for thermal data at: ./%s\n",THERMAL_DATA_CSV_FILENAME);

    int csv_file_status = write_array_to_csv(
        THERMAL_DATA_CSV_FILENAME,
        thermal_telemetry_array,                // Generic pointer to the data array
        *thermal_length,                         // Number of elements
        sizeof(ThermalTelemetryCalibrated),     // Size of a single element
        thermal_calibrated_to_csv_line,         // The callback formatter function
        CSV_DECIMAL_PRECISION,                  // Decimal precision on print
        "rtc_s",                                // First column name
        "CPU_C",                                // Remaining column names
        "mirror_cell_C",
        NULL                                    // NULL to terminate the list of column names
    );

    if (csv_file_status != 1)
    {
        fprintf(stderr, "CSV generation failed.\n");
    }
    printf("[SAVE] Data file saved at: ./%s\n", THERMAL_DATA_CSV_FILENAME);
    return 1;
}

int process_sun_sensors_data(SunSensorsTelemetryCalibrated* sun_sensors_telemetry_array, size_t *sun_sensors_length)
{
    //PROCESS SUNSENSOR VALUES
    // sort by timestamp using qsort
    qsort(
          sun_sensors_telemetry_array,
          *sun_sensors_length,
          sizeof * sun_sensors_telemetry_array,
          sun_sensors_timestamp_comparator
          );

    //Update the new array size with duplicates removed
    *sun_sensors_length = array_duplicate_removal
                                    (
                                    sun_sensors_telemetry_array,
                                    *sun_sensors_length,
                                    sizeof *sun_sensors_telemetry_array,
                                    sun_sensors_timestamp_comparator
                                    );

    printf("[EXEC] generating CSV for sun_vector data at: ./%s\n",SUN_SENSOR_DATA_CSV_FILENAME);

    int csv_file_status = write_array_to_csv(
        SUN_SENSOR_DATA_CSV_FILENAME,
        sun_sensors_telemetry_array,                // Generic pointer to the data array
        *sun_sensors_length,                         // Number of elements
        sizeof(SunSensorsTelemetryCalibrated),      // Size of a single element
        sun_sensors_calibrated_to_csv_line,         // The callback formatter function
        CSV_DECIMAL_PRECISION,                      // Decimal precision on print
        "rtc_s",                                    // First column name
        "sun_vector_x",
        "sun_vector_y",
        "sun_vector_z",
        NULL                                    // NULL to terminate the list of column names
    );

    if (csv_file_status != 1)
    {
        fprintf(stderr, "CSV generation failed.\n");
    }
    printf("[SAVE] Data file saved at: ./%s\n", SUN_SENSOR_DATA_CSV_FILENAME);
    return 1;
}
