#ifndef SIC8250_H
#define SIC8250_H

#include <stdbool.h>
#include <stdint.h>

#include "ph_board_port.h"

typedef enum {
    SIC8250_STATUS_OK = 0,
    SIC8250_STATUS_NOT_IMPLEMENTED,
    SIC8250_STATUS_INVALID_ARGUMENT,
    SIC8250_STATUS_IO_ERROR
} sic8250_status_t;

typedef struct {
    const ph_board_port_t *port;
} sic8250_device_t;

sic8250_status_t sic8250_init(sic8250_device_t *device);
sic8250_status_t sic8250_configure_measurement(
    sic8250_device_t *device,
    uint8_t channel_id);
sic8250_status_t sic8250_start_measurement(sic8250_device_t *device);
sic8250_status_t sic8250_check_ready(
    sic8250_device_t *device,
    bool *ready);
sic8250_status_t sic8250_read_adc(
    sic8250_device_t *device,
    uint32_t *raw_adc);
sic8250_status_t sic8250_stop(sic8250_device_t *device);

#endif /* SIC8250_H */
