# Intern 8250 pH Sensor

Training firmware skeleton for NUCLEO-L432KC and SIC8250.

## Current state

This `main` branch is intentionally incomplete. It builds in a safe idle state,
retains the CubeMX board-default LD3/VCP GPIO initialization, and never uses
those board-default pins. SIC8250/application SPI, chip select, reset, power,
and other application GPIO remain unconfigured and unused. The starter C and
narrative guidance contain no measurement solution; the unchanged draw.io is
the approved technical flow reference.

## Learning objectives

- derive a non-blocking FSM from the supplied flow diagram;
- separate Application, Domain, device-driver, and BSP responsibilities;
- extend a reusable single-channel operation to four channels;
- preserve channel identity through samples and errors.

## Project structure

The generated project root is
`firmware/nucleo_l432kc/NUCLEO_L432KC_PH`. Its generated headers and sources
live in `Inc` and `Src`; the `STM32CubeIDE` child contains the Eclipse project
metadata. `Application` owns the top-level lifecycle, FSM, and four-channel
contexts. `Domain` owns shared transport-independent types and configuration.
`Drivers/SIC8250` defines hardware-independent SIC8250 operation boundaries.
`BSP` defines the board-port boundary and currently performs no hardware I/O.
`docs` contains the assignment and the approved reference flow diagram.

## Open in STM32CubeIDE

Import
`firmware/nucleo_l432kc/NUCLEO_L432KC_PH/STM32CubeIDE` as an existing project,
open the linked `NUCLEO_L432KC_PH.ioc`, configure the real board connections,
regenerate only after reviewing CubeMX changes, and build the Debug
configuration.

## Assignment

Read `docs/assignment.md` and `docs/SIC8250_pH_OCP_Flow.drawio`.

## Git workflow

Fetch `main`, create `feature/<short-description>`, commit only source and
documentation, push the feature branch, and open a pull request. Do not commit
directly to `main`.
