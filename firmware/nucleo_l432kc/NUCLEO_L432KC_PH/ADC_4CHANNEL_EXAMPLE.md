# SIC8250 ADC example: four pH channels

This branch is a hardware example for NUCLEO-L432KC. It keeps `main` as the
training baseline and demonstrates the missing low-level path separately from
the intern's feature branch.

## Wiring

| NUCLEO-L432KC | MCU pin | SIC8250 |
|---|---|---|
| D0 | PA10 | RESET, active low |
| D10 | PA11 | CS, active low |
| D11 | PB5 | MOSI |
| D12 | PB4 | MISO |
| D13 | PB3 | SCK |
| GND | GND | GND |

The example assumes the SIC8250 board is externally powered at the correct
voltage and is strapped for normal SPI mode. PB3 is shared with the green LED
on NUCLEO-L432KC, so the LED is unavailable while SPI1 is active.

## What the example does

1. Configure SPI1 as Mode 0, 8-bit, MSB-first at 250 kbit/s.
2. Reset SIC8250 and load TCFG defaults through SFR `FE/FF`.
3. Apply the OCP/pH profile taken from the working Python reference.
4. Select channels 0, 1, 2 and 3 using `C4 = 0x18 | (channel << 5)`.
5. Start each conversion with `BF = 0x80`.
6. Poll `B5[7:4]` until it differs from the value captured before start.
7. Read `E1`, `E2`, `E3`; publish `(E3 << 8) | E2` as the unsigned ADC code.

Inspect these globals in the debugger:

- `g_sic8250_adc_code[4]`
- `g_sic8250_adc_fraction[4]`
- `g_sic8250_channel_status[4]`

An individual channel times out after 50 ms. A timeout or SPI/frame error is
stored in the corresponding status entry and the loop continues with the next
channel.

This example returns ADC codes only. Converting them to voltage and pH still
requires the board's verified ADC transfer function, per-channel calibration,
pH zero/slope calibration and, if required, temperature compensation.
