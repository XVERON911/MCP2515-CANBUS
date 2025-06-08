# CAN Bus Reader for Vehicles
**Script to read CAN bus data from your car using an `Arduino Uno R3` and `MCP2515 CAN Transceiver (with TJA1050 module)`**

---

## Where’s the Code?
You’ll find the script in the `src` folder under `main.cpp`.

You can also use the same script with `ESP32` or `ESP8266` instead of the Arduino Uno R3.

---

## Wiring Diagram

> ⚠️ **NOTE:** CS and INT pins are defined in code. Be careful when changing these, as the INT pin from the MCP2515 outputs **5V**.

| MCP2515     | Arduino R3 | OBD2 Connector         |
|-------------|------------|------------------------|
| VCC         | 5V         |                        |
| GND         | GND        |                        |
| CS          | 10         |                        |
| SO (MISO)   | 12         |                        |
| SI (MOSI)   | 11         |                        |
| SCK         | 13         |                        |
| INT         | 2          |                        |
| CAN HIGH    |            | PIN 6 (CAN H)          |
| CAN LOW     |            | PIN 14 (CAN L)         |

---

## Configuration Tips

- Ensure your Serial Monitor baud rate is set to **250000**.
- In the script, check **line 98** and modify it to match your MCP2515 crystal speed (8 MHz or 16 MHz).
- On the same line, adjust the **CAN bus speed**:
  - **1000 Kbps**: Modern ECUs
  - **500 Kbps**: Standard ECUs
  - **125 Kbps**: BCM (Body Control Module)

---

## Available Serial Commands

| Command    | Description                                                                 |
|------------|-----------------------------------------------------------------------------|
| `NORMAL`   | Sets MCP2515 to Normal mode (default is LOOPBACK on boot)                   |
| `LOOPBACK` | Sets MCP2515 to Loopback mode (test mode for internal frame echo)           |
| `SEND`     | Sends a diagnostic request to the ECU                                       |
| `SEND2`    | Alternate diagnostic request to the ECU                                     |
| `SEND3`    | Sends an RPM request to the ECU; expects HEX response with engine RPM       |

---

## MCP2515 Mode Descriptions

- **NORMAL MODE**: Actively listens for incoming CAN frames, including frames it sends.
- **LOOPBACK MODE**: Test mode — MCP2515 does not receive external frames but will "hear" its own messages.

---

## ⚠️ Important Notes & Troubleshooting

- This script allows **read/write via Serial** using defined commands.
- Some vehicles have a **gateway in the OBD2 port** that blocks direct CAN communication. In that case:
  - You may need to find another CAN bus port inside the vehicle.
- Some vehicles require a **diagnostic request** to start transmitting CAN frames.
- If the OBD2 port appears idle, the vehicle may use a different protocol (like ISO/KWP2000) instead of CAN.
  - In this case, this setup won’t work for CAN, but you can still potentially communicate over **K-Line** using just an Arduino/ESP board and some resistors.

---

