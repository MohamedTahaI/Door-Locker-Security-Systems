 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: Mohamed Tarek
 *
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "std_types.h"

//for data sheet get this data
typedef enum
{

	five_bit ,
	six_bit ,
	seven_bit ,
	eight_bit ,
	Reserved_1,
	Reserved_2,
	Reserved_3,
	ninth_bit
}UART_BitData;

typedef enum
{
	Disabled,
	Reserved,
	Enabled_even,
	Enabled_odd
}UART_Parity;

typedef enum
{
	one_bit,
	two_bit
}UART_StopBit;

//for 8 Mhz
typedef enum
{
	baud_2400 =2400,
	baud_4800 =4800,
	baud_9600 =9600 ,
	baud_14k  =14400 ,
	baud_19k  =19200 ,
	baud_28k  =28800 ,
	baud_38k  =38400 ,
	baud_57k  =57600  ,
	baud_76k  =76800  ,
	baud_115k =115200  ,
	baud_230k =230000  ,
	baud_250k =250000  ,
	baud_halfM=500000
}UART_BaudRate;


typedef struct{
 UART_BitData bit_data;
 UART_Parity parity;
 UART_StopBit stop_bit;
 UART_BaudRate baud_rate;
}UART_ConfigType;


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType * Config_Ptr);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */
