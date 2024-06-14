# CAN BUS Communication with PIC16F877a

This project demonstrates CAN bus communication using MCP2515 CAN modules and PIC16F877A microcontrollers. The goal is to send and receive messages by pressing a button and to indicate received messages via LEDs.

## Project Description:
In this project, we set up a CAN network with two nodes, a 'master' and a 'slave'. The slave node send messages while the master node receives messages. The slave node has a button that, when pressed, sends its ID to the master and flashes an LED to indicate the message has been sent. The master node is programmed to light up an LED when it receives something.

## Master Node
Microcontroller: PIC16F877A  
CAN Controller: MCP2515  
Oscillator: 4 MHz external oscillator  
LED: Connected to RD2  
SPI Connections:  
SCK (MCP2515 Pin 13) to SCK (PIC Pin 18)  
SI (MCP2515 Pin 14) to SDO (PIC Pin 26)  
SO (MCP2515 Pin 15) to SDI (PIC Pin 25)  
CS (MCP2515 Pin 16) to RD0 (PIC Pin 19)  
Button: Connected to RB3 (Pin 36)  
CANH and CANL: Connected to the CAN bus  

 
## Slave Node  
Microcontroller: PIC16F877A  
CAN Controller: MCP2515  
Oscillator: 4 MHz external oscillator  
LED: Connected to RD2  
SPI Connections:  
SCK (MCP2515 Pin 13) to SCK (PIC Pin 18)  
SI (MCP2515 Pin 14) to SDO (PIC Pin 26)  
SO (MCP2515 Pin 15) to SDI (PIC Pin 25)  
CS (MCP2515 Pin 16) to RD0 (PIC Pin 19)  
Button: Connected to RB3 (Pin 36)  
CANH and CANL: Connected to the CAN bus  

## Next - using pickit 3:
1. Upload the master.c to the designated master MPU
2. Upload the salve.c to the designated slave MPU
 
