/**
 * @file beacon_frame_schema.c
 * @brief Definitions of the Schema of the satellite frame data to read
 *
 *  If the frame changes, this is the first file to edit and adapt. The proposed architecture is of
 *          SCHEMA <-> CALIBRATION
 *  Modifying the SCHEMA will probably imply a modification of the calibration. But, modifying the calibration
 *  should have no impact in the SCHEMA, as they are a split process.
 *
 * @author Federico Jose Diaz
 * @date 26/10/2025
 *
 * @note It is included a file reading function that helps populate a frame based on this schema
 */

#ifndef BEACON_FRAME_SCHEMA_H_INCLUDED
#define BEACON_FRAME_SCHEMA_H_INCLUDED

#include "extended_tools.h"

#include <stdio.h>
#include <stdint.h>

#define IS_BIG_ENDIAN 1

/**
    @enum defines the data id's for each section of the frame
    @note used to identify if read data properly
**/
typedef enum
{
    PLATFORM_ID = 0x0001,
    MEMORY_ID = 0x0101,
    CDH_ID = 0x0201,
    POWER_ID = 0x0301,
    THERMAL_ID = 0x0401,
    AOCS_ID = 0x0501,
    PAYLOAD_ID = 0x0601
} FrameID;

typedef enum
{
    READ_OK,
    READ_FAIL,
    READ_EOF
} ReadFileReturnType;

/**
    @file Frame Reader Schema Structures and file reading
    @brief Frame reader organization schema defined in the stream docs.

    @note All the fields are directly translated from the documentation, and used to read the file
          The user of this schema has to read and convert to calibrated values according to docs
    @note For this sample, only temperature and sun_sensor data from AOCS has the calibration implemented.
**/

/* ---- HEADER ---- */
typedef struct HEADER
{
    uint24_t  beacon_id;                    // Pseudo sequence number beacon ID: 0xFF 0xFF 0x00
} BeaconHeader;

/* ---- PLATFORM ---- */
typedef struct PLATFORM_SCHEMA
{
    uint16_t platform_telemetry_id;          // Expected: 0x00 0x01
    uint32_t uptime_s;
    uint32_t rtc_s;                           // Seconds Since 1970-01-01
    uint24_t resetCount;                      // custom 3 bytes structure
    uint8_t  currentMode;                     // Value & 0x7F is mode, Value & 0x8F is computer (0 = B, 1 = A)
    uint32_t lastBootReason;
} PlatformTelemetrySchema;

/* ---- MEMORY ---- */
typedef struct MEMORY_SCHEMA
{
    uint16_t memory_telemetry_id;             // Expected: 0x01 0x01
    uint32_t heap_free_bytes;
} MemoryTelemetrySchema;

/* ---- CDH ---- */
typedef struct CDH_SCHEMA
{
    uint16_t cdh_id;                          // Expected: 0x02 0x01
    uint32_t lastSeenSequenceNumber;
    uint8_t  antennaDeployStatus;
} CDHTelemetrySchema;

/* ---- POWER ---- */
typedef struct POWER_SCHEMA
{
    uint16_t power_telemetry_id;              // Expected: 0x03 0x01
    uint16_t low_voltage_counter;
    uint16_t nice_battery_mV;
    uint16_t raw_battery_mV;
    uint16_t battery_A;                       // A = value * 0.005237
    uint16_t pcm_3v3_V;                       // V = value * 0.003988
    uint16_t pcm_3v3_A;                       // A = value * 0.005237
    uint16_t pcm_5v_V;                        // V = value * 0.005865
    uint16_t pcm_5v_A;                        // A = value * 0.005237
} PowerTelemetrySchema;

/* ---- THERMAL ---- */
typedef struct THERMAL_SCHEMA
{
    uint16_t thermal_telemetry_id;            // Expected: 0x04 0x01
    int16_t  CPU_C;                           // C = value / 100.0
    int16_t  mirror_cell_C;                   // C = value / 100.0
} ThermalTelemetrySchema;

/* ---- AOCS ---- */
typedef struct AOCS_SCHEMA
{
    uint16_t aocs_telemetry_id;               // Expected: 0x05 0x01
    uint32_t aocs_mode;

    int16_t  sunvectorX;                      // value / 16384.0
    int16_t  sunvectorY;                      // value / 16384.0
    int16_t  sunvectorZ;                      // value / 16384.0

    int16_t  magnetometerX_mg;                // mg = value * 0.5
    int16_t  magnetometerY_mg;                // mg = value * 0.5
    int16_t  magnetometerZ_mg;                // mg = value * 0.5

    int16_t  gyroX_dps;                       // dps = value * 0.0125
    int16_t  gyroY_dps;                       // dps = value * 0.0125
    int16_t  gyroZ_dps;                       // dps = value * 0.0125

    int16_t  temperature_IMU_C;               // C = value * 0.14 + 25
    int32_t  fine_gyroX_dps;                  // dps = value * (256 / 6300.0) / 65536
    int32_t  fine_gyroY_dps;                  // dps = value * (256 / 6300.0) / 65536
    int32_t  fine_gyroZ_dps;                  // dps = value * (256 / 6300.0) / 65536

    int16_t  wheel_1_radsec;                  // rad/s = value * 0.3
    int16_t  wheel_2_radsec;                  // rad/s = value * 0.3
    int16_t  wheel_3_radsec;                  // rad/s = value * 0.3
    int16_t  wheel_4_radsec;                  // rad/s = value * 0.3
} AOCSTelemetrySchema;

/* ---- PAYLOAD ---- */
typedef struct PAYLOAD_SCHEMA
{
    uint16_t payload_telemetry_id;            // Expected: 0x06 0x01
    uint16_t experimentsRun;
    uint16_t experimentsFailed;
    int16_t  lastExperimentRun;
    uint8_t  currentState;
} PayloadTelemetrySchema;

/* ---- FULL FRAME STRUCT ---- */
typedef struct FRAME_SCHEMA
{
    PlatformTelemetrySchema platform;
    MemoryTelemetrySchema   memory;
    CDHTelemetrySchema      cdh;
    PowerTelemetrySchema    power;
    ThermalTelemetrySchema  thermal;
    AOCSTelemetrySchema     aocs;
    PayloadTelemetrySchema  payload;
} BeaconFrame;


/**
 * @brief Searchs for the header in the file and then reads a data frame element
 *
 *  When the header is found, reads a frame of data and returns a BeaconFrame struct with the raw values
 *
 * @param[in]   file        File pointer to the data
 * @param[in]   header      Constant structure that holds the beacon header ID to search for
 * @param[out]  out         Pointer to the return structure holding the frame values
 *
 * @return Read file return state
 *
 */
ReadFileReturnType read_data_frame
(
    FILE *file,
    const BeaconHeader header,
    BeaconFrame *out
);

#endif // BEACON_FRAME_SCHEMA_H
