#include "ph_board_port.h"

#include <stddef.h>

static ph_board_status_t ph_board_spi_write_reg(
    void *context,
    uint8_t address,
    uint8_t value)
{
    (void)context;
    (void)address;
    (void)value;

    return PH_BOARD_STATUS_NOT_CONFIGURED;
}

static ph_board_status_t ph_board_spi_read_reg(
    void *context,
    uint8_t address,
    uint8_t *value)
{
    (void)context;
    (void)address;
    (void)value;

    return PH_BOARD_STATUS_NOT_CONFIGURED;
}

static uint32_t ph_board_millis(void *context)
{
    (void)context;

    return 0U;
}

static ph_board_status_t ph_board_set_power(void *context, bool enabled)
{
    (void)context;
    (void)enabled;

    return PH_BOARD_STATUS_NOT_CONFIGURED;
}

static ph_board_status_t ph_board_set_reset(void *context, bool asserted)
{
    (void)context;
    (void)asserted;

    return PH_BOARD_STATUS_NOT_CONFIGURED;
}

const ph_board_port_t *ph_board_port_default(void)
{
    static const ph_board_port_t port = {
        .context = NULL,
        .spi_write_reg = ph_board_spi_write_reg,
        .spi_read_reg = ph_board_spi_read_reg,
        .millis = ph_board_millis,
        .set_power = ph_board_set_power,
        .set_reset = ph_board_set_reset
    };

    return &port;
}
