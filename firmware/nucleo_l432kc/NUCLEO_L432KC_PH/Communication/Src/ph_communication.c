#include "ph_communication.h"

#include <stddef.h>
#include <string.h>

ph_comm_status_t ph_communication_init(
    ph_communication_t *communication,
    const ph_comm_port_t *port)
{
    if ((communication == NULL) || (port == NULL)) {
        return PH_COMM_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(communication, 0, sizeof(*communication));
    communication->port = port;
    communication->initialized = true;
    ph_protocol_parser_init(&communication->parser);

    return PH_COMM_STATUS_OK;
}

ph_comm_status_t ph_communication_process_rx(
    ph_communication_t *communication)
{
    if (communication == NULL) {
        return PH_COMM_STATUS_INVALID_ARGUMENT;
    }

    if ((!communication->initialized) ||
        (communication->port == NULL) ||
        (communication->port->read_byte == NULL)) {
        return PH_COMM_STATUS_NOT_CONFIGURED;
    }

    /* TODO: Drain transport bytes, parse frames, and enqueue complete RX frames. */
    return PH_COMM_STATUS_NOT_IMPLEMENTED;
}

ph_comm_status_t ph_communication_process_tx(
    ph_communication_t *communication)
{
    if (communication == NULL) {
        return PH_COMM_STATUS_INVALID_ARGUMENT;
    }

    if ((!communication->initialized) ||
        (communication->port == NULL) ||
        (communication->port->write == NULL) ||
        (communication->port->tx_busy == NULL)) {
        return PH_COMM_STATUS_NOT_CONFIGURED;
    }

    /* TODO: Encode and transmit one queued frame without blocking. */
    return PH_COMM_STATUS_NOT_IMPLEMENTED;
}

ph_comm_status_t ph_communication_take_frame(
    ph_communication_t *communication,
    ph_protocol_frame_t *frame,
    bool *available)
{
    if ((communication == NULL) || (frame == NULL) ||
        (available == NULL)) {
        return PH_COMM_STATUS_INVALID_ARGUMENT;
    }

    *available = false;

    /* TODO: Pop one complete frame from the RX queue. */
    return PH_COMM_STATUS_NOT_IMPLEMENTED;
}

ph_comm_status_t ph_communication_queue_frame(
    ph_communication_t *communication,
    const ph_protocol_frame_t *frame)
{
    if ((communication == NULL) || (frame == NULL)) {
        return PH_COMM_STATUS_INVALID_ARGUMENT;
    }

    /* TODO: Enqueue one response or event for the sole TX owner. */
    return PH_COMM_STATUS_NOT_IMPLEMENTED;
}
