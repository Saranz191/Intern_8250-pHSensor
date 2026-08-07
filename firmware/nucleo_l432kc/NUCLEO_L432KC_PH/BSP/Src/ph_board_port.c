#include "ph_board_port.h"

#include <stddef.h>
#include <string.h>

#include "main.h"
#include "spi.h"

#define SIC8250_HEADER             (0xAAU)
#define SIC8250_SFR_READ_LENGTH    (0x02U)
#define SIC8250_SFR_WRITE_LENGTH   (0x03U)
#define SIC8250_SFR_READ_COMMAND   (0xD0U)
#define SIC8250_SFR_WRITE_COMMAND  (0xD1U)
#define SIC8250_TRANSACTION_SIZE   (15U)
#define SIC8250_PADDING            (0xFFU)
#define SIC8250_SPI_TIMEOUT_MS     (50U)

static uint8_t ph_board_xor(const uint8_t *data, size_t length)
{
    uint8_t checksum = 0U;
    size_t index;

    for (index = 0U; index < length; ++index) {
        checksum ^= data[index];
    }

    return checksum;
}

static ph_board_status_t ph_board_transfer(
    SPI_HandleTypeDef *hspi,
    uint8_t tx[SIC8250_TRANSACTION_SIZE],
    uint8_t rx[SIC8250_TRANSACTION_SIZE])
{
    HAL_StatusTypeDef status;

    HAL_GPIO_WritePin(SIC8250_CS_GPIO_Port, SIC8250_CS_Pin, GPIO_PIN_RESET);
    status = HAL_SPI_TransmitReceive(
        hspi,
        tx,
        rx,
        SIC8250_TRANSACTION_SIZE,
        SIC8250_SPI_TIMEOUT_MS);
    HAL_GPIO_WritePin(SIC8250_CS_GPIO_Port, SIC8250_CS_Pin, GPIO_PIN_SET);

    return (status == HAL_OK) ? PH_BOARD_STATUS_OK : PH_BOARD_STATUS_IO_ERROR;
}

static ph_board_status_t ph_board_parse_response(
    const uint8_t rx[SIC8250_TRANSACTION_SIZE],
    uint8_t *value)
{
    size_t header_index;

    for (header_index = 0U;
         header_index < SIC8250_TRANSACTION_SIZE;
         ++header_index) {
        size_t checksum_index;
        uint8_t packet_length;
        uint8_t checksum;

        if (rx[header_index] != SIC8250_HEADER) {
            continue;
        }
        if ((header_index + 3U) >= SIC8250_TRANSACTION_SIZE) {
            return PH_BOARD_STATUS_IO_ERROR;
        }

        packet_length = rx[header_index + 1U];
        checksum_index = header_index + (size_t)packet_length + 2U;
        if ((packet_length < 3U) || (checksum_index >= SIC8250_TRANSACTION_SIZE)) {
            return PH_BOARD_STATUS_IO_ERROR;
        }

        checksum = ph_board_xor(
            &rx[header_index + 1U],
            checksum_index - header_index - 1U);
        if (checksum != rx[checksum_index]) {
            return PH_BOARD_STATUS_IO_ERROR;
        }

        *value = rx[header_index + 4U];
        return PH_BOARD_STATUS_OK;
    }

    return PH_BOARD_STATUS_IO_ERROR;
}

static ph_board_status_t ph_board_spi_write_reg(
    void *context,
    uint8_t address,
    uint8_t value)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)context;
    uint8_t tx[SIC8250_TRANSACTION_SIZE];
    uint8_t rx[SIC8250_TRANSACTION_SIZE] = {0U};
    uint8_t response_value = 0U;
    ph_board_status_t status;

    if (hspi == NULL) {
        return PH_BOARD_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(tx, SIC8250_PADDING, sizeof(tx));
    tx[0] = SIC8250_HEADER;
    tx[1] = SIC8250_SFR_WRITE_LENGTH;
    tx[2] = SIC8250_SFR_WRITE_COMMAND;
    tx[3] = address;
    tx[4] = value;
    tx[5] = ph_board_xor(&tx[1], 4U);

    status = ph_board_transfer(hspi, tx, rx);
    if (status != PH_BOARD_STATUS_OK) {
        return status;
    }

    return ph_board_parse_response(rx, &response_value);
}

static ph_board_status_t ph_board_spi_read_reg(
    void *context,
    uint8_t address,
    uint8_t *value)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)context;
    uint8_t tx[SIC8250_TRANSACTION_SIZE];
    uint8_t rx[SIC8250_TRANSACTION_SIZE] = {0U};
    ph_board_status_t status;

    if ((hspi == NULL) || (value == NULL)) {
        return PH_BOARD_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(tx, SIC8250_PADDING, sizeof(tx));
    tx[0] = SIC8250_HEADER;
    tx[1] = SIC8250_SFR_READ_LENGTH;
    tx[2] = SIC8250_SFR_READ_COMMAND;
    tx[3] = address;
    tx[4] = ph_board_xor(&tx[1], 3U);

    status = ph_board_transfer(hspi, tx, rx);
    if (status != PH_BOARD_STATUS_OK) {
        return status;
    }

    return ph_board_parse_response(rx, value);
}

static uint32_t ph_board_millis(void *context)
{
    (void)context;
    return HAL_GetTick();
}

static void ph_board_delay_ms(void *context, uint32_t delay_ms)
{
    (void)context;
    HAL_Delay(delay_ms);
}

static ph_board_status_t ph_board_set_power(void *context, bool enabled)
{
    (void)context;
    (void)enabled;

    /* SIC8250 power is supplied externally on this example wiring. */
    return PH_BOARD_STATUS_OK;
}

static ph_board_status_t ph_board_set_reset(void *context, bool asserted)
{
    (void)context;
    HAL_GPIO_WritePin(
        SIC8250_RESET_GPIO_Port,
        SIC8250_RESET_Pin,
        asserted ? GPIO_PIN_RESET : GPIO_PIN_SET);

    return PH_BOARD_STATUS_OK;
}

const ph_board_port_t *ph_board_port_default(void)
{
    static const ph_board_port_t port = {
        .context = &hspi1,
        .spi_write_reg = ph_board_spi_write_reg,
        .spi_read_reg = ph_board_spi_read_reg,
        .millis = ph_board_millis,
        .delay_ms = ph_board_delay_ms,
        .set_power = ph_board_set_power,
        .set_reset = ph_board_set_reset
    };

    return &port;
}
