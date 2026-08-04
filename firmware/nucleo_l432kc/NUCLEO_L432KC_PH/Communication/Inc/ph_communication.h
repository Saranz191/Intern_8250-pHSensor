#ifndef PH_COMMUNICATION_H
#define PH_COMMUNICATION_H

#include <stdbool.h>
#include <stdint.h>

#include "ph_protocol.h"

#define PH_COMM_RX_QUEUE_DEPTH (2U)
#define PH_COMM_TX_QUEUE_DEPTH (4U)

typedef enum {
    PH_COMM_STATUS_OK = 0,
    PH_COMM_STATUS_NOT_IMPLEMENTED,
    PH_COMM_STATUS_NOT_CONFIGURED,
    PH_COMM_STATUS_INVALID_ARGUMENT,
    PH_COMM_STATUS_BUSY,
    PH_COMM_STATUS_IO_ERROR,
    PH_COMM_STATUS_QUEUE_FULL
} ph_comm_status_t;

typedef struct {
    void *context;
    ph_comm_status_t (*read_byte)(
        void *context,
        uint8_t *byte,
        bool *available);
    ph_comm_status_t (*write)(
        void *context,
        const uint8_t *data,
        uint16_t length);
    bool (*tx_busy)(void *context);
} ph_comm_port_t;

typedef struct {
    const ph_comm_port_t *port;
    ph_protocol_parser_t parser;
    ph_protocol_frame_t rx_queue[PH_COMM_RX_QUEUE_DEPTH];
    ph_protocol_frame_t tx_queue[PH_COMM_TX_QUEUE_DEPTH];
    uint8_t rx_head;
    uint8_t rx_tail;
    uint8_t rx_count;
    uint8_t tx_head;
    uint8_t tx_tail;
    uint8_t tx_count;
    uint8_t tx_buffer[PH_PROTOCOL_MAX_FRAME_SIZE];
    bool initialized;
} ph_communication_t;

ph_comm_status_t ph_communication_init(
    ph_communication_t *communication,
    const ph_comm_port_t *port);
ph_comm_status_t ph_communication_process_rx(
    ph_communication_t *communication);
ph_comm_status_t ph_communication_process_tx(
    ph_communication_t *communication);
ph_comm_status_t ph_communication_take_frame(
    ph_communication_t *communication,
    ph_protocol_frame_t *frame,
    bool *available);
ph_comm_status_t ph_communication_queue_frame(
    ph_communication_t *communication,
    const ph_protocol_frame_t *frame);

#endif /* PH_COMMUNICATION_H */
