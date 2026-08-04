#include "ph_command_dispatcher.h"

#include <stddef.h>

ph_status_t ph_command_dispatch(
    ph_app_t *app,
    const ph_protocol_frame_t *request,
    ph_protocol_frame_t *response)
{
    if ((app == NULL) || (request == NULL) || (response == NULL)) {
        return PH_STATUS_INVALID_ARGUMENT;
    }

    /*
     * TODO: Validate command payloads, call the pH application API, and build
     * a response carrying the original sequence ID. Never access HAL or the
     * SIC8250 directly from this layer.
     */
    return PH_STATUS_NOT_IMPLEMENTED;
}
