#include "ph_fsm.h"

#include <stddef.h>

ph_status_t ph_fsm_init(
    ph_fsm_t *fsm,
    const ph_dependencies_t *dependencies)
{
    if ((fsm == NULL) ||
        (dependencies == NULL) ||
        (dependencies->device == NULL)) {
        return PH_STATUS_INVALID_ARGUMENT;
    }

    fsm->state = PH_STATE_SAFE_IDLE;
    fsm->active_channel = 0U;
    fsm->last_status = PH_STATUS_NOT_IMPLEMENTED;
    fsm->device = dependencies->device;

    return PH_STATUS_NOT_IMPLEMENTED;
}

ph_status_t ph_fsm_process(ph_fsm_t *fsm)
{
    if (fsm == NULL) {
        return PH_STATUS_INVALID_ARGUMENT;
    }

    return PH_STATUS_NOT_IMPLEMENTED;
}

ph_status_t ph_fsm_stop(ph_fsm_t *fsm)
{
    if (fsm == NULL) {
        return PH_STATUS_INVALID_ARGUMENT;
    }

    fsm->state = PH_STATE_SAFE_IDLE;
    fsm->active_channel = 0U;
    fsm->last_status = PH_STATUS_NOT_IMPLEMENTED;

    return PH_STATUS_NOT_IMPLEMENTED;
}

ph_status_t ph_fsm_get_status(const ph_fsm_t *fsm)
{
    if (fsm == NULL) {
        return PH_STATUS_INVALID_ARGUMENT;
    }

    return fsm->last_status;
}
