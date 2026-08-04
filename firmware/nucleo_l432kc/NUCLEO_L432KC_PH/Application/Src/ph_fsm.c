#include "ph_fsm.h"

#include <stddef.h>
#include <string.h>

ph_status_t ph_fsm_init(
    ph_fsm_t *fsm,
    const ph_dependencies_t *dependencies)
{
    if ((fsm == NULL) ||
        (dependencies == NULL) ||
        (dependencies->device == NULL)) {
        return PH_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(fsm, 0, sizeof(*fsm));
    fsm->state = PH_STATE_INIT;
    fsm->channel_phase = PH_CHANNEL_PHASE_CONFIGURE;
    fsm->configured.sample_period_ms = PH_DEFAULT_SAMPLE_PERIOD_MS;
    fsm->active_channel = PH_CHANNEL_NONE;
    fsm->retry_limit = PH_DEFAULT_RETRY_LIMIT;
    fsm->last_status = PH_STATUS_NOT_IMPLEMENTED;
    fsm->device = dependencies->device;

    return PH_STATUS_OK;
}

ph_status_t ph_fsm_set_config(
    ph_fsm_t *fsm,
    const ph_measurement_config_t *config)
{
    if ((fsm == NULL) || (config == NULL)) {
        return PH_STATUS_INVALID_ARGUMENT;
    }

    /* TODO: Validate and snapshot the host configuration in IDLE. */
    return PH_STATUS_NOT_IMPLEMENTED;
}

ph_status_t ph_fsm_start(ph_fsm_t *fsm, uint32_t job_id)
{
    (void)job_id;

    if (fsm == NULL) {
        return PH_STATUS_INVALID_ARGUMENT;
    }

    /* TODO: Guard IDLE, copy the channel mask, and enter START_CHANNEL. */
    return PH_STATUS_NOT_IMPLEMENTED;
}

ph_status_t ph_fsm_process(ph_fsm_t *fsm)
{
    if (fsm == NULL) {
        return PH_STATUS_INVALID_ARGUMENT;
    }

    /*
     * TODO: Implement one bounded transition step for the seven-state FSM.
     * This safe baseline performs no SIC8250 application I/O.
     */
    return PH_STATUS_NOT_IMPLEMENTED;
}

ph_status_t ph_fsm_stop(ph_fsm_t *fsm)
{
    if (fsm == NULL) {
        return PH_STATUS_INVALID_ARGUMENT;
    }

    /* TODO: Stop SIC8250 safely before clearing the active job and entering IDLE. */
    return PH_STATUS_NOT_IMPLEMENTED;
}

ph_status_t ph_fsm_take_sample(
    ph_fsm_t *fsm,
    ph_sample_t *sample,
    bool *available)
{
    if ((fsm == NULL) || (sample == NULL) || (available == NULL)) {
        return PH_STATUS_INVALID_ARGUMENT;
    }

    *available = false;

    /* TODO: Return and clear a completed channel-tagged sample. */
    return PH_STATUS_NOT_IMPLEMENTED;
}

ph_state_t ph_fsm_get_state(const ph_fsm_t *fsm)
{
    if (fsm == NULL) {
        return PH_STATE_FAULT;
    }

    return fsm->state;
}

ph_status_t ph_fsm_get_status(const ph_fsm_t *fsm)
{
    if (fsm == NULL) {
        return PH_STATUS_INVALID_ARGUMENT;
    }

    return fsm->last_status;
}
