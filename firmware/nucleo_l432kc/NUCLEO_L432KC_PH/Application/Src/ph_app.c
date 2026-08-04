#include "ph_app.h"

#include <stddef.h>

ph_status_t ph_app_init(
    ph_app_t *app,
    const ph_dependencies_t *dependencies)
{
    ph_channel_id_t channel_id;

    if ((app == NULL) || (dependencies == NULL)) {
        return PH_STATUS_INVALID_ARGUMENT;
    }

    for (channel_id = 0U; channel_id < PH_CHANNEL_COUNT; ++channel_id) {
        app->channels[channel_id].latest_sample.job_id = 0U;
        app->channels[channel_id].latest_sample.sample_index = 0U;
        app->channels[channel_id].latest_sample.timestamp_ms = 0U;
        app->channels[channel_id].latest_sample.channel_id = channel_id;
        app->channels[channel_id].latest_sample.raw_adc = 0U;
        app->channels[channel_id].latest_sample.vph_uv = 0;
        app->channels[channel_id].latest_sample.ph_milli = 0;
        app->channels[channel_id].latest_sample.status =
            PH_STATUS_NOT_IMPLEMENTED;
        app->channels[channel_id].has_sample = false;
        app->channels[channel_id].status = PH_STATUS_NOT_IMPLEMENTED;
    }

    return ph_fsm_init(&app->fsm, dependencies);
}

ph_status_t ph_app_process(ph_app_t *app)
{
    if (app == NULL) {
        return PH_STATUS_INVALID_ARGUMENT;
    }

    return ph_fsm_process(&app->fsm);
}
