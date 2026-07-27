#ifndef PH_TYPES_H
#define PH_TYPES_H

#include <stdint.h>

typedef uint8_t ph_channel_id_t;

typedef enum {
    PH_STATUS_OK = 0,
    PH_STATUS_NOT_IMPLEMENTED,
    PH_STATUS_INVALID_ARGUMENT,
    PH_STATUS_IO_ERROR,
    PH_STATUS_TIMEOUT
} ph_status_t;

typedef enum {
    PH_STATE_SAFE_IDLE = 0
} ph_state_t;

typedef struct {
    ph_channel_id_t channel_id;
    uint32_t raw_adc;
    float voltage;
    float ph;
    uint32_t timestamp_ms;
    ph_status_t status;
} ph_sample_t;

typedef struct {
    ph_sample_t latest_sample;
    ph_status_t status;
} ph_channel_context_t;

#endif /* PH_TYPES_H */
