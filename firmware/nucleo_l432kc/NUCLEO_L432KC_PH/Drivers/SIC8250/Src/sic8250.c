#include "sic8250.h"

#include <stddef.h>

#define SIC8250_CHANNEL_COUNT              (4U)
#define SIC8250_CHANNEL_NONE               (0xFFU)
#define SIC8250_MEASUREMENT_TIMEOUT_MS     (50U)

#define SIC8250_REG_ADC_READY              (0xB5U)
#define SIC8250_REG_ADC_TIMING_M_LSB       (0xB9U)
#define SIC8250_REG_ADC_TIMING_M_MSB       (0xBAU)
#define SIC8250_REG_ADC_TIMING_N           (0xBBU)
#define SIC8250_REG_ADC_DELAY_N            (0xBCU)
#define SIC8250_REG_ADC_DELAY_M_LSB        (0xBDU)
#define SIC8250_REG_ADC_DELAY_M_MSB        (0xBEU)
#define SIC8250_REG_RUN_POTENTIOSTAT       (0xBFU)
#define SIC8250_REG_WE_RANGE_01            (0xC1U)
#define SIC8250_REG_WE_RANGE_23            (0xC2U)
#define SIC8250_REG_WE_CONFIG              (0xC3U)
#define SIC8250_REG_ONE_SHOT               (0xC4U)
#define SIC8250_REG_POTENTIOSTAT_CONFIG    (0xD4U)
#define SIC8250_REG_ANALOG_CONFIG          (0xD5U)
#define SIC8250_REG_ADC_FRACTION           (0xE1U)
#define SIC8250_REG_ADC_LSB                (0xE2U)
#define SIC8250_REG_ADC_MSB                (0xE3U)
#define SIC8250_REG_TICK_TABLE             (0xFDU)
#define SIC8250_REG_TCFG_ADDRESS           (0xFEU)
#define SIC8250_REG_TCFG_DATA              (0xFFU)

#define SIC8250_ONE_SHOT_OCP_BASE          (0x18U)
#define SIC8250_RUN                         (0x80U)

typedef struct {
    uint8_t address;
    uint8_t value;
} sic8250_register_value_t;

static sic8250_status_t sic8250_from_board_status(ph_board_status_t status)
{
    return (status == PH_BOARD_STATUS_OK)
        ? SIC8250_STATUS_OK
        : SIC8250_STATUS_IO_ERROR;
}

static bool sic8250_port_is_valid(const ph_board_port_t *port)
{
    return (port != NULL) &&
           (port->spi_write_reg != NULL) &&
           (port->spi_read_reg != NULL) &&
           (port->millis != NULL) &&
           (port->delay_ms != NULL) &&
           (port->set_reset != NULL);
}

static sic8250_status_t sic8250_write_sfr(
    sic8250_device_t *device,
    uint8_t address,
    uint8_t value)
{
    return sic8250_from_board_status(
        device->port->spi_write_reg(device->port->context, address, value));
}

static sic8250_status_t sic8250_read_sfr(
    sic8250_device_t *device,
    uint8_t address,
    uint8_t *value)
{
    return sic8250_from_board_status(
        device->port->spi_read_reg(device->port->context, address, value));
}

static sic8250_status_t sic8250_write_tcfg(
    sic8250_device_t *device,
    uint8_t address,
    uint8_t value)
{
    sic8250_status_t status;

    status = sic8250_write_sfr(device, SIC8250_REG_TCFG_ADDRESS, address);
    if (status != SIC8250_STATUS_OK) {
        return status;
    }

    return sic8250_write_sfr(device, SIC8250_REG_TCFG_DATA, value);
}

static sic8250_status_t sic8250_write_sfr_sequence(
    sic8250_device_t *device,
    const sic8250_register_value_t *sequence,
    size_t count)
{
    size_t index;

    for (index = 0U; index < count; ++index) {
        sic8250_status_t status = sic8250_write_sfr(
            device,
            sequence[index].address,
            sequence[index].value);
        if (status != SIC8250_STATUS_OK) {
            return status;
        }
    }

    return SIC8250_STATUS_OK;
}

