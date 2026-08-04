#include "ph_protocol.h"

#include <stddef.h>
#include <string.h>

void ph_protocol_parser_init(ph_protocol_parser_t *parser)
{
    if (parser == NULL) {
        return;
    }

    (void)memset(parser, 0, sizeof(*parser));
    parser->state = PH_PROTOCOL_PARSE_SOF_0;
}

ph_protocol_status_t ph_protocol_parser_push(
    ph_protocol_parser_t *parser,
    uint8_t byte,
    ph_protocol_frame_t *frame,
    bool *frame_available)
{
    (void)byte;

    if ((parser == NULL) || (frame == NULL) ||
        (frame_available == NULL)) {
        return PH_PROTOCOL_STATUS_INVALID_ARGUMENT;
    }

    *frame_available = false;

    /* TODO: Implement the byte-wise, resynchronizing frame parser. */
    return PH_PROTOCOL_STATUS_NOT_IMPLEMENTED;
}

ph_protocol_status_t ph_protocol_encode(
    const ph_protocol_frame_t *frame,
    uint8_t *output,
    uint16_t output_capacity,
    uint16_t *output_length)
{
    (void)output_capacity;

    if ((frame == NULL) || (output == NULL) ||
        (output_length == NULL)) {
        return PH_PROTOCOL_STATUS_INVALID_ARGUMENT;
    }

    *output_length = 0U;

    /* TODO: Serialize little-endian fields and append CRC-16/CCITT-FALSE. */
    return PH_PROTOCOL_STATUS_NOT_IMPLEMENTED;
}

uint16_t ph_protocol_crc16(const uint8_t *data, uint16_t length)
{
    (void)data;
    (void)length;

    /* TODO: Implement CRC-16/CCITT-FALSE, polynomial 0x1021. */
    return 0xFFFFU;
}
