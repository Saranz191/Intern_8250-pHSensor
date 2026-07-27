#ifndef PH_FSM_H
#define PH_FSM_H

#include "ph_types.h"
#include "sic8250.h"

typedef struct {
    sic8250_device_t *device;
} ph_dependencies_t;

typedef struct {
    ph_state_t state;
    ph_channel_id_t active_channel;
    ph_status_t last_status;
    sic8250_device_t *device;
} ph_fsm_t;

ph_status_t ph_fsm_init(
    ph_fsm_t *fsm,
    const ph_dependencies_t *dependencies);
ph_status_t ph_fsm_process(ph_fsm_t *fsm);
ph_status_t ph_fsm_stop(ph_fsm_t *fsm);
ph_status_t ph_fsm_get_status(const ph_fsm_t *fsm);

#endif /* PH_FSM_H */