sic8250_status_t sic8250_init(sic8250_device_t *device)
{
    if ((device == NULL) || !sic8250_port_is_valid(device->port)) {
        return SIC8250_STATUS_INVALID_ARGUMENT;
    }

    device->active_channel = SIC8250_CHANNEL_NONE;
    device->ready_token = 0U;
    device->last_adc_fraction = 0U;
    device->measurement_started_ms = 0U;
    device->measurement_running = false;

    if ((device->port->set_power != NULL) &&
        (device->port->set_power(device->port->context, true) != PH_BOARD_STATUS_OK)) {
        return SIC8250_STATUS_IO_ERROR;
    }
    if (device->port->set_reset(device->port->context, true) != PH_BOARD_STATUS_OK) {
        return SIC8250_STATUS_IO_ERROR;
    }
    device->port->delay_ms(device->port->context, 10U);
    if (device->port->set_reset(device->port->context, false) != PH_BOARD_STATUS_OK) {
        return SIC8250_STATUS_IO_ERROR;
    }
    device->port->delay_ms(device->port->context, 10U);

    return SIC8250_STATUS_OK;
}

sic8250_status_t sic8250_load_defaults(sic8250_device_t *device)
{
    static const uint8_t defaults[] = {
        0x00U, 0x56U, 0xF6U, 0x49U, 0x82U, 0x1BU, 0x00U,
        0x00U, 0x00U, 0x02U, 0x00U, 0x20U, 0x10U, 0x10U,
        0x20U, 0x00U, 0xAAU, 0x0AU, 0x10U, 0x10U, 0x06U
    };
    size_t index;

    if ((device == NULL) || !sic8250_port_is_valid(device->port)) {
        return SIC8250_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < (sizeof(defaults) / sizeof(defaults[0])); ++index) {
        sic8250_status_t status = sic8250_write_tcfg(
            device,
            (uint8_t)(index + 1U),
            defaults[index]);
        if (status != SIC8250_STATUS_OK) {
            return status;
        }
    }

    return SIC8250_STATUS_OK;
}

sic8250_status_t sic8250_configure_measurement(
    sic8250_device_t *device,
    uint8_t channel_id)
{
    static const sic8250_register_value_t profile[] = {
        {SIC8250_REG_ADC_TIMING_M_LSB, 0xCCU},
        {SIC8250_REG_ADC_TIMING_M_MSB, 0x01U},
        {SIC8250_REG_ADC_TIMING_N, 0x20U},
        {SIC8250_REG_ADC_DELAY_N, 0x10U},
        {SIC8250_REG_ADC_DELAY_M_LSB, 0x04U},
        {SIC8250_REG_ADC_DELAY_M_MSB, 0x00U},
        {SIC8250_REG_ANALOG_CONFIG, 0x0CU},
        {SIC8250_REG_POTENTIOSTAT_CONFIG, 0x33U},
        {SIC8250_REG_TICK_TABLE, 0x09U}
    };
    static const sic8250_register_value_t tcfg_overrides[] = {
        {0x08U, 0xC0U}, {0x0BU, 0x8AU}, {0x11U, 0xAAU},
        {0x12U, 0x0AU}, {0x02U, 0x56U}, {0x03U, 0x00U},
        {0x0DU, 0x07U}
    };
    size_t index;
    sic8250_status_t status;

    if ((device == NULL) || !sic8250_port_is_valid(device->port) ||
        (channel_id >= SIC8250_CHANNEL_COUNT)) {
        return SIC8250_STATUS_INVALID_ARGUMENT;
    }

    status = sic8250_write_sfr_sequence(
        device,
        profile,
        sizeof(profile) / sizeof(profile[0]));
    if (status != SIC8250_STATUS_OK) {
        return status;
    }

    for (index = 0U;
         index < (sizeof(tcfg_overrides) / sizeof(tcfg_overrides[0]));
         ++index) {
        status = sic8250_write_tcfg(
            device,
            tcfg_overrides[index].address,
            tcfg_overrides[index].value);
        if (status != SIC8250_STATUS_OK) {
            return status;
        }
    }

    status = sic8250_write_sfr(device, SIC8250_REG_WE_RANGE_01, 0x00U);
    if (status == SIC8250_STATUS_OK) {
        status = sic8250_write_sfr(device, SIC8250_REG_WE_RANGE_23, 0x00U);
    }
    if (status == SIC8250_STATUS_OK) {
        status = sic8250_write_sfr(device, SIC8250_REG_WE_CONFIG, 0x00U);
    }
    if (status == SIC8250_STATUS_OK) {
        status = sic8250_write_sfr(
            device,
            SIC8250_REG_ONE_SHOT,
            (uint8_t)(SIC8250_ONE_SHOT_OCP_BASE | (channel_id << 5U)));
    }
    if (status != SIC8250_STATUS_OK) {
        return status;
    }

    device->active_channel = channel_id;
    device->measurement_running = false;
    return SIC8250_STATUS_OK;
}

