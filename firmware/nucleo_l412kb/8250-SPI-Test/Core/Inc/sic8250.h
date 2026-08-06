#ifndef SIC8250_H
#define SIC8250_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l4xx_hal.h"

typedef enum
{
  SIC8250_OK = 0,
  SIC8250_ERROR_ARGUMENT,
  SIC8250_ERROR_SPI,
  SIC8250_ERROR_NO_RESPONSE,
  SIC8250_ERROR_FRAME,
  SIC8250_ERROR_CHECKSUM
} SIC8250_Status;

typedef struct
{
  SPI_HandleTypeDef *hspi;
  GPIO_TypeDef *cs_port;
  uint16_t cs_pin;
  GPIO_TypeDef *reset_port;
  uint16_t reset_pin;
  uint32_t timeout_ms;
  uint8_t last_response_command;
  uint8_t last_response_flag;
} SIC8250_Handle;

SIC8250_Status SIC8250_Init(SIC8250_Handle *device,
                            SPI_HandleTypeDef *hspi,
                            GPIO_TypeDef *cs_port,
                            uint16_t cs_pin,
                            GPIO_TypeDef *reset_port,
                            uint16_t reset_pin,
                            uint32_t timeout_ms);

void SIC8250_ResetAssert(SIC8250_Handle *device);
void SIC8250_ResetRelease(SIC8250_Handle *device);
void SIC8250_ResetPulse(SIC8250_Handle *device,
                        uint32_t assert_time_ms,
                        uint32_t boot_time_ms);

SIC8250_Status SIC8250_SFR_Read(SIC8250_Handle *device,
                                uint8_t address,
                                uint8_t *value);
SIC8250_Status SIC8250_SFR_Write(SIC8250_Handle *device,
                                 uint8_t address,
                                 uint8_t value);
SIC8250_Status SIC8250_TCFG_Read(SIC8250_Handle *device,
                                 uint8_t address,
                                 uint8_t *value);
SIC8250_Status SIC8250_TCFG_Write(SIC8250_Handle *device,
                                  uint8_t address,
                                  uint8_t value);

#ifdef __cplusplus
}
#endif

#endif
