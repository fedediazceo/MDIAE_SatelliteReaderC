/**
 * @file beacon_frame_schema.c
 * @brief Implementations of the Schema of the satellite frame data to read
 *
 *  Contains mostly the helper function to read a schema from a provided file
 *
 * @author Federico Jose Diaz
 * @date 26/10/2025
 *
 */

#include "beacon_frame_schema.h"

#include <stdbool.h>
#include <stdio.h>

//Used to read field by field, and avoid struct padding problems with the file
#define READ_FIELD(file, ptr) (fread((ptr), 1, sizeof(*(ptr)), (file)) == sizeof(*(ptr)))

//////////////////////////////////////////

ReadFileReturnType read_data_frame
(
    FILE *file,
    const BeaconHeader header,
    BeaconFrame *out
)
{
    if (!file || !out) return READ_FAIL;

    unsigned char header_match[3] = {0,0,0};
    size_t header_index = 0;

    //search for the beaconID using a sliding mask
    for (;;)
    {
        int byte_read = fgetc(file);
        if (byte_read == EOF) return READ_EOF;

        if (header_index < 3)
        {
            header_match[header_index++] = byte_read;
        }
        else
        {
            header_match[0] = header_match[1];
            header_match[1] = header_match[2];
            header_match[2] = byte_read;
        }
        if (header_index == 3 &&
            header_match[0] == header.beacon_id.b[0] &&
            header_match[1] == header.beacon_id.b[1] &&
            header_match[2] == header.beacon_id.b[2])
        {
            break;
        }
    }
    // Beacon ID found, now on to read the frame
    // because of struct padding, the "fastest" way of preventing reading problems
    // is to read each field separately. Yeah, this is because I've used the full structure
    // I could have just read the stream of bytes for the thermal data, but.. here we are

    /* PLATFORM */
    if (!READ_FIELD(file, &out->platform.platform_telemetry_id)) return READ_FAIL;

    if (PLATFORM_ID != (IS_BIG_ENDIAN?byte16_swap(out->platform.platform_telemetry_id):out->platform.platform_telemetry_id))
    {
        printf("WRONG PLATFORM ID IN FRAME, read %x, expected %x", byte16_swap(out->platform.platform_telemetry_id), PLATFORM_ID);
        return READ_FAIL;
    }

    if (!READ_FIELD(file, &out->platform.uptime_s))              return READ_FAIL;
    if (!READ_FIELD(file, &out->platform.rtc_s))                 return READ_FAIL;
    if (!READ_FIELD(file, &out->platform.resetCount))            return READ_FAIL;
    if (!READ_FIELD(file, &out->platform.currentMode))           return READ_FAIL;
    if (!READ_FIELD(file, &out->platform.lastBootReason))        return READ_FAIL;

    /* MEMORY */
    if (!READ_FIELD(file, &out->memory.memory_telemetry_id))     return READ_FAIL;

    if (MEMORY_ID != (IS_BIG_ENDIAN?byte16_swap(out->memory.memory_telemetry_id):out->memory.memory_telemetry_id))
    {
        printf("WRONG MEMORY ID IN FRAME, read %x, expected %x", byte16_swap(out->memory.memory_telemetry_id), MEMORY_ID);
        return READ_FAIL;
    }

    if (!READ_FIELD(file, &out->memory.heap_free_bytes))         return READ_FAIL;

    /* CDH */
    if (!READ_FIELD(file, &out->cdh.cdh_id))                     return READ_FAIL;

    if (CDH_ID != (IS_BIG_ENDIAN?byte16_swap(out->cdh.cdh_id):out->cdh.cdh_id))
    {
        printf("WRONG CDH ID IN FRAME, read %x, expected %x", byte16_swap(out->cdh.cdh_id), CDH_ID);
        return READ_FAIL;
    }

    if (!READ_FIELD(file, &out->cdh.lastSeenSequenceNumber))     return READ_FAIL;
    if (!READ_FIELD(file, &out->cdh.antennaDeployStatus))        return READ_FAIL;

    /* POWER */
    if (!READ_FIELD(file, &out->power.power_telemetry_id))       return READ_FAIL;

    if (POWER_ID != (IS_BIG_ENDIAN?byte16_swap(out->power.power_telemetry_id):out->power.power_telemetry_id))
    {
        printf("WRONG POWER ID IN FRAME, read %x, expected %x", byte16_swap(out->power.power_telemetry_id), POWER_ID);
        return READ_FAIL;
    }

    if (!READ_FIELD(file, &out->power.low_voltage_counter))      return READ_FAIL;
    if (!READ_FIELD(file, &out->power.nice_battery_mV))          return READ_FAIL;
    if (!READ_FIELD(file, &out->power.raw_battery_mV))           return READ_FAIL;
    if (!READ_FIELD(file, &out->power.battery_A))                return READ_FAIL;
    if (!READ_FIELD(file, &out->power.pcm_3v3_V))                return READ_FAIL;
    if (!READ_FIELD(file, &out->power.pcm_3v3_A))                return READ_FAIL;
    if (!READ_FIELD(file, &out->power.pcm_5v_V))                 return READ_FAIL;
    if (!READ_FIELD(file, &out->power.pcm_5v_A))                 return READ_FAIL;

    /* THERMAL */
    if (!READ_FIELD(file, &out->thermal.thermal_telemetry_id))   return READ_FAIL;

    if (THERMAL_ID != (IS_BIG_ENDIAN?byte16_swap(out->thermal.thermal_telemetry_id):out->thermal.thermal_telemetry_id))
    {
        printf("WRONG THERMAL ID IN FRAME, read %x, expected %x", byte16_swap(out->thermal.thermal_telemetry_id), THERMAL_ID);
        return READ_FAIL;
    }

    if (!READ_FIELD(file, &out->thermal.CPU_C))                  return READ_FAIL;
    if (!READ_FIELD(file, &out->thermal.mirror_cell_C))          return READ_FAIL;

    /* AOCS */
    if (!READ_FIELD(file, &out->aocs.aocs_telemetry_id))         return READ_FAIL;

    if (AOCS_ID != (IS_BIG_ENDIAN?byte16_swap(out->aocs.aocs_telemetry_id):out->aocs.aocs_telemetry_id))
    {
        printf("WRONG AOCS ID IN FRAME, read %x, expected %x", byte16_swap(out->aocs.aocs_telemetry_id), AOCS_ID);
        return READ_FAIL;
    }

    if (!READ_FIELD(file, &out->aocs.aocs_mode))                 return READ_FAIL;
    if (!READ_FIELD(file, &out->aocs.sunvectorX))                return READ_FAIL;
    if (!READ_FIELD(file, &out->aocs.sunvectorY))                return READ_FAIL;
    if (!READ_FIELD(file, &out->aocs.sunvectorZ))                return READ_FAIL;
    if (!READ_FIELD(file, &out->aocs.magnetometerX_mg))          return READ_FAIL;
    if (!READ_FIELD(file, &out->aocs.magnetometerY_mg))          return READ_FAIL;
    if (!READ_FIELD(file, &out->aocs.magnetometerZ_mg))          return READ_FAIL;
    if (!READ_FIELD(file, &out->aocs.gyroX_dps))                 return READ_FAIL;
    if (!READ_FIELD(file, &out->aocs.gyroY_dps))                 return READ_FAIL;
    if (!READ_FIELD(file, &out->aocs.gyroZ_dps))                 return READ_FAIL;
    if (!READ_FIELD(file, &out->aocs.temperature_IMU_C))         return READ_FAIL;
    if (!READ_FIELD(file, &out->aocs.fine_gyroX_dps))            return READ_FAIL;
    if (!READ_FIELD(file, &out->aocs.fine_gyroY_dps))            return READ_FAIL;
    if (!READ_FIELD(file, &out->aocs.fine_gyroZ_dps))            return READ_FAIL;
    if (!READ_FIELD(file, &out->aocs.wheel_1_radsec))            return READ_FAIL;
    if (!READ_FIELD(file, &out->aocs.wheel_2_radsec))            return READ_FAIL;
    if (!READ_FIELD(file, &out->aocs.wheel_3_radsec))            return READ_FAIL;
    if (!READ_FIELD(file, &out->aocs.wheel_4_radsec))            return READ_FAIL;

    /* PAYLOAD */
    if (!READ_FIELD(file, &out->payload.payload_telemetry_id))   return READ_FAIL;

    if (PAYLOAD_ID != (IS_BIG_ENDIAN?byte16_swap(out->payload.payload_telemetry_id):out->payload.payload_telemetry_id))
    {
        printf("WRONG PAYLOAD ID IN FRAME, read %x, expected %x", byte16_swap(out->payload.payload_telemetry_id), PAYLOAD_ID);
        return READ_FAIL;
    }

    if (!READ_FIELD(file, &out->payload.experimentsRun))         return READ_FAIL;
    if (!READ_FIELD(file, &out->payload.experimentsFailed))      return READ_FAIL;
    if (!READ_FIELD(file, &out->payload.lastExperimentRun))      return READ_FAIL;
    if (!READ_FIELD(file, &out->payload.currentState))           return READ_FAIL;

    // all fields read correctly
    return READ_OK;
}