sic8250_status_t sic8250_start_measurement(sic8250_device_t *device)
{
    sic8250_status_t status;

    if ((device == NULL) || !sic8250_port_is_valid(device->port) ||
        (device->active_channel >= SIC8250_CHANNEL_COUNT)) {
        return SIC8250_STATUS_INVALID_ARGUMENT;
    }

    status = sic8250_read_sfr(device, SIC8250_REG_ADC_READY, &device->ready_token);
    if (status != SIC8250_STATUS_OK) {
        return status;
    }
    device->ready_token &= 0xF0U;

    status = sic8250_write_sfr(
        device,
        SIC8250_REG_ONE_SHOT,
        (uint8_t)(SIC8250_ONE_SHOT_OCP_BASE | (device->active_channel << 5U)));
    if (status == SIC8250_STATUS_OK) {
        status = sic8250_write_sfr(device, SIC8250_REG_RUN_POTENTIOSTAT, SIC8250_RUN);
    }
    if (status != SIC8250_STATUS_OK) {
        return status;
    }

    device->measurement_started_ms = device->port->millis(device->port->context);
    device->measurement_running = true;
    return SIC8250_STATUS_OK;
}

sic8250_status_t sic8250_check_ready(
    sic8250_device_t *device,
    uint8_t channel_id,
    bool *ready)
{
    if (ready != NULL) {
        *ready = false;
    }

    if ((device == NULL) || (ready == NULL) ||
        !sic8250_port_is_valid(device->port) ||
        !device->measurement_running ||
        (channel_id != device->active_channel)) {
        return SIC8250_STATUS_INVALID_ARGUMENT;
    }

    {
        uint8_t current_token = 0U;
        sic8250_status_t status = sic8250_read_sfr(
            device,
            SIC8250_REG_ADC_READY,
            &current_token);
        if (status != SIC8250_STATUS_OK) {
            return status;
        }

        if ((current_token & 0xF0U) != device->ready_token) {
            *ready = true;
            return SIC8250_STATUS_OK;
        }
    }

    if ((uint32_t)(device->port->millis(device->port->context) -
                   device->measurement_started_ms) >= SIC8250_MEASUREMENT_TIMEOUT_MS) {
        return SIC8250_STATUS_TIMEOUT;
    }

    return SIC8250_STATUS_NOT_READY;
}

sic8250_status_t sic8250_read_adc(
    sic8250_device_t *device,
    uint8_t channel_id,
    uint32_t *raw_adc)
{
    if (raw_adc != NULL) {
        *raw_adc = 0U;
    }

    if ((device == NULL) || (raw_adc == NULL) ||
        !sic8250_port_is_valid(device->port) ||
        (channel_id != device->active_channel)) {
        return SIC8250_STATUS_INVALID_ARGUMENT;
    }

    {
        uint8_t adc_lsb = 0U;
        uint8_t adc_msb = 0U;
        sic8250_status_t status;

        status = sic8250_read_sfr(
            device,
            SIC8250_REG_ADC_FRACTION,
            &device->last_adc_fraction);
        if (status == SIC8250_STATUS_OK) {
            status = sic8250_read_sfr(device, SIC8250_REG_ADC_LSB, &adc_lsb);
        }
        if (status == SIC8250_STATUS_OK) {
            status = sic8250_read_sfr(device, SIC8250_REG_ADC_MSB, &adc_msb);
        }
        if (status != SIC8250_STATUS_OK) {
            return status;
        }

        *raw_adc = ((uint32_t)adc_msb << 8U) | (uint32_t)adc_lsb;
    }

    device->measurement_running = false;
    return SIC8250_STATUS_OK;
}

sic8250_status_t sic8250_stop(sic8250_device_t *device)
{
    sic8250_status_t status;

    if ((device == NULL) || !sic8250_port_is_valid(device->port)) {
        return SIC8250_STATUS_INVALID_ARGUMENT;
    }

    status = sic8250_write_sfr(device, SIC8250_REG_RUN_POTENTIOSTAT, 0x00U);
    if (status == SIC8250_STATUS_OK) {
        status = sic8250_write_sfr(device, SIC8250_REG_POTENTIOSTAT_CONFIG, 0x30U);
    }

    device->measurement_running = false;
    return status;
}
