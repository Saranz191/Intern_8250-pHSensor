#include "ph_comm_board_port.h"

#include <stddef.h>

static ph_comm_status_t ph_comm_board_read_byte(
    void *context,
    uint8_t *byte,
    bool *available)
{
    (void)context;

    if ((byte == NULL) || (available == NULL)) {
        return PH_COMM_STATUS_INVALID_ARGUMENT;
    }

    *byte = 0U;
    *available = false;
    return PH_COMM_STATUS_NOT_CONFIGURED;
}

static ph_comm_status_t ph_comm_board_write(
    void *context,
    const uint8_t *data,
    uint16_t length)
{
    (void)context;
    (void)length;

    if (data == NULL) {
        return PH_COMM_STATUS_INVALID_ARGUMENT;
    }

    return PH_COMM_STATUS_NOT_CONFIGURED;
}

static bool ph_comm_board_tx_busy(void *context)
{
    (void)context;
    return false;
}

const ph_comm_port_t *ph_comm_board_port_default(void)
{
    static const ph_comm_port_t port = {
        .context = NULL,
        .read_byte = ph_comm_board_read_byte,
        .write = ph_comm_board_write,
        .tx_busy = ph_comm_board_tx_busy
    };

    return &port;
}
