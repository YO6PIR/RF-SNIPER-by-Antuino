# RF-SNIPER by Antuino

<p align="center">
 <img width="512" height="487" alt="image" src="https://github.com/user-attachments/assets/62785fdf-115f-4717-bc3f-01f04347d740" />

</p>

A compact RF bridge-type frequency scanner inspired by the original Antuino project by Ashhar Farhan (VU2ESE).

RF-SNIPER by Antuino is a portable radio-frequency scanner designed for the 0.1–150 MHz range. It combines a graphical display, touch-button interface, and practical measurement modes to provide a compact and useful RF analysis tool for experimentation, tuning, and field use.

The project is a practical evolution of the original Antuino concept, adapted to a more ergonomic and easier-to-use interface.

---

## Overview

This instrument is intended for:

- frequency scanning across a wide RF range
- signal observation and signal presence detection
- SWR and signal level monitoring
- RF bridge measurement and antenna tuning support
- graphical display of measured response

Unlike a full spectrum analyzer, this project focuses on a compact, fast, and practical RF scanning approach using a bridge-type measurement architecture and a visual display.

---

## Key Improvements Compared to the Original Antuino Concept

The device introduces several practical improvements:

- soft-touch button interface instead of a mechanical encoder
- better ergonomics and fewer mechanical control issues
- improved menu selection and function clarity
- battery voltage indication on the main screen
- simplified SWR display with a practical 0–9.99 limit
- continuous sweep mode for real-time display updates
- improved cursor behavior and center-frequency selection
- more stable and readable operation during scanning

---

## Hardware Architecture

The design is based on an embedded microcontroller platform and a custom RF front-end.

| Component | Description |
|---|---|
| Microcontroller | STM32 / embedded controller platform |
| Display | Graphical TFT display |
| RF Front-End | Bridge-type RF measurement circuit |
| Measurement Modes | PWR, SNA, and related sweep functions |
| User Interface | Soft-touch buttons |
| Power Monitoring | Battery voltage display |
| Storage | EEPROM-based settings / calibration |

The project keeps the concept simple and functional, while improving usability and interface behavior.

---

## Operating Modes

### PWR Mode

This mode provides real-time power-related measurements and a continuously updated graph. The signal changes are visible as they occur during scanning.

### SNA Mode

The analyzer view is optimized for signal observation, allowing the user to inspect the RF response in an intuitive and quick way.

### Cursor / Center Selection

When the cursor is moved across the graph, pressing the ENTER key stores the selected value as the center frequency for the next scan. This makes it easier to inspect and refine specific frequency regions.

---

## Graphical Display

The instrument uses a graphical display to show signal strength and RF response over frequency.

This allows the user to:

- scan the frequency range visually
- inspect signal peaks and valleys
- move the cursor through the graph
- center measurements around a selected frequency
- monitor real-time changes during scan

---

## Main Display and User Interface

<p align="center">
  <img width="900" alt="RF-SNIPER main menu and scan display" src="https://github.com/user-attachments/assets/3e9d23f6-8245-4b89-a9dc-0fbe025f48f4" />
</p>

The user interface uses a simple operational layout centered on the scan and signal graphs. After improvements, the instrument is easier to use and more stable during operation.

---

## Update: Ver. 2.1

A new software version was introduced with several improvements and refinements.

<p align="center">
  <img width="600" height="726" alt="image" src="https://github.com/user-attachments/assets/60d62c84-4532-4368-a8eb-9ab9dc760d61" />
</p>

This version improves the software behavior and user experience while preserving the original RF scanning concept.

---

## Project Status

RF-SNIPER by Antuino is a functional compact RF scanner and remains a practical example of a bridge-based measurement instrument inspired by the original Antuino project.

It is intended for:

- RF exploration
- field signal detection
- antenna and matching evaluation
- educational radio experimentation
- compact measurement tools for amateur radio work

---

## References

- Original concept: Antuino by Ashhar Farhan (VU2ESE)
- Project page: https://www.qsl.net/yo6pir/sniper.html

---

## Credits

This project is a practical adaptation and improvement of the original Antuino-based RF scanner concept, developed and refined by Ovidiu — YO6PIR.

---

## License

This project is distributed under the repository license included in the project.

See the LICENSE file for the full terms and conditions.
