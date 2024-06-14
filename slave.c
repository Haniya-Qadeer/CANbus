#include <xc.h>
#include <string.h>
 
// Configuration Bits
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (LVP disabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)
#define _XTAL_FREQ 4000000     // 4 MHz external oscillator frequency
 
#define SPI_WRITE 0x02
#define SPI_READ 0x03
#define BIT_MODIFY 0x05
#define READ_STATUS 0xA0
#define RX_STATUS 0xB0
#define RTS_TXB0 0x81
#define RTS_TXB1 0x82
#define RTS_TXB2 0x84
#define CNF1 0x2A
#define CNF2 0x29
#define CNF3 0x28
#define CANCTRL 0x0F
#define CANINTE 0x2B
#define CANINTF  0x2C
 
#define LOAD_TX0 0x40
#define RTS_TX0 0x81
#define TXB0SIDH 0x31
#define TXB0SIDL 0x32
#define TXB0EID8 0x33
#define TXBnCTRL 0x30
#define TXB0EID0 0x34
#define TXB0DLC  0x35
#define TXB0D0   0x36
 
void MCP2515_Reset(void);
void MCP2515_Init(void);
void MCP2515_SendMessage(unsigned char id);
void blinkLED(int ms);
void MCP_write(char add, char data);
char MCP_read(char add);
void SPI_Init();
void SPI_Write(unsigned char data);
unsigned char SPI_Read(void);
void MCP2515_LoadTX(unsigned char buffer, unsigned long id, unsigned char dlc, unsigned char *data);
unsigned char MCP2515_ReadRXB0(unsigned long *id, unsigned char *dlc, unsigned char *data);
 
void main() {
    // SLAVE
    TRISBbits.TRISB3 = 1; // Configure RB3 as input for Button
    TRISD = 0; // Configure RD2 as output for LED
    PORTDbits.RD2 = 1; // Initial LED state
 
    SPI_Init(); // Initialize SPI
    MCP2515_Init(); // Initialize MCP2515
    blinkLED(100);
 
    while (1) {
        if (PORTBbits.RB3 == 1) {
            PORTDbits.RD2 = 0;
            // Button pressed, send CAN message to Master
            MCP2515_LoadTX(0, 0x123, 1, 0x01);
            __delay_ms(1000); // Debounce delay
            blinkLED(100);
        }
        if (PORTBbits.RB3 == 0) {
            PORTDbits.RD2 = 1;
        }
    }
}
 
void MCP2515_Reset(void) {
    PORTDbits.RD0 = 0; // Select MCP2515
    SPI_Write(0xC0); // Send reset command
    PORTDbits.RD0 = 1; // Deselect MCP2515
    __delay_ms(1); // Wait for the reset to complete
}
 
void MCP2515_Init(void) {
    MCP2515_Reset();
    MCP_write(CANCTRL, 0x80);
    MCP_write(CNF1, 0x03);
    MCP_write(CNF2, 0x90);
    MCP_write(CNF2, 0x02);
    MCP_write(CANINTE, 0x1F);
    MCP_write(CANCTRL, 0x00);
}
 
void MCP2515_SendMessage(unsigned char id) {
    PORTDbits.RD0 = 0; // Select MCP2515
    SPI_Write(0x40); // Load TX buffer command
    SPI_Write(id); // Load identifier
    PORTDbits.RD0 = 1; // Deselect MCP2515
    PORTDbits.RD0 = 0; // Select MCP2515
    SPI_Write(0x80); // Request to send command
    PORTDbits.RD0 = 1; // Deselect MCP2515
}
 
unsigned char MCP2515_ReceiveMessage(unsigned char* id) {
    PORTDbits.RD0 = 0; // Select MCP2515
    SPI_Write(0x90); // Read RX buffer command
    *id = SPI_Read(); // Read identifier
    PORTDbits.RD0 = 1; // Deselect MCP2515
    return 1; // Indicate a message was received
}
 
char MCP_read(char add) {
    char data;
    PORTDbits.RD0 = 0; // Select MCP2515
    SPI_Write(0x03);
    SPI_Write(add);
    data = SPI_Read();        
    PORTDbits.RD0 = 1; // Deselect MCP2515
    return data;
}
 
void MCP_write(char add, char data) {
    PORTDbits.RD0 = 0; // Select MCP2515
    SPI_Write(0x02);
    SPI_Write(add);
    SPI_Write(data);      
    PORTDbits.RD0 = 1; // Deselect MCP2515    
}
 
void SPI_Init() {
    SSPCON = 0x20;
    SSPSTAT = 0X00;
    TRISCbits.TRISC5 = 0; // SDO output
    TRISCbits.TRISC4 = 1; // SDI input
    TRISCbits.TRISC3 = 0; // SCK output for Master (ignored for Slave)
    TRISBbits.TRISB0 = 0;
    PORTDbits.RD0 = 1; // Set CS high (inactive)
}
 
void SPI_Write(unsigned char data) {
    SSPBUF = data; // Load data into buffer
    while(!SSPSTATbits.BF); // Wait until transmission is complete
}
 
unsigned char SPI_Read(void) {
    SPI_Write(0x00); // Write dummy data to initiate SPI read
    while(!PIR1bits.SSPIF);
    return SSPBUF; // Return the received data
}
 
void blinkLED(int ms) {
    PORTDbits.RD2 = 0;
    __delay_ms(ms); 
    PORTDbits.RD2 = 1;
}
 
void MCP2515_LoadTX(unsigned char buffer, unsigned long id, unsigned char dlc, unsigned char *data) {
    unsigned char address;
    // Select the appropriate TX buffer
    if (buffer == 0) address = TXB0SIDH;
    else return; // Only buffer 0 is handled in this example
 
    // Set the Standard Identifier
    MCP_write(address, (unsigned char)(id >> 3));
    MCP_write(address + 1, (unsigned char)(id << 5));
 
    // Set the Data Length Code
    MCP_write(address + 4, dlc);
 
    // Load the data bytes
    for (unsigned char i = 0; i < dlc; i++) {
        MCP_write(address + 5 + i, data[i]);
    }
 
    // Request to Send the message
    PORTDbits.RD2 = 0;
    SPI_Write(RTS_TX0);
    PORTDbits.RD2 = 1;
}
 
unsigned char MCP2515_ReadRXB0(unsigned long *id, unsigned char *dlc, unsigned char *data) {
    // Check if there is a new message in RX buffer 0
    unsigned char status = MCP_read(CANINTF);
    if (!(status & 0x01)) {
        return 0; // No new message in RXB0
    }
 
    // Read the Standard Identifier
    unsigned char sidh = MCP_read(RXB0SIDH);
    unsigned char sidl = MCP_read(RXB0SIDL);
    *id = ((unsigned long)sidh << 3) | (sidl >> 5);
 
    // Read the Data Length Code
    *dlc = MCP_read(RXB0DLC) & 0x0F;
 
    // Read the data bytes
    for (unsigned char i = 0; i < *dlc; i++) {
        data[i] = MCP_read(RXB0D0 + i);
    }
 
    // Clear the RX buffer 0 interrupt flag
    MCP_write(CANINTF, status & ~0x01);
 
    return 1; // Message read successfully
}
