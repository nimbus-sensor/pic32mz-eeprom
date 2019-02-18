#ifndef _EEPROM_H    /* Guard against multiple inclusion */
#define _EEPROM_H

#include <stdint.h>
#include <proc/p32mz2048efh144.h>


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

    
#define SLAVE_SELECT    LATDbits.LATD0 = 0; // select slave device (low)
#define SLAVE_DESELECT  LATDbits.LATD0 = 1; // deselect slave device (high)
    
   
// Instruction Set -- from data sheet
#define EEPROM_READ      0b00000011    // read memory
#define EEPROM_WRITE     0b00000010    // write to memory

#define EEPROM_WRDI      0b00000100    // write disable
#define EEPROM_WREN      0b00000110    // write enable

#define EEPROM_RDSR      0b00000101    // read status register
#define EEPROM_WRSR      0b00000001    // write status register
    

// EEPROM Status Register Bits -- from data sheet
// Use these to parse status register
#define EEPROM_WRITE_IN_PROGRESS    0
#define EEPROM_WRITE_ENABLE_LATCH   1
#define EEPROM_BLOCK_PROTECT_0      2
#define EEPROM_BLOCK_PROTECT_1      3

#define EEPROM_BYTES_PER_PAGE       64
#define EEPROM_BYTES_MAX            0x7FFF
    

// init SPI to run EEPROM
void SPI_Init(void);


// puts data in register from master and returns data from slave
uint8_t SPI_transferByte(uint8_t byte);


// splits 16-bit address into 2 bytes, sends both 
void EEPROM_send16BitAddress(uint16_t address);


// sets EEPROM write enable 
void EEPROM_writeEnable(void);


// writes a single byte to a given memory location 
void EEPROM_writeByte(uint16_t address, uint8_t byte);


// write two bytes to a given memory location 
//void EEPROM_writeWord(uint16_t address, uint16_t word);


// read byte from address in eeprom memory 
uint8_t EEPROM_readByte(uint16_t address);


// reads two bytes from a given memory location 
//uint16_t EEPROM_readWord(uint16_t address);


// reads the EEPROM status register 
uint8_t EEPROM_readStatus(void);

// get the EEPROM write status register 
uint8_t EEPROM_writeStatus(void);


// sets every byte in memory to zero 
//void EEPROM_clearAll(void);



/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EEPROM_H */
