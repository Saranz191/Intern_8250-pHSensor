#include "sic8250.h"

#include <stddef.h>
#include <string.h>

#define SIC8250_HEADER              0xAAU
#define SIC8250_SFR_READ_LENGTH     0x02U
#define SIC8250_SFR_WRITE_LENGTH    0x03U
#define SIC8250_SFR_READ_COMMAND    0xD0U
#define SIC8250_SFR_WRITE_COMMAND   0xD1U
#define SIC8250_TCFG_ADDRESS_SFR    0xFEU
#define SIC8250_TCFG_DATA_SFR       0xFFU
#define SIC8250_TRANSACTION_SIZE    15U
#define SIC8250_PADDING             0xFFU
#define SIC8250_DEFAULT_TIMEOUT_MS  50U

static uint8_t SIC8250_Xor(const uint8_t *data, size_t length)
{
  uint8_t checksum = 0U;
  size_t index;

  for (index = 0U; index < length; ++index)
  {
    checksum ^= data[index];
  }

  return checksum;
}

static uint8_t SIC8250_IsReady(const SIC8250_Handle *device)
{
  return (device != NULL) &&(device->hspi != NULL) &&(device->cs_port != NULL) &&(device->reset_port != NULL);
}

static SIC8250_Status SIC8250_Transfer(SIC8250_Handle *device,uint8_t tx[SIC8250_TRANSACTION_SIZE],uint8_t rx[SIC8250_TRANSACTION_SIZE])
{
  HAL_StatusTypeDef hal_status;

  if (!SIC8250_IsReady(device))
  {
    return SIC8250_ERROR_ARGUMENT;
  }

  HAL_GPIO_WritePin(device->cs_port, device->cs_pin, GPIO_PIN_RESET);
  hal_status = HAL_SPI_TransmitReceive(device->hspi,tx,rx,SIC8250_TRANSACTION_SIZE,device->timeout_ms);
  HAL_GPIO_WritePin(device->cs_port, device->cs_pin, GPIO_PIN_SET);

  return (hal_status == HAL_OK) ? SIC8250_OK : SIC8250_ERROR_SPI;
}

static SIC8250_Status SIC8250_ParseResponse(SIC8250_Handle *device,const uint8_t rx[SIC8250_TRANSACTION_SIZE],uint8_t *value)
{
  size_t header_index;

  for (header_index = 0U;
       header_index < SIC8250_TRANSACTION_SIZE;
       ++header_index)
  {
    size_t checksum_index;
    uint8_t packet_length;
    uint8_t checksum;

    if (rx[header_index] != SIC8250_HEADER)
    {
      continue;
    }

    if ((header_index + 3U) >= SIC8250_TRANSACTION_SIZE)
    {
      return SIC8250_ERROR_FRAME;
    }

    packet_length = rx[header_index + 1U];
    checksum_index = header_index + (size_t)packet_length + 2U;

    /* A normal response contains command, flag and at least one data byte. */
    if ((packet_length < 3U) || (checksum_index >= SIC8250_TRANSACTION_SIZE))
    {
      return SIC8250_ERROR_FRAME;
    }

    checksum = SIC8250_Xor(&rx[header_index + 1U],checksum_index - header_index - 1U);
    if (checksum != rx[checksum_index])
    {
      return SIC8250_ERROR_CHECKSUM;
    }

    device->last_response_command = rx[header_index + 2U];
    device->last_response_flag = rx[header_index + 3U];
    *value = rx[header_index + 4U];
    return SIC8250_OK;
  }

  return SIC8250_ERROR_NO_RESPONSE;
}

SIC8250_Status SIC8250_Init(SIC8250_Handle *device,SPI_HandleTypeDef *hspi,GPIO_TypeDef *cs_port,uint16_t cs_pin,GPIO_TypeDef *reset_port,uint16_t reset_pin,uint32_t timeout_ms)
{
  if ((device == NULL) || (hspi == NULL) || (cs_port == NULL) || (reset_port == NULL))
  {
    return SIC8250_ERROR_ARGUMENT;
  }

  device->hspi = hspi;
  device->cs_port = cs_port;
  device->cs_pin = cs_pin;
  device->reset_port = reset_port;
  device->reset_pin = reset_pin;
  device->timeout_ms = (timeout_ms == 0U) ? SIC8250_DEFAULT_TIMEOUT_MS : timeout_ms;
  device->last_response_command = 0U;
  device->last_response_flag = 0U;

  HAL_GPIO_WritePin(device->cs_port, device->cs_pin, GPIO_PIN_SET);
  SIC8250_ResetAssert(device);
  return SIC8250_OK;
}

