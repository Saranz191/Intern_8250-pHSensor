#ifndef PH_COMMAND_DISPATCHER_H
#define PH_COMMAND_DISPATCHER_H

#include "ph_app.h"
#include "ph_protocol.h"

ph_status_t ph_command_dispatch(
    ph_app_t *app,
    const ph_protocol_frame_t *request,
    ph_protocol_frame_t *response);

#endif /* PH_COMMAND_DISPATCHER_H */
