#ifndef PH_PROTOCOL_H
#define PH_PROTOCOL_H

#include <stdbool.h>
#include <stdint.h>

#define PH_PROTOCOL_SOF_0 (0xAAU)
#define PH_PROTOCOL_SOF_1 (0x55U)
#define PH_PROTOCOL_VERSION (0x01U)
#define PH_PROTOCOL_HEADER_SIZE (11U)
#define PH_PROTOCOL_CRC_SIZE (2U)
#define PH_PROTOCOL_MAX_PAYLOAD (64U)
#define PH_PROTOCOL_MAX_FRAME_SIZE \
    (PH_PROTOCOL_HEADER_SIZE + PH_PROTOCOL_MAX_PAYLOAD + \
     PH_PROTOCOL_CRC_SIZE)

typedef enum {
    PH_PROTOCOL_MESSAGE_COMMAND = 0x00,
    PH_PROTOCOL_MESSAGE_RESPONSE = 0x01,
    PH_PROTOCOL_MESSAGE_EVENT = 0x02
} ph_protocol_message_type_t;

typedef enum {
    PH_PROTOCOL_COMMAND_PING = 0x01,
    PH_PROTOCOL_COMMAND_GET_INFO = 0x02,
    PH_PROTOCOL_COMMAND_SET_CONFIG = 0x10,
    PH_PROTOCOL_COMMAND_START = 0x11,
    PH_PROTOCOL_COMMAND_STOP = 0x12,
    PH_PROTOCOL_COMMAND_GET_STATUS = 0x13,
    PH_PROTOCOL_COMMAND_GET_LATEST = 0x14,
    PH_PROTOCOL_COMMAND_SET_CALIBRATION = 0x20,
    PH_PROTOCOL_COMMAND_GET_CALIBRATION = 0x21
} ph_protocol_command_t;

typedef enum {
    PH_PROTOCOL_EVENT_SAMPLE = 0x80,
    PH_PROTOCOL_EVENT_JOB_DONE = 0x81,
    PH_PROTOCOL_EVENT_ERROR = 0x82,
    PH_PROTOCOL_EVENT_OVERFLOW = 0x83
} ph_protocol_event_t;

typedef enum {
    PH_PROTOCOL_WIRE_STATUS_OK = 0x00,
    PH_PROTOCOL_WIRE_STATUS_UNKNOWN_COMMAND = 0x01,
    PH_PROTOCOL_WIRE_STATUS_INVALID_LENGTH = 0x02,
    PH_PROTOCOL_WIRE_STATUS_INVALID_PARAMETER = 0x03,
    PH_PROTOCOL_WIRE_STATUS_INVALID_STATE = 0x04,
    PH_PROTOCOL_WIRE_STATUS_BUSY = 0x05,
    PH_PROTOCOL_WIRE_STATUS_CRC_ERROR = 0x06,
    PH_PROTOCOL_WIRE_STATUS_TIMEOUT = 0x07,
    PH_PROTOCOL_WIRE_STATUS_DEVICE_IO_ERROR = 0x08,
    PH_PROTOCOL_WIRE_STATUS_BUFFER_OVERFLOW = 0x09,
    PH_PROTOCOL_WIRE_STATUS_INTERNAL_ERROR = 0x0A
} ph_protocol_wire_status_t;

typedef enum {
    PH_PROTOCOL_STATUS_OK = 0,
    PH_PROTOCOL_STATUS_NOT_IMPLEMENTED,
    PH_PROTOCOL_STATUS_INVALID_ARGUMENT,
    PH_PROTOCOL_STATUS_INCOMPLETE,
    PH_PROTOCOL_STATUS_INVALID_VERSION,
    PH_PROTOCOL_STATUS_INVALID_LENGTH,
    PH_PROTOCOL_STATUS_CRC_ERROR
} ph_protocol_status_t;

typedef enum {
    PH_PROTOCOL_PARSE_SOF_0 = 0,
    PH_PROTOCOL_PARSE_SOF_1,
    PH_PROTOCOL_PARSE_HEADER,
    PH_PROTOCOL_PARSE_PAYLOAD,
    PH_PROTOCOL_PARSE_CRC
} ph_protocol_parse_state_t;

typedef struct {
    uint8_t version;
    ph_protocol_message_type_t type;
    uint8_t flags;
    uint16_t sequence_id;
    uint8_t code;
    ph_protocol_wire_status_t status;
    uint16_t payload_length;
    uint8_t payload[PH_PROTOCOL_MAX_PAYLOAD];
} ph_protocol_frame_t;

typedef struct {
    ph_protocol_parse_state_t state;
    ph_protocol_frame_t frame;
    uint16_t header_index;
    uint16_t payload_index;
    uint8_t received_crc[PH_PROTOCOL_CRC_SIZE];
    uint8_t crc_index;
} ph_protocol_parser_t;

void ph_protocol_parser_init(ph_protocol_parser_t *parser);
ph_protocol_status_t ph_protocol_parser_push(
    ph_protocol_parser_t *parser,
    uint8_t byte,
    ph_protocol_frame_t *frame,
    bool *frame_available);
ph_protocol_status_t ph_protocol_encode(
    const ph_protocol_frame_t *frame,
    uint8_t *output,
    uint16_t output_capacity,
    uint16_t *output_length);
uint16_t ph_protocol_crc16(const uint8_t *data, uint16_t length);

#endif /* PH_PROTOCOL_H */
