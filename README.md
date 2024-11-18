# PicoCompass
The goal of the project was to create a complete electronic device functioning as a digital compass, operating based on Earth's magnetic field measurements. A compact board with a display, battery, and additional sensors was to be enclosed in a handy case.

<div align="center">
  <img src="Compass_photo_1.png" alt="screenshot1">
</div>

## Assumptions<br />
- Utilization of a board with a small circular display and an RP2040 microcontroller.<br />
- Design of a PCB with additional sensors, an encoder, and a debug connector.<br />
- Use of the C programming language.<br />
- Indication of the north direction based on Earth's magnetic field measurements.<br />
- Measurement of temperature and humidity.<br />
- Displaying the compass and measurement data on the screen.<br />
- Design and 3D printing of a durable enclosure.<br />

## Hardware<br />
- Development Board: Waveshare RP2040-LCD-1.28 featuring the RP2040 microcontroller and a circular LCD display.<br />
- Magnetometer: QST QMC5883L - a cost-effective chip that measures the magnetic field in three axes (x, y, z) and uses the I2C interface.<br />
- Temperature and Humidity Sensor: Sensirion SHT20 (equivalent to HTU21D).<br />
- Encoder: SMD Mitsumi Electric (push-button type).<br />
- LDO: Shenzhen Fuman Elec 662K (5V to 3.3V voltage regulator).<br />
- Passive SMD Components: Ceramic capacitors and thick-film resistors.<br />
- Custom PCB: Designed specifically for the project.<br />
- 3D Printed Enclosure: Made from durable PETG material.<br />

## Software<br />
- The project utilized the recommended Pico C/C++ Toolchain, which includes:<br />
- Visual Studio Code for Pico – as the main development environment.<br />
- GNU Arm Embedded Toolchain – for compiling the code.<br />
- Pico SDK – a software development kit provided by the manufacturer for programming the RP2040 microcontroller.<br />
- CMake + Ninja – for build configuration and build automation.<br />
- OpenOCD Server – for debugging and hardware interface.<br />

## Authors
- PCB Design and Component Assembly: Antoni Kijania<br />
- User Interface Development: Piotr Podkówka & Jan Trojnar<br />
- Development and Implementation of Sensor Libraries: Miłosz Janik & Piotr Podkówka & Jan Trojnar<br />
- Encoder Handling, Final Testing, and Adjustments: Miłosz Janik & Piotr Podkówka<br />
- Enclosure Design and 3D Printing: Miłosz Janik<br />

