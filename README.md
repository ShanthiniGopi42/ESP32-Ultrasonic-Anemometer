# ESP32-Based Ultrasonic Anemometer

A 2D ultrasonic anemometer designed to measure wind speed and wind direction using ultrasonic transit-time measurements. The project involves electronic circuit design, signal conditioning, microcontroller integration, PCB layout, and Gerber file generation using KiCad.

## Overview

An ultrasonic anemometer measures wind conditions using the propagation time of ultrasonic signals between transducers. Wind affects the travel time of sound along different directions, allowing wind speed and direction to be estimated from the measured transit times.

This project uses an ESP32-S3 microcontroller and four 40 kHz ultrasonic transducers arranged in North, East, South, and West directions. The electronic circuit is designed to support ultrasonic transmission, reception, signal processing, and microcontroller interfacing.

The schematic and PCB layout were developed in KiCad, and Gerber files were generated to prepare the board for fabrication.

## Objectives

- Design an electronic system for measuring wind speed and wind direction using ultrasonic transit-time principles.
- Integrate an ESP32-S3 microcontroller with ultrasonic transmitter and receiver circuits.
- Design the required signal-conditioning and interface circuits.
- Develop the complete circuit schematic using KiCad.
- Create a PCB layout based on the schematic and component footprints.
- Generate Gerber and drill files for PCB manufacturing.
- Prepare the design files and documentation for future hardware fabrication and testing.

## Hardware Components

**Microcontroller:** ESP32-S3 DevKitC-1U for system control and signal processing.

**Ultrasonic Transducers:** Four 40 kHz ultrasonic transducers arranged along the North, East, South, and West directions.

**Signal Conditioning:** Operational amplifier and comparator circuits designed for processing received ultrasonic signals.

**Analog Multiplexing:** Analog multiplexing circuitry for selecting signal paths.

**Power Supply:** Voltage regulation and power-management circuits for the electronic system.

Refer to the project schematic and report for the complete component list and circuit connections.

## Software and Design Tools

**KiCad:** Used for schematic capture, component placement, PCB routing, and manufacturing-file generation.

**Embedded Development:** ESP32-S3-based development for controlling the measurement system. Add the firmware language and development environment here if they are part of your completed work.

## Working Principle

1. Ultrasonic transducers transmit and receive signals across the measurement paths.
2. The transit time of the ultrasonic signals is used to determine the effect of wind on sound propagation.
3. Measurements along different directions can be compared to estimate wind speed and wind direction.
4. The ESP32-S3 is intended to coordinate the measurement process and process the relevant signals.

The accuracy of the measurements depends on the transducer arrangement, timing measurements, signal conditioning, and calibration.

## Circuit and PCB Design

The electronic circuit was developed in KiCad, with separate sections for power regulation, ultrasonic signal transmission and reception, signal conditioning, analog multiplexing, and microcontroller interfacing.

The PCB layout was created from the schematic, with component footprints and routed tracks arranged on the board. Gerber and drill files were then generated for use in PCB fabrication.

## Repository Contents

**`Report/`** – Project report and technical documentation.

**`Kicad Design/`** – KiCad schematic, PCB layout, and associated project files.

**`Gerber Files/`** – PCB manufacturing outputs, including copper layers, solder mask, silkscreen, board outline, and drill files where available.

**`Images/`** – Circuit connection diagrams, PCB layout screenshots, and other project visuals.

## Project Status

Schematic design, PCB layout, and Gerber file generation have been completed. PCB fabrication, hardware integration, and experimental validation remain pending if not yet performed.

## Future Work

- Fabricate the PCB using the generated manufacturing files.
- Assemble and test the circuit with the ESP32-S3 and ultrasonic transducers.
- Develop and validate the firmware for transit-time measurement.
- Calibrate the system and compare its measurements with a reference instrument.
- Evaluate measurement accuracy under different wind conditions.

## Author

**Shanthini G**

Electronics and Communication Engineering
