Camera_test
===========


## Hardware

- STM32F4 Discovery board
- OV2640 camera module (2 megapixels)
- Serial-USB FTDI module

### Connecting camera module to discovery board

Camera board                        | DCMI signal | STM32 pins
----------------------------------- | ----------- | ----------
VCC : Power Supply                  | VCC         | 3V
GND : Power Ground                  | GND         | GND
SCL : I2C Clock                     | I2C_2_SCL   | PB10
SDA : I2C Data                      | I2C_2_SDA   | PB11
VSYNC : Vertical Sync               | VSYNC       | PB7
HREF : Horizon Reference            | HSYNC       | PA4
PCLK : Pixel Clock 36MHz=>15fps     | PIXCLK      | PA6
XCLK : Master Clock 6-24-...MHz     | MCO1        | PA8
D0~D7 : Data Bus                    | D:0-7       |
                                    | D0          | PC6
                                    | D1          | PC7
                                    | D2          | PC8
                                    | D3          | PC9
                                    | D4          | PC11
                                    | D5          | PB6
                                    | D6          | PB8
                                    | D7          | PB9
Y0~Y1 : LSB in YUV 10 bit mode      | nc          |
Flash : Optional Flash signal       | nc          |
PWDN : Power down input             | GPIO        | PA9
RST : Complete hardware reset (LOW) | GPIO        | PA10


### Connecting FTDI module to discovery board

FTDI | USART | STM32 pins
-----|-------|-----------
GND  | GND   | GND
TX   | RX    | PA3
RX   | TX    | PA2