# NUCLEO-L432KC Four-Channel pH Training Skeleton Design

> Historical setup record. The current canonical training contract is
> `docs/firmware-baseline-design.md`.

## Purpose

Create the initial `main` branch for an intern training project targeting the
NUCLEO-L432KC (STM32L432KC). The project teaches finite-state-machine design,
layered embedded software, hardware abstraction, and four-channel pH
acquisition with the SIC8250.

The repository must provide a buildable, safe project skeleton without
implementing the measurement algorithm, SIC8250 register sequence, channel
scheduling policy, state transition table, ADC-to-voltage conversion, or
voltage-to-pH conversion.

## Inputs

- `docs/firmware-baseline-design.md` is the current functional FSM and protocol
  contract. Working draw.io files remain local and ignored by Git.
- `firmware/macallan_low_leakage_dropper_sum.py` and
  `firmware/generated_registers.py` are instructor-side reference
  implementations used only to identify useful abstraction boundaries.
- The Python reference files and their working register values must not be
  copied into the training repository.

## Goals

- Provide a STM32CubeIDE/CubeMX project for NUCLEO-L432KC.
- Preserve the CubeMX board-default LD3/VCP GPIO initialization, while keeping
  SIC8250/application SPI, chip select, reset, power, other application GPIO,
  and board timing unconfigured so the intern must complete the `.ioc` file
  from the real wiring.
- Separate application lifecycle/FSM, shared domain types/configuration,
  SIC8250 device access, and board-specific hardware access.
- Fix the required channel count at four while leaving channel scheduling and
  channel-selection implementation to the intern.
- Provide compile-safe stubs that never access hardware until implemented.
- Document a feature-branch and pull-request workflow while creating only the
  `main` branch.

## Non-goals

- A working pH measurement.
- Complete SIC8250 initialization or register configuration.
- A solved FSM, transition table, retry policy, or scheduling algorithm.
- Real board pin assignments.
- Four duplicated copies of single-channel code.
- Temperature compensation, calibration storage, user interface, telemetry,
  or production bootloader support.

## Repository Layout

```text
Intern_8250-pHSensor/
|-- docs/
|   |-- assignment.md
|   |-- firmware-baseline-design.md
|   `-- superpowers/
|       |-- plans/
|       `-- specs/
|-- firmware/
|   `-- nucleo_l432kc/
|       |-- NUCLEO_L432KC_PH.ioc
|       |-- Core/
|       |-- Application/
|       |-- Domain/
|       |-- Drivers/SIC8250/
|       `-- BSP/
|-- .gitignore
`-- README.md
```

Cube-generated startup, CMSIS, HAL, linker, and project metadata files may add
the standard directories required by STM32CubeIDE. Project-owned code remains
inside the four named project layers.

## Architecture

### Cube entry point

`Core/Src/main.c` performs Cube-generated platform initialization and then
calls only:

```c
ph_status_t ph_app_init(ph_app_t *app, const ph_dependencies_t *dependencies);
ph_status_t ph_app_process(ph_app_t *app);
```

The main loop must remain non-blocking. In the initial skeleton,
`ph_app_process()` performs no hardware access and returns
`PH_STATUS_NOT_IMPLEMENTED`.

### Application layer

The application layer owns the top-level lifecycle, FSM, and the four channel
contexts:

```c
#define PH_CHANNEL_COUNT (4U)

typedef struct {
    ph_fsm_t fsm;
    ph_channel_context_t channels[PH_CHANNEL_COUNT];
} ph_app_t;
```

It decides when a measurement activity may start and where completed samples
are published. The skeleton does not decide which channel runs next, whether
contexts are processed sequentially or cooperatively, or how retries affect
channel ordering.

The initial state model contains only enough information to represent an
unimplemented safe idle state. The intern derives all operational states,
events, guards, actions, and transitions from the flow diagram.

The public FSM contract is:

```c
ph_status_t ph_fsm_init(ph_fsm_t *fsm, const ph_dependencies_t *dependencies);
ph_status_t ph_fsm_process(ph_fsm_t *fsm);
ph_status_t ph_fsm_stop(ph_fsm_t *fsm);
ph_status_t ph_fsm_get_status(const ph_fsm_t *fsm);
```

These functions are compile-safe stubs and do not encode the completed flow.

### Domain layer

The domain layer owns shared transport-independent types and configuration:

- fixed channel count
- channel identifier
- raw ADC value
- voltage
- pH value
- timestamp
- typed status
- sample record

### SIC8250 driver layer

The device layer provides hardware-independent operation boundaries:

```c
sic8250_status_t sic8250_init(sic8250_device_t *device);
sic8250_status_t sic8250_configure_measurement(
    sic8250_device_t *device,
    uint8_t channel_id);
