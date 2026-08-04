#include "sic8250.h"

#include <stddef.h>

sic8250_status_t sic8250_init(sic8250_device_t *device)
{
    if (device == NULL) {
        return SIC8250_STATUS_INVALID_ARGUMENT;
    }

    return SIC8250_STATUS_NOT_IMPLEMENTED;
}

sic8250_status_t sic8250_load_defaults(sic8250_device_t *device)
{
    if (device == NULL) {
        return SIC8250_STATUS_INVALID_ARGUMENT;
    }

    return SIC8250_STATUS_NOT_IMPLEMENTED;
}

sic8250_status_t sic8250_configure_measurement(
    sic8250_device_t *device,
    uint8_t channel_id)
{
    (void)channel_id;

    if (device == NULL) {
        return SIC8250_STATUS_INVALID_ARGUMENT;
    }

    return SIC8250_STATUS_NOT_IMPLEMENTED;
}

sic8250_status_t sic8250_start_measurement(sic8250_device_t *device)
{
    if (device == NULL) {
        return SIC8250_STATUS_INVALID_ARGUMENT;
    }

    return SIC8250_STATUS_NOT_IMPLEMENTED;
}

sic8250_status_t sic8250_check_ready(
    sic8250_device_t *device,
    uint8_t channel_id,
    bool *ready)
{
    (void)channel_id;

    if (ready != NULL) {
        *ready = false;
    }

    if ((device == NULL) || (ready == NULL)) {
        return SIC8250_STATUS_INVALID_ARGUMENT;
    }

    return SIC8250_STATUS_NOT_IMPLEMENTED;
}

sic8250_status_t sic8250_read_adc(
    sic8250_device_t *device,
    uint8_t channel_id,
    uint32_t *raw_adc)
{
    (void)channel_id;

    if (raw_adc != NULL) {
        *raw_adc = 0U;
    }

    if ((device == NULL) || (raw_adc == NULL)) {
        return SIC8250_STATUS_INVALID_ARGUMENT;
    }

    return SIC8250_STATUS_NOT_IMPLEMENTED;
}

sic8250_status_t sic8250_stop(sic8250_device_t *device)
{
    if (device == NULL) {
        return SIC8250_STATUS_INVALID_ARGUMENT;
    }

    return SIC8250_STATUS_NOT_IMPLEMENTED;
}
