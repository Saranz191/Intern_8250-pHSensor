# Intern 8250 pH Sensor

Training firmware skeleton for NUCLEO-L432KC and SIC8250.

## Current state

This `main` branch is intentionally incomplete. It builds in a safe idle state,
retains the CubeMX board-default LD3/VCP GPIO initialization, and never uses
those board-default pins. SIC8250/application SPI, chip select, reset, power,
application UART, DMA, and other application GPIO remain unconfigured and
unused. The starter C defines the intended seven-state FSM, four-channel data
model, binary protocol frame, communication boundary, and operation APIs while
leaving their behavior as safe compile-time stubs.

## Learning objectives

- complete a non-blocking FSM from the canonical baseline contract;
- separate Application, Domain, device-driver, and BSP responsibilities;
- extend a reusable single-channel operation to four channels;
- preserve channel identity through samples and errors;
- implement framed host communication without mixing protocol and hardware I/O.

## Project structure

The generated project root is
`firmware/nucleo_l432kc/NUCLEO_L432KC_PH`. Its generated headers and sources
live in `Inc` and `Src`; the `STM32CubeIDE` child contains the Eclipse project
metadata. `Application` owns the top-level lifecycle, FSM, and four-channel
contexts. `Domain` owns shared transport-independent types and configuration.
`Drivers/SIC8250` defines hardware-independent SIC8250 operation boundaries.
`Protocol` owns the transport-independent binary frame. `Communication` owns
RX/TX processing and frame queues. `BSP` defines board-port boundaries and
currently performs no hardware I/O. `docs` contains the assignment and the
canonical baseline design. Local flowcharts are intentionally ignored by Git.

## Open in STM32CubeIDE

Import
`firmware/nucleo_l432kc/NUCLEO_L432KC_PH/STM32CubeIDE` as an existing project,
open the linked `NUCLEO_L432KC_PH.ioc`, configure the real board connections,
regenerate only after reviewing CubeMX changes, and build the Debug
configuration.

## Assignment

Read `docs/assignment.md` and `docs/firmware-baseline-design.md`.

## Git workflow

Fetch `main`, create `feature/<short-description>`, commit only source and
documentation, push the feature branch, and open a pull request. Do not commit
directly to `main`.