sic8250_status_t sic8250_start_measurement(sic8250_device_t *device);
sic8250_status_t sic8250_check_ready(
    sic8250_device_t *device,
    bool *ready);
sic8250_status_t sic8250_read_adc(
    sic8250_device_t *device,
    uint32_t *raw_adc);
sic8250_status_t sic8250_stop(sic8250_device_t *device);
```

The skeleton provides no register addresses, masks, values, command order, or
channel selection expression. Each operation returns a typed
not-implemented status.

### BSP port layer

The BSP owns every STM32 HAL dependency. Its public port contains only the
minimum hardware services required by higher layers:

- register-oriented SPI read and write
- monotonic millisecond time
- optional power control
- optional reset control

No SIC8250/application SPI peripheral, chip-select pin, power pin, reset pin,
or other application GPIO is selected in the initial project. CubeMX
board-default LD3/VCP GPIO initialization remains, but the starter application
never uses those pins. The board implementation returns
`PH_STATUS_NOT_IMPLEMENTED` and performs no I/O.

## Data Flow

```text
main.c
  -> Application lifecycle and four-channel ownership
    -> One non-blocking FSM step
      -> SIC8250 operation boundary
        -> BSP SPI/GPIO/time port

BSP/device status
  -> FSM status
    -> Application status and channel-tagged sample
```

Every completed sample must carry its channel identifier so data cannot be
mistaken for another channel. The design does not prescribe the channel
scheduling algorithm.

## Error Handling and Safe Behavior

- Each layer returns a typed status rather than printing or silently ignoring
  an error.
- FSM and application code must not call STM32 HAL directly.
- Timing decisions use the injected monotonic clock and must not use blocking
  delay loops.
- The intern defines timeout, retry, and recovery transitions from the supplied
  flow and assignment constraints.
- The initial skeleton remains in safe idle and never powers, resets,
  configures, starts, reads, or stops the SIC8250.
- A channel error must retain the correct channel identity.

## Build Verification

No automated tests, host test harness, fake BSP, CMake host build, or CI
workflow are included. Verification of the initial skeleton is limited to
opening the project in STM32CubeIDE and confirming that the safe-idle firmware
with no application hardware assignment builds successfully.

## Assignment Guidance

`docs/assignment.md` directs the intern to:

1. read the flow diagram;
2. define states, events, guards, actions, and a transition table;
3. configure SPI/GPIO in CubeMX from the actual wiring;
4. implement the BSP and SIC8250 driver;
5. make the one-channel operation reusable for channels 0 through 3;
6. design the four-channel scheduling policy;
7. implement ready, timeout, retry, and safe-stop behavior;
8. implement conversion and channel-tagged reporting.

The starter C and narrative documentation explain responsibilities and
acceptance criteria without providing completed code or a transition solution.
The unchanged draw.io remains the human-approved technical flow reference.

## Git Workflow

- Create and publish only the `main` branch during setup.
- Set `main` as the GitHub default branch.
- The intern creates a new `feature/<short-description>` branch from the latest
  `main`.
- Implementation is submitted through a pull request.
- Generated build output, local IDE state, credentials, and machine-specific
  files are excluded by `.gitignore`.

## Acceptance Criteria for the Initial Main Branch

- The repository contains only the `main` branch created by this setup.
- STM32CubeIDE recognizes the STM32L432KC project and its `.ioc` file.
- The safe-idle skeleton builds successfully in STM32CubeIDE.
- No SIC8250/application SPI/GPIO assignment or SIC8250 register sequence is
  present; only the retained CubeMX board-default LD3/VCP GPIO initialization
  is excepted.
- Project-owned code is divided into Application, Domain, SIC8250 driver, and
  BSP layers.
- The stub supports the four-channel data model without implementing channel
  scheduling.
- The supplied flow diagram and assignment documentation are present.
- The working Python reference implementations are absent.
- The README explains setup, architecture, safety state, learning objectives,
  and the feature-branch workflow.
