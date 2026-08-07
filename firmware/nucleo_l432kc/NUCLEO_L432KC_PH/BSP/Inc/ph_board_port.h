#ifndef PH_BOARD_PORT_H
#define PH_BOARD_PORT_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    PH_BOARD_STATUS_OK = 0,
    PH_BOARD_STATUS_NOT_CONFIGURED,
    PH_BOARD_STATUS_INVALID_ARGUMENT,
    PH_BOARD_STATUS_IO_ERROR
} ph_board_status_t;

typedef struct {
    void *context;
    ph_board_status_t (*spi_write_reg)(
        void *context,
        uint8_t address,
        uint8_t value);
    ph_board_status_t (*spi_read_reg)(
        void *context,
        uint8_t address,
        uint8_t *value);
    uint32_t (*millis)(void *context);
    void (*delay_ms)(void *context, uint32_t delay_ms);
    ph_board_status_t (*set_power)(void *context, bool enabled);
    ph_board_status_t (*set_reset)(void *context, bool asserted);
} ph_board_port_t;

const ph_board_port_t *ph_board_port_default(void);

#endif /* PH_BOARD_PORT_H */