void SIC8250_ResetAssert(SIC8250_Handle *device)
{
  if ((device != NULL) && (device->reset_port != NULL))
  {
    HAL_GPIO_WritePin(device->reset_port, device->reset_pin, GPIO_PIN_RESET);
  }
}

void SIC8250_ResetRelease(SIC8250_Handle *device)
{
  if ((device != NULL) && (device->reset_port != NULL))
  {
    HAL_GPIO_WritePin(device->reset_port, device->reset_pin, GPIO_PIN_SET);
  }
}

void SIC8250_ResetPulse(SIC8250_Handle *device,uint32_t assert_time_ms,uint32_t boot_time_ms)
{
  SIC8250_ResetAssert(device);
  HAL_Delay(assert_time_ms);
  SIC8250_ResetRelease(device);
  HAL_Delay(boot_time_ms);
}

SIC8250_Status SIC8250_SFR_Read(SIC8250_Handle *device,uint8_t address,uint8_t *value)
{
  uint8_t tx[SIC8250_TRANSACTION_SIZE];
  uint8_t rx[SIC8250_TRANSACTION_SIZE] = {0U};
  SIC8250_Status status;

  if ((!SIC8250_IsReady(device)) || (value == NULL))
  {
    return SIC8250_ERROR_ARGUMENT;
  }

  memset(tx, SIC8250_PADDING, sizeof(tx));
  tx[0] = SIC8250_HEADER;
  tx[1] = SIC8250_SFR_READ_LENGTH;
  tx[2] = SIC8250_SFR_READ_COMMAND;
  tx[3] = address;
  tx[4] = SIC8250_Xor(&tx[1], 3U);

  status = SIC8250_Transfer(device, tx, rx);
  if (status != SIC8250_OK)
  {
    return status;
  }

  return SIC8250_ParseResponse(device, rx, value);
}

SIC8250_Status SIC8250_SFR_Write(SIC8250_Handle *device,uint8_t address,uint8_t value)
{
  uint8_t tx[SIC8250_TRANSACTION_SIZE];
  uint8_t rx[SIC8250_TRANSACTION_SIZE] = {0U};
  uint8_t echoed_value = 0U;
  SIC8250_Status status;

  if (!SIC8250_IsReady(device))
  {
    return SIC8250_ERROR_ARGUMENT;
  }

  memset(tx, SIC8250_PADDING, sizeof(tx));
  tx[0] = SIC8250_HEADER;
  tx[1] = SIC8250_SFR_WRITE_LENGTH;
  tx[2] = SIC8250_SFR_WRITE_COMMAND;
  tx[3] = address;
  tx[4] = value;
  tx[5] = SIC8250_Xor(&tx[1], 4U);

  status = SIC8250_Transfer(device, tx, rx);
  if (status != SIC8250_OK)
  {
    return status;
  }

  status = SIC8250_ParseResponse(device, rx, &echoed_value);
  return status;
}

SIC8250_Status SIC8250_TCFG_Read(SIC8250_Handle *device,uint8_t address,uint8_t *value)
{
  SIC8250_Status status;

  if (value == NULL)
  {
    return SIC8250_ERROR_ARGUMENT;
  }

  status = SIC8250_SFR_Write(device, SIC8250_TCFG_ADDRESS_SFR, address);
  if (status != SIC8250_OK)
  {
    return status;
  }

  return SIC8250_SFR_Read(device, SIC8250_TCFG_DATA_SFR, value);
}

SIC8250_Status SIC8250_TCFG_Write(SIC8250_Handle *device,uint8_t address,uint8_t value)
{
  SIC8250_Status status;

  status = SIC8250_SFR_Write(device, SIC8250_TCFG_ADDRESS_SFR, address);
  if (status != SIC8250_OK)
  {
    return status;
  }

  return SIC8250_SFR_Write(device, SIC8250_TCFG_DATA_SFR, value);
}
