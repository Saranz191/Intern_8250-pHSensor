#ifndef PH_TYPES_H
#define PH_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t ph_channel_id_t;

typedef enum {
    PH_STATUS_OK = 0,
    PH_STATUS_NOT_IMPLEMENTED,
    PH_STATUS_NOT_CONFIGURED,
    PH_STATUS_INVALID_ARGUMENT,
    PH_STATUS_INVALID_STATE,
    PH_STATUS_BUSY,
    PH_STATUS_IO_ERROR,
    PH_STATUS_TIMEOUT,
    PH_STATUS_INVALID_SAMPLE,
    PH_STATUS_BUFFER_OVERFLOW,
    PH_STATUS_PROTOCOL_ERROR
} ph_status_t;

typedef enum {
    PH_STATE_INIT = 0,
    PH_STATE_IDLE,
    PH_STATE_START_CHANNEL,
    PH_STATE_WAIT_RESULT,
    PH_STATE_WAIT_CYCLE,
    PH_STATE_RECOVER,
    PH_STATE_FAULT
} ph_state_t;

typedef enum {
    PH_SAMPLE_MODE_SINGLE_CYCLE = 0,
    PH_SAMPLE_MODE_CONTINUOUS,
    PH_SAMPLE_MODE_FINITE
} ph_sample_mode_t;

typedef struct {
    uint8_t channel_mask;
    ph_sample_mode_t mode;
    uint16_t sample_count;
    uint32_t sample_period_ms;
} ph_measurement_config_t;

typedef struct {
    uint32_t job_id;
    uint32_t sample_index;
    uint32_t timestamp_ms;
    ph_channel_id_t channel_id;
    uint32_t raw_adc;
    int32_t vph_uv;
    int32_t ph_milli;
    ph_status_t status;
} ph_sample_t;

typedef struct {
    ph_sample_t latest_sample;
    bool has_sample;
    ph_status_t status;
} ph_channel_context_t;

#endif /* PH_TYPES_H */
