/*
 ============================================================================
 Name        : main_Control_ECU.C
 Author      : Mohamed TaHa
 ============================================================================
 */

/*******************************************************************************
 *                                Includes                                  *
 *******************************************************************************/


#include "buzzer.h"
#include "dc_motor.h"
#include "uart.h"
#include "twi.h"
#include "external_eeprom.h"
#include "timer1.h"

#include "std_types.h"


#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/


#define NUMBER_FOR_PASSWORD     5
#define MAX_TRYING              3

#define STEP_1_SAVE_PASSWORD   0xE1
#define STEP_2_OPEN_DOOR       0xE3
#define STEP_3_CHANGE_PASSWORD 0xE2
#define STEP_4_BUZZER_ON       0xE4


#define START_SEND_PASSWOED     0xF0
#define DOOR_OPEN               0xF1
#define DOOR_CLOSED             0xF2
#define SUCCESS_MC              0xF3
#define FAILED				    0xF4
#define ERROR_MC		        0xF5
#define START_SEND_PASSWOED_ASK 0xF6

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

void RECEIVE_PASSWORD (void);
void SAVE_PASSWORD_FOR_EPPROM(void);
uint8 CHECK_PASSWORD_FROM_EPPROM(void);
void OPEN_DOOR (void);
void TURN_ON_BUZZER (void);
void delaySeconds(uint8 sec);
void countSec(void);

/*******************************************************************************
 *                             global variables                                *
 *******************************************************************************/

uint8  g_sec =0 ;
uint8  password[NUMBER_FOR_PASSWORD]={1,2,3,4,5};
uint8  checkPassword[NUMBER_FOR_PASSWORD]={0};

/*******************************************************************************
 *                              main code                                      *
 *******************************************************************************/

int main()
{
	// local variable and Set directions and Initial value
	//config to the uart driver
	uint8 Data;
	UART_ConfigType uartConfig = {eight_bit ,Disabled,one_bit,baud_9600};
	TWI_ConfigType twiConfig ={0x01,Prescaler_1};

	//setup driver
	BUZZER_init();
	DcMotor_Init();
	TWI_init(&twiConfig);
	UART_init(&uartConfig);
	SREG |= (1<<7);

	while(1)
	{
		// waiting receive any step . what you do HMI_ECU ?
		Data =UART_recieveByte();
		switch(Data)
		{
		/* if send STEP_1_SAVE_PASSWORD jump here and run this code
		 *
		 */
		case  STEP_1_SAVE_PASSWORD :

			RECEIVE_PASSWORD(); // receive password
			SAVE_PASSWORD_FOR_EPPROM(); //save password in EEPROM
			UART_sendByte(SUCCESS_MC);	//sent to HMI micro success save password

			break;
		case  STEP_2_OPEN_DOOR :
			/*
			 * if send STEP_2_OPEN_DOOR jump here and run this code
			 */
			RECEIVE_PASSWORD(); // receive password
			if(CHECK_PASSWORD_FROM_EPPROM()) // check if the same password in eeprom
			{
				UART_sendByte(SUCCESS_MC); // send SUCCESS_MC to HMI micro
				OPEN_DOOR(); //call open door
			}
			else
			{
				UART_sendByte(ERROR_MC); // else not correct password send ERROR_MC to HMI micro
			}
			break;
		case  STEP_3_CHANGE_PASSWORD :
			/*
			 * if send STEP_3_CHANGE_PASSWORD jump here and run this code
			 */
			RECEIVE_PASSWORD(); // receive password
			if(CHECK_PASSWORD_FROM_EPPROM()) // check if the same password in eeprom
			{
				SAVE_PASSWORD_FOR_EPPROM(); //save new password to EEPROM
				UART_sendByte(SUCCESS_MC); // send success when save password
			}
			else
			{

				UART_sendByte(ERROR_MC); // else password not correct send ERROR_MC to  HMI micro


			}
			break;
		case  STEP_4_BUZZER_ON  :

			/*
			 * if send STEP_4_BUZZER_ON jump here and run this code
			 */

			TURN_ON_BUZZER(); //call function turn on buzzer to alarm admin

			break;
		default:
			// Nothing
			break;
		}

	}

}



/*******************************************************************************
 *                              Functions                                      *
 *******************************************************************************/

void RECEIVE_PASSWORD (void)
{
	// the first receive password from MC1 element  BY element
	uint8 i;
	while(UART_recieveByte()!= START_SEND_PASSWOED_ASK );
	UART_sendByte(START_SEND_PASSWOED);
	for(i=0 ; i < NUMBER_FOR_PASSWORD ; i++)
	{
		password[i]=UART_recieveByte();
	}
}

void SAVE_PASSWORD_FOR_EPPROM(void)
{
	// the second save password for  EEPROM element  BY element
	uint8 i=0;
	for(i=0 ; i < NUMBER_FOR_PASSWORD ; i++)
	{
		EEPROM_writeByte( (0x0300)+i , password[i]);
		_delay_ms(10);
	}

}
uint8 CHECK_PASSWORD_FROM_EPPROM(void)
{
	// check password for  EEPROM element  BY element and return true or false
	uint8 i,count=0;
	for(i=0 ; i < NUMBER_FOR_PASSWORD ; i++)
	{
		EEPROM_readByte( (0x0300)+i ,&checkPassword[i]);
		_delay_ms(10);
	}
	for(i=0 ; i < NUMBER_FOR_PASSWORD ; i++)
	{
		if( checkPassword[i]==password[i])
			count++;
	}
	if(count==NUMBER_FOR_PASSWORD)
	{
		return 1;
	}
	return FALSE;
}

void OPEN_DOOR (void)
{
	//rotates motor for 15-seconds CW and display a message on the screen
	//“Door is Unlocking”
	DcMotor_Rotate(CW,100);
	UART_sendByte(DOOR_OPEN);
	delaySeconds(15);

	// hold the motor for 3-seconds.
	DcMotor_Rotate(STOP, 100);
	delaySeconds(3);
	//rotates motor for 15-seconds A-CW and display a message on the screen
	//“Door is Locking”
	DcMotor_Rotate(A_CW,100);
	UART_sendByte(DOOR_CLOSED);
	delaySeconds(15);
	// STOP MOTOR
	DcMotor_Rotate(STOP, 100);
}

void TURN_ON_BUZZER (void)
{
	//Activate Buzzer for 1-minute.
	BUZZER_on();
	delaySeconds(60);
	BUZZER_off();
}

/*
 * Description :
 * A function that counts the sec by causing an interrupt every second and increase our global variable
 */
void delaySeconds(uint8 sec)
{
	/* setting the callBack function to count seconds */
	TIMER1_setCallBack(countSec);
	/* timer1 configurations to count sec per interrupt */
	Timer1_ConfigType timer1Config = {0,31250,N_256,COMPARE};

	TIMER1_init(&timer1Config);
	while(g_sec < sec);
	g_sec = 0;
	TIMER1_deInit();

}

/*
 * Description :
 * A function that increase our global variable each second
 */
void countSec(void)
{
	g_sec++;
}





