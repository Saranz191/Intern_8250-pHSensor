#ifndef PH_FSM_H
#define PH_FSM_H

#include <stdbool.h>

#include "ph_config.h"
#include "ph_types.h"
#include "sic8250.h"

typedef struct {
    sic8250_device_t *device;
} ph_dependencies_t;

typedef enum {
    PH_CHANNEL_PHASE_CONFIGURE = 0,
    PH_CHANNEL_PHASE_SETTLING
} ph_channel_phase_t;

typedef struct {
    ph_state_t state;
    ph_channel_phase_t channel_phase;
    ph_measurement_config_t configured;
    uint8_t active_channel_mask;
    uint8_t remaining_channel_mask;
    ph_channel_id_t active_channel;
    uint32_t job_id;
    uint32_t sample_index;
    uint32_t settle_deadline_ms;
    uint32_t adc_deadline_ms;
    uint32_t next_cycle_deadline_ms;
    uint8_t retry_count;
    uint8_t retry_limit;
    uint32_t raw_adc;
    ph_sample_t pending_sample;
    bool sample_pending;
    ph_status_t last_status;
    sic8250_device_t *device;
} ph_fsm_t;

ph_status_t ph_fsm_init(
    ph_fsm_t *fsm,
    const ph_dependencies_t *dependencies);
ph_status_t ph_fsm_set_config(
    ph_fsm_t *fsm,
    const ph_measurement_config_t *config);
ph_status_t ph_fsm_start(ph_fsm_t *fsm, uint32_t job_id);
ph_status_t ph_fsm_process(ph_fsm_t *fsm);
ph_status_t ph_fsm_stop(ph_fsm_t *fsm);
ph_status_t ph_fsm_take_sample(
    ph_fsm_t *fsm,
    ph_sample_t *sample,
    bool *available);
ph_state_t ph_fsm_get_state(const ph_fsm_t *fsm);
ph_status_t ph_fsm_get_status(const ph_fsm_t *fsm);

#endif /* PH_FSM_H */
