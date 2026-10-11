# ESP32 LoRa RSSI Graph Monitor

A wireless signal monitoring project built using two ESP32 DevKit V1 boards, RA-02 SX1278 LoRa modules, and a 1.3-inch OLED display. The receiver displays a real-time scrolling graph of Received Signal Strength Indicator (RSSI) values to help visualize wireless link quality.

## Features

- **LoRa wireless communication** using the SX1278 transceiver.
- **ESP32-based transmitter and receiver** using the LoRa library.
- **Live RSSI monitoring** in dBm.
- **Scrolling signal-strength graph** showing the latest 48 received packets.
- **OLED visualization** using a 1.3-inch, 128×64 SH1106 display.
- **Packet counter** to track successfully received messages.
- **Serial Monitor output** for debugging and signal analysis.

## Hardware Requirements

| Component | Quantity |
|---|---:|
| ESP32 DevKit V1 | 2 |
| RA-02 SX1278 LoRa module | 2 |
| 1.3-inch, 128×64 I2C OLED (SH1106) | 1 |
| Breadboard | 1 |
| Jumper wires | As required |
| USB cables | 2 |
| Compatible LoRa antennas | 2 |

> **Note:** The transmitter needs one ESP32 and one LoRa module. The receiver needs another ESP32 and LoRa module, plus the OLED display.

## System Architecture

```text
        TRANSMITTER
    ┌─────────────────┐
    │   ESP32 DevKit  │
    └────────┬────────┘
             │ SPI
    ┌────────▼────────┐
    │  RA-02 SX1278   │
    │   LoRa Module   │
    └────────┬────────┘
             │
             │ LoRa RF link
             │
    ┌────────▼────────┐
    │  RA-02 SX1278   │
    │   LoRa Module   │
    └────────┬────────┘
             │ SPI
    ┌────────▼────────┐
    │   ESP32 DevKit  │
    └────────┬────────┘
             │ I2C
    ┌────────▼────────┐
    │ 1.3-inch OLED   │
    │ RSSI Graph      │
    └─────────────────┘
           RECEIVER
```

## Wiring Connections

### 1. LoRa Module to ESP32

The following connections apply to both transmitter and receiver.

| RA-02 Pin | ESP32 GPIO |
|---|---|
| VCC | 3V3 |
| GND | GND |
| NSS / CS | GPIO 5 |
| SCK | GPIO 18 |
| MISO | GPIO 19 |
| MOSI | GPIO 23 |
| RST | GPIO 14 |
| DIO0 | GPIO 26 |

### 2. OLED to Receiver ESP32

| OLED Pin | ESP32 Pin |
|---|---|
| VCC | 3V3 |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

The OLED and LoRa receiver share the same 3.3 V and GND rails.

**Power precautions:**
- Supply the RA-02 from 3.3 V, never 5 V.
- Keep all grounds connected.
- Verify that the 3.3 V supply remains stable when the OLED and LoRa module are connected.
- Connect compatible antennas before transmitting.
- Use the correct operating frequency for the module and comply with local radio regulations.

## Software Requirements

- Arduino IDE
- ESP32 board support package
- LoRa library by Sandeep Mistry
- Adafruit GFX Library
- Adafruit SH110X Library

Install the libraries using **Arduino IDE → Sketch → Include Library → Manage Libraries**.

## Configuration

The transmitter and receiver must use matching LoRa settings.

| Parameter | Value |
|---|---|
| Frequency | 433 MHz |
| Spreading Factor | 7 |
| Signal Bandwidth | 125 kHz |
| Coding Rate | 4/5 |
| Sync Word | `0x12` |
| CRC | Enabled |

These are the settings used by the current example firmware. Confirm that 433 MHz is appropriate for your hardware and permitted for your intended use.

## How the RSSI Graph Works

RSSI stands for **Received Signal Strength Indicator**. It represents the received radio signal power as measured by the LoRa transceiver.

The receiver:

