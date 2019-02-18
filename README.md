# pic32mz-eeprom
Working demo of reading and writing from [Pic32mz ef starter kit](https://www.microchip.com/Developmenttools/ProductDetails/Dm320007#additional-summary) (Pic32mz2048efh144 MCU) to [25LC256 EEPROM](https://www.microchip.com/wwwproducts/en/25LC256) via SPI. I found a lot of resources relating to the 25LC256; however, there seemed to be a lack of info using the Pic32mz. The two key files used are eeprom.h and eeprom.c

[This repo](https://github.com/hexagon5un/AVR-Programming/tree/master/Chapter16_SPI/spiEEPROMDemo), which uses an Atmel chip with the same EEPROM, contained simple, clean code and served as a strong influence for interfacing between my mcu and eeprom. I commented out the functions that I didn't test (because I didn't personally need it). This [blog](http://aidanmocke.com/blog/2018/09/01/spi-sd/) also really helped in understanding SPI with Pic32mz.

The demo is writing and reading to/from EEPROM, then outputting the data (from eeprom) via Uart. I sent it to an Arduino board. I'm using SPI1 and my configuration setup for this is in SPI_Init(). This eeprom uses SPI Mode 0, so this is configured by setting:
```cpp 
SPI1CONbits.CKP = 0; // using SPI mode 0 - CPOL = 0
SPI1CONbits.CKE = 1; // using SPI mode 0 - CPHA = 0
``` 
This eeprom operates at max 10 MHz, so the SPI clock needs to be lowered in the configuration by setting SPI1BRG. This SPI pins (SDI, SDO, SCK, & SS) are configured in this function as well. In eeprom.h, set to match your SS pin:
```cpp 
#define SLAVE_SELECT    LATDbits.LATD0 = 0; // select slave device (low)
#define SLAVE_DESELECT  LATDbits.LATD0 = 1; // deselect slave device (high)
```

Something to note. I had to wait for the SPI buffer to receive data, so needed to add this after transferring data to SPI buffer via SDO:
```cpp 
while(!SPI1STATbits.SPIRBF);
```

### Resources:

**[Pic32mz ef starter kit](https://www.microchip.com/Developmenttools/ProductDetails/Dm320007#additional-summary)**
- [Datasheet](https://github.com/square/certstrap)


**[25LC256 EEPROM](https://www.microchip.com/wwwproducts/en/25LC256)**
- [Digikey](https://www.digikey.com/product-detail/en/microchip-technology/25LC256-I-P/25LC256-I-P-ND/665615)
- [Datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/21822G.pdf)


### Pins Connections (J12 header on starter kit) to EEPROM:

- Pin 2 (5V) to VCC
    - To VCC on eeprom
- Pin 9 (GND) 
    - To VSS on eeprom
- Pin 11 (RD0) - Slave Select (SS)
    - To CS on eeprom
- Pin 19 (RF5) - MOSI/SDO
    - To SI on eeprom
- Pin 21 (RF4) - MISO/SDI
    - To SO on eeprom
- Pin 23 (RD1) - SCLK
    - To SCK on eeprom
- Pin 7 (RA14) - configuration commented
    - To HOLD on eeprom. I actually just connected this pin to 5V to keep it high.
- Pin 13 (RJ13) - configuration commented
    - To WP on eeprom. Also connected this pin to 5V to keep it high.
- Pin 8 (RG6) - TX
    - Connected to RX on an arduino.
- Pin 10 (RB14) - RX
    - I didn't use RX to return anything
