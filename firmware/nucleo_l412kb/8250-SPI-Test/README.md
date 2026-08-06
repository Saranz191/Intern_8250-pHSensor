# SIC8250 SPI test on STM32L412KB

โปรเจกต์ STM32CubeIDE ขั้นต่ำสำหรับสั่ง read/write register ของ SIC8250 โดยอ้างอิง
protocol จาก `SIC8250_Evaluation` branch `develop`

## Wiring

| NUCLEO-L412KB | MCU pin | SIC8250 |
|---|---|---|
| D0 | PA10 | RESET (active low) |
| D10 | PA11 | CS (active low) |
| D11 | PB5 | MOSI |
| D12 | PB4 | MISO |
| D13 | PB3 | SCK |
| GND | GND | GND |

ใช้ logic level 3.3 V และต่อ ground ร่วมกัน ตรวจสอบแรงดันเลี้ยงของบอร์ด SIC8250
ก่อนต่อไฟจริง D13/PB3 ใช้เส้นเดียวกับ LED สีเขียวบน NUCLEO-L412KB ตามวงจรของบอร์ด

ตัวอย่าง `SIC8250_Evaluation` ตั้งขา test mode และ bus type เป็น LOW แต่ขาทั้งสองไม่ได้อยู่ใน
pin list ของโปรเจกต์นี้ ดังนั้นบอร์ด SIC8250 ต้อง strap ขาเหล่านี้ไว้ที่ normal/SPI mode อยู่แล้ว
หากยังลอยอยู่ SPI read/write อาจไม่ตอบกลับ

## SPI configuration

- SPI1 master, full duplex, 8-bit, MSB first
- Mode 0: CPOL low, CPHA first edge
- 250 kbit/s (`32 MHz / 128`)
- Software NSS; PA11 controls CS manually
- One command transaction is always 15 bytes and pads unused bytes with `0xFF`

## API

```c
uint8_t value;
SIC8250_Status status;

status = SIC8250_SFR_Read(&hsic8250, sfr_address, &value);
status = SIC8250_SFR_Write(&hsic8250, sfr_address, value);
status = SIC8250_TCFG_Read(&hsic8250, tcfg_address, &value);
status = SIC8250_TCFG_Write(&hsic8250, tcfg_address, value);
```

TCFG access ทำผ่าน SFR `0xFE` (TCFG address) และ `0xFF` (TCFG data) ภายใน driver
อัตโนมัติ ฟังก์ชัน TCFG จึงเป็น indirect sequence และไม่ควรถูกเรียกสลับกันจากหลาย task
โดยไม่มี mutex

Response checksum และ frame bounds ถูกตรวจทุกครั้ง ส่วน response flag ถูกเก็บไว้ใน
`hsic8250.last_response_flag` โดยคงพฤติกรรมเดียวกับ evaluation code ซึ่งไม่ได้แปลง flag
เป็น error return

หลังเปิดเครื่อง `main.c` จะตั้ง CS high, reset SIC8250 แบบ active-low เป็นเวลา 10 ms
แล้วรอใน main loop โดยยังไม่อ่านหรือเขียน register ใด สามารถทดสอบแบบ one-shot ผ่าน
debugger โดยกำหนด `g_sic8250_address`, `g_sic8250_value` และตั้ง `g_sic8250_command` ดังนี้:

| Command | Operation |
|---:|---|
| 1 | SFR read |
| 2 | SFR write |
| 3 | TCFG read |
| 4 | TCFG write |

เมื่อทำรายการเสร็จ command จะกลับเป็น 0 ค่า return อยู่ใน `g_sic8250_status` และผล read
อยู่ใน `g_sic8250_value`

ใน STM32CubeIDE ให้ Import > Existing Projects into Workspace แล้วเลือกโฟลเดอร์
`STM32CubeIDE` หรือเปิด `8250-SPI_Test.ioc` เพื่อแก้ pin/clock ด้วย CubeMX เมื่อ generate ใหม่
ไฟล์ `sic8250.c/.h` จะยังอยู่ใน `Core` และโค้ดที่เพิ่มใน `main.c` อยู่ใน USER CODE section