1. Listens for incoming LoRa packets.
2. Reads each successfully received message.
3. Retrieves the packet's RSSI value.
4. Stores the value in a rolling buffer of 48 samples.
5. Draws the historical measurements on the OLED.
6. Updates the packet counter and Serial Monitor output.

### Graph Parameters

| Parameter | Configuration |
|---|---|
| Measurement | RSSI |
| Unit | dBm |
| Display resolution | 128 × 64 pixels |
| History length | 48 packet samples |
| Graph scale | −120 to −30 dBm |
| Update trigger | Successful packet reception |

The graph uses a fixed scale so readings can be compared over time.

### Understanding RSSI

| Example RSSI | General interpretation |
|---|---|
| −40 dBm | Strong received signal |
| −60 dBm | Relatively strong signal |
| −80 dBm | Moderate received signal |
| −100 dBm | Weak received signal |
| −120 dBm | Near the lower end of the configured graph scale |

These are illustrative values, not universal quality thresholds. Actual performance depends on the radio configuration, antenna, interference, environment, and receiver sensitivity.

## Setup Instructions

1. Connect the LoRa transmitter according to the wiring table.
2. Upload the transmitter firmware to the first ESP32.
3. Connect the LoRa module and OLED to the receiver ESP32.
4. Install the required Arduino libraries.
5. Upload the RSSI graph receiver firmware to the second ESP32.
6. Open the receiver Serial Monitor at **115200 baud**.
7. Power on the transmitter and receiver with compatible antennas attached.
8. Observe the RSSI graph as packets arrive.

The receiver should display the current RSSI, received packet count, and scrolling graph.

## Testing and Experiments

Possible experiments include:

- Measure RSSI at different distances.
- Compare signal strength indoors and outdoors.
- Observe RSSI changes when obstacles are introduced.
- Compare antenna orientations.
- Record signal strength while moving the receiver.
- Investigate packet loss under different environmental conditions.

For meaningful comparisons, keep the radio configuration and antenna arrangement consistent.

## Troubleshooting

| Problem | Checks |
|---|---|
| LoRa initialization fails | Verify SPI wiring, CS, reset, power, and module connectivity. |
| Receiver gets no packets | Check frequency, sync word, spreading factor, bandwidth, coding rate, CRC, and antenna connections. |
| OLED remains blank | Verify I2C wiring, address `0x3C`, display controller, and library configuration. |
| Graph does not change | Ensure the transmitter is sending packets and the receiver is successfully receiving them. |
| ESP32 resets unexpectedly | Check the 3.3 V supply, ground connections, and power stability. |
| RSSI readings vary | Consider distance, antenna orientation, obstructions, interference, and multipath fading. |

## Future Improvements

- Add packet-loss measurement.
- Implement bidirectional LoRa communication.
- Log RSSI readings over time.
- Add OLED controls to change graph scales.
- Send sensor measurements through LoRa.
- Build a desktop dashboard for signal monitoring.
- Evaluate range and reliability under different conditions.

## Learning Outcomes

This project provides practical experience with:

- ESP32 GPIO and SPI interfaces.
- I2C display interfacing.
- LoRa packet transmission and reception.
- Radio signal-strength measurement.
- Embedded graphics and circular buffers.
- Wireless communication testing and debugging.

## License

This project may be distributed under the MIT License. If you choose MIT, include a `LICENSE` file with the appropriate copyright notice.

---

**Project:** ESP32 LoRa RSSI Graph Monitor  
**Platform:** ESP32 + SX1278 + SH1106 OLED  
**Development environment:** Arduino IDE
## Image Gallery

<p align="center">
  <img src="images/lora-module.jpg" alt="RA-02 SX1278 LoRa Module" width="30%">
  <img src="images/esp32.jpg" alt="ESP32 DevKit V1" width="30%">
  <img src="images/oled-screen.jpg" alt="OLED displaying live RSSI data graph" width="30%">
</p>

<p align="center">
  <b>LoRa Module</b> · <b>ESP32</b> · <b>OLED RSSI Graph</b>
</p>
