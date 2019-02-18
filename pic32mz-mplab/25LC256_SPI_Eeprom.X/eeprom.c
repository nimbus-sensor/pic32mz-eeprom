#include "eeprom.h"

/* 
 * Init SPI to run EEPROM with phase, polarity = 0,0 
 */
void SPI_Init(void) {
    
    // eeprom spi master
    SPI1CONbits.ON = 0; // turn off SPI1 before configuring
    
    SPI1CONbits.MODE32 = 0; // use 8 bit mode
    SPI1CONbits.MODE16 = 0; // use 8 bit mode
    
    SPI1CONbits.MSTEN = 1; // make master
    SPI1CONbits.CKP = 0; // using SPI mode 0 - CPOL = 0 - https://www.microchip.com/forums/m273491.aspx
    SPI1CONbits.CKE = 1; // using SPI mode 0 - CPHA = 0
    SPI1CONbits.SMP = 1; // input data is sampled at the end of the clock signal
    
    SPI1BRG = 49; // 2 MHz - divided from clock frequency of 200MHz
    SPI1CONbits.ON = 1; // turn on SPI1
    
    
    // Set up PPS for SPI1 on RF4 (MISO - SDI) and RF5 (MOSI - SDO)
	
    /*// HOLD (RDA14) config -> kept high by attaching to 5V pin
    TRISAbits.TRISA14 = 0; // set HOLD as output
    LATAbits.LATA14 = 1; // keep HOLD high during operation
    
    // WP (RJ13) config -> kept high by attaching to 5V pin
    TRISJbits.TRISJ13 = 0; // set WP as output
    LATJbits.LATJ13 = 1; // keep WP high during operation*/
    
    
    // SDI (RF4) config
    TRISFbits.TRISF4 = 1; // Set RF4 as input because it's SDI1
    SDI1R = 0b0010; // RF4 = SDI1
    
    // SD0 (RF5) config
    TRISFbits.TRISF5 = 0; // set RF5 as output because it's SD01
    RPF5R = 0b0101; // RF5 = SDO1
    
    // SCLK (RD1) config
    TRISDbits.TRISD1 = 0; // set sclk as output
    
    // CS (RD0) config
    TRISDbits.TRISD0 = 0; // set RD0 as output because it's CS
    LATDbits.LATD0 = 1; // start off with CS selected high
}


/* 
 * puts data in register from master and returns data from slave
 */
uint8_t SPI_transferByte(uint8_t byte) {
    
    // place data in spi buffer
    SPI1BUF = byte;
    
    // wait until master has received data
    while(!SPI1STATbits.SPIRBF);
    
    // return data from spi
    return SPI1BUF;
}


/* 
 * splits 16-bit address into 2 bytes, sends both 
 */
void EEPROM_send16BitAddress(uint16_t address) {
    SPI_transferByte((address >> 8)); // most significant byte 
    SPI_transferByte(address);        // least significant byte
}


/* 
 * sets EEPROM write enable 
 */
void EEPROM_writeEnable(void) {
    SLAVE_SELECT; 
    SPI_transferByte(EEPROM_WREN); // write enable instruction
    SLAVE_DESELECT;
}


/* 
 * writes a single byte to a given memory location 
 */
void EEPROM_writeByte(uint16_t address, uint8_t byte) {
    
    // write enable must be enabled before writing
    EEPROM_writeEnable();
    
    SLAVE_SELECT; // cs low
    
    // write instruction
    SPI_transferByte(EEPROM_WRITE);
    
    // address in memory
    EEPROM_send16BitAddress(address);
    
    // byte to write to eeprom
    SPI_transferByte(byte); 
    
    SLAVE_DESELECT; // cs high
    
    while (EEPROM_readStatus() & EEPROM_WRITE_IN_PROGRESS) {;}
}

/*
 * write two bytes to a given memory location 
 */
/*
void EEPROM_writeWord(uint16_t address, uint16_t word) {
  EEPROM_writeEnable();
  SLAVE_SELECT;
  SPI_transferByte(EEPROM_WRITE);
  EEPROM_send16BitAddress(address);
  SPI_transferByte((word >> 8));
  SPI_transferByte(word);
  SLAVE_DESELECT;
  while (EEPROM_readStatus() & EEPROM_WRITE_IN_PROGRESS) {;}
}
*/


/* 
 * read byte from address in eeprom memory 
 */
uint8_t EEPROM_readByte(uint16_t EEPROM_address) {
    
    // set chip select (low) to select device
    SLAVE_SELECT;

    // read instruction
    SPI_transferByte(EEPROM_READ);
    
    // eeprom memory address
    EEPROM_send16BitAddress(EEPROM_address);
  
    // get data from eeprom by placing
    uint8_t data = SPI_transferByte(0);
     
    // release chip select
    SLAVE_DESELECT;

    return data;
}



/* 
 * gets two bytes from a given memory location 
 */
/*
uint16_t EEPROM_readWord(uint16_t address) {
    uint16_t eepromWord;
    SLAVE_SELECT;
    SPI_transferByte(EEPROM_READ);
    EEPROM_send16BitAddress(address);
    SPI_transferByte(0);
    eepromWord = SPI1BUF;
    eepromWord = (eepromWord << 8); // most-sig bit
    SPI_transferByte(0);
    eepromWord += SPI1BUF;  // least-sig bit
    SLAVE_DESELECT;
    return (eepromWord);
}
*/



/* 
 * reads the EEPROM status register 
 */
uint8_t EEPROM_readStatus(void) {
    SLAVE_SELECT;
    SPI_transferByte(EEPROM_RDSR);
    SPI_transferByte(0xFF);  // clock out eight bits
    SLAVE_DESELECT;
    return (SPI1BUF); // return the result
}



/* 
 * get the EEPROM write status register 
 */
uint8_t EEPROM_writeStatus(void) {
    SLAVE_SELECT;
    SPI_transferByte(EEPROM_WRSR);
    SPI_transferByte(0);  // clock out eight bits
    SLAVE_DESELECT;
    return (SPI1BUF); // return the result
}



/* 
 * sets every byte in memory to zero 
 */
/*
void EEPROM_clearAll(void) {
    uint8_t i;
    uint16_t pageAddress = 0;
    while (pageAddress < EEPROM_BYTES_MAX) {
      EEPROM_writeEnable();
      SLAVE_SELECT;
      SPI_transferByte(EEPROM_WRITE);
      EEPROM_send16BitAddress(pageAddress);
      for (i = 0; i < EEPROM_BYTES_PER_PAGE; i++) {
        SPI_transferByte(0);
      }
      SLAVE_DESELECT;
      pageAddress += EEPROM_BYTES_PER_PAGE;
      while (EEPROM_readStatus() & EEPROM_WRITE_IN_PROGRESS) {;}
    }
}
*/