#ifndef PH_APP_H
#define PH_APP_H

#include "ph_config.h"
#include "ph_fsm.h"
#include "ph_types.h"

typedef struct {
    ph_fsm_t fsm;
    ph_channel_context_t channels[PH_CHANNEL_COUNT];
} ph_app_t;

ph_status_t ph_app_init(
    ph_app_t *app,
    const ph_dependencies_t *dependencies);
ph_status_t ph_app_process(ph_app_t *app);

#endif /* PH_APP_H */
