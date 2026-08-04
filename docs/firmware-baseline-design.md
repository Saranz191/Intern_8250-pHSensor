# Four-channel pH firmware baseline

This document is the canonical reference for the training baseline. Local
flowcharts may be used as working notes, but they are intentionally not tracked
by Git.

## Constraints

- The MCU is the sole master of the SIC8250.
- Channels use internal identifiers 0 through 3 and a four-bit channel mask.
- Only one acquisition owner may access the SIC8250.
- The superloop and every state handler must be non-blocking.
- Every sample and every channel error carries a channel ID and job ID.
- Communication, protocol, application, device, and BSP responsibilities stay
  in separate layers.
- The baseline must build without performing application hardware I/O.

## Seven-state acquisition FSM

| State | Responsibility left for the trainee |
|---|---|
| `INIT` | Power sequencing, interface check, defaults, bounded init recovery |
| `IDLE` | Accept configuration/start commands and expose status |
| `START_CHANNEL` | Select one enabled channel, configure it, wait for settling, start ADC |
| `WAIT_RESULT` | Check channel-specific readiness, enforce the deadline, read and publish |
| `WAIT_CYCLE` | Wait until the next configured sampling period without blocking |
| `RECOVER` | Stop safely, retry with a bound, or publish a channel-tagged error |
| `FAULT` | Keep hardware safe, report status, and accept an explicit reinitialize command |

`SELECT_CHANNEL`, conversion, validation, publishing, and stop are bounded
actions rather than top-level states. `START_CHANNEL` has a small internal phase
for configure versus settling. A long-running or asynchronous action must be
promoted to a real state instead of hidden behind a flag.

The required transition table must cover at least:

- init success and each init failure;
- valid and invalid `SET_CONFIG` and `START` commands;
- ready, not-ready, timeout, and SIC8250 I/O error for the active channel;
- more selected channels, finite completion, and continuous next-cycle wait;
- bounded retry, per-channel failure continuation, and fatal device failure;
- `STOP` from every running state;
- `GET_STATUS` without changing acquisition state.

## Channel scheduling contract

At `START`, copy the configured mask to an active job snapshot and never change
that snapshot during the job. A typical sequential scheduler uses:

```text
active_channel_mask    = configured_channel_mask & 0x0F
remaining_channel_mask = active_channel_mask
```

Select one set bit, store its index in `active_channel`, and clear only that bit
from `remaining_channel_mask`. For internal channels 1 and 3, the mask is
`0x0A`. User interfaces that display channels 1 through 4 must translate at the
software boundary.

## Communication ownership

```text
software
  -> board RX interrupt/DMA
  -> Communication byte processing
  -> Protocol parser
  -> command dispatcher
  -> pH FSM (sole SIC8250 owner)
  -> response/event queue
  -> Protocol encoder
  -> board TX DMA
  -> software
```

Interrupt and DMA callbacks only move bytes or signal completion. They do not
parse commands, change FSM state, or access the SIC8250.

## Protocol frame version 1

All multi-byte integers are little-endian.

| Offset | Field | Size |
|---:|---|---:|
| 0 | SOF `AA 55` | 2 |
| 2 | Version `01` | 1 |
| 3 | Message type: command, response, event | 1 |
| 4 | Flags | 1 |
| 5 | Sequence ID | 2 |
| 7 | Command/event code | 1 |
| 8 | Wire status | 1 |
| 9 | Payload length | 2 |
| 11 | Payload | N, maximum 64 |
| 11 + N | CRC-16/CCITT-FALSE | 2 |

CRC parameters are polynomial `0x1021`, initial value `0xFFFF`, no reflection,
and final XOR `0x0000`. The CRC covers Version through the final payload byte;
it excludes SOF and the CRC field.

The minimum command set is `PING`, `GET_INFO`, `SET_CONFIG`, `START`, `STOP`,
`GET_STATUS`, and `GET_LATEST`. A response repeats the request sequence ID. A
successful `START` response returns a new job ID; measurement results are later
sent as asynchronous events carrying that job ID.

Sample events use fixed-point wire values rather than copying C structures:

```text
job_id, sample_index, timestamp_ms, channel_id, sample_status,
raw_adc, vph_uv, ph_milli
```

`ph_milli = 7123` represents pH 7.123. Encoders and decoders must serialize
each field explicitly; never `memcpy` a native C structure onto the wire.

## Student-owned implementation

The baseline intentionally leaves these functions incomplete:

- SIC8250 register order and conversion-ready interpretation;
- FSM transition actions, deadlines, retry policy, and scheduler;
- ADC-to-voltage and voltage-to-pH conversion;
- protocol parser, encoder, CRC, and resynchronization;
- command payload validation and response construction;
- UART/USB DMA, RX/TX buffering, and board pin/peripheral configuration.

HAL calls belong only in BSP source files. Project-owned upper layers must be
host-testable by replacing their callback dependencies.
