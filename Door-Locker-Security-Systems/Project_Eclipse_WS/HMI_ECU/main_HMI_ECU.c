/*
 ============================================================================
 Name        : MAIN_HMI_ECU.C
 Author      : Mohamed TaHa
 ============================================================================
 */

/*******************************************************************************
 *                                Includes                                  *
 *******************************************************************************/
#include "std_types.h"
#include "lcd.h"
#include "keypad.h"
#include "uart.h"
#include "timer1.h"

#include <util/delay.h> /* For the delay functions */
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
 *                               Types Declaration                             *
 *******************************************************************************/

// state step to system direction
typedef enum state
{
	Step1_CreatePassword     ,
	Step1_send_save_password ,
	Step2_main_Options       ,
	Step3_OpenDoor           ,
	Step4_ChangePassword     ,
	Step5
}state;

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

void CREATE_PASSWORD (void);
void SEND_SAVE_PASSWORD(void);
void MAIN_OPTIONS(void);
void OPEN_DOOR(void);
void CHANGE_PASSWORD(void);
void ERROR_SYSTEM (void);

void GET_PASSWORD (void);
void GET_SECOND_PASSWORD (void);
void SUCCESS_OPEN_DOOR (void);
void ERROR_MASSEGE (void);
void SEND_PASSWORD(void);
void delaySeconds(uint8 sec);
void countSec(void);

/*******************************************************************************
 *                             global variables                                *
 *******************************************************************************/
//Initial vale
uint8 g_sec=0;
uint8 counter_try=0;
uint8 password[NUMBER_FOR_PASSWORD]={0};
uint8 checkPassword[NUMBER_FOR_PASSWORD]={0};
state status_step = Step1_CreatePassword;

/*******************************************************************************
 *                              main code                                      *
 *******************************************************************************/

int main(void)
{
	//config to the uart driver
	UART_ConfigType uartConfig = {eight_bit ,Disabled,one_bit,baud_9600};
	//setup driver
	UART_init(&uartConfig);
	LCD_init();
	SREG |= (1<<7);
	//welcome message
	LCD_displayStringRowColumn(0,4,"Welcome");
	delaySeconds(1);

	while(1)
	{
		switch(status_step)
		{
		case Step1_CreatePassword :
			// the first step create password
			CREATE_PASSWORD();

			break;
		case Step1_send_save_password :
			// the second step send this password to control_ECU and save him into EEPROM
			SEND_SAVE_PASSWORD();

			break;
		case Step2_main_Options :
			//the step number 2 is display the main options and if chose same one go to the chose step
			MAIN_OPTIONS();

			break;
		case  Step3_OpenDoor :
			// When user chooses the '+' option jump to here
			// call function open door
			OPEN_DOOR();

			break;
		case Step4_ChangePassword :
			// When user chooses the '-' option jump to here
			// call function change
			CHANGE_PASSWORD();

			break;
		case Step5 :
			// When user entered 3 try password not correct the code  jump to here
			// call function ERROR_SYSTEM
			ERROR_SYSTEM();
			break;
		default:

			break;
		}

	}
}




/*******************************************************************************
 *                              Functions                                      *
 *******************************************************************************/


/*
 * Description :
 * A function that create new password   buy get two password from user and compare them
 * if matching save password in EEPROM
 */

void CREATE_PASSWORD (void)
{
	uint8 i,counter=0;

	GET_PASSWORD();
	GET_SECOND_PASSWORD();
	for(i=0 ; i< NUMBER_FOR_PASSWORD ; i++)
	{
		if(password[i] == checkPassword[i])
			counter++;

	}

	if(counter == NUMBER_FOR_PASSWORD )
	{
		status_step = Step1_send_save_password;
	}
	else
	{
		status_step = Step1_CreatePassword;
	}


}

/*
 * Description :
 * A function that save password in EEPROM
 * and go step 2 main options
 */

void SEND_SAVE_PASSWORD(void)
{

	UART_sendByte(STEP_1_SAVE_PASSWORD);
	SEND_PASSWORD();
	while(UART_recieveByte()!= SUCCESS_MC );
	status_step = Step2_main_Options;

}
/*
 * Description :
 * A function that display main options
 * and go step3 (open door) if press '+' or if press '-' go to step4 (change password)
 */

void MAIN_OPTIONS(void)
{
	uint8 PressedKey ;

	LCD_displayStringRowColumn(0, 0, "+ : open Door ");
	LCD_displayStringRowColumn(1, 0, "- : change pass ");

	PressedKey= KEYPAD_getPressedKey();

	switch(PressedKey)
	{
	case '+' :
		LCD_clearScreen();
		status_step = Step3_OpenDoor;
		break;
	case '-' :
		LCD_clearScreen();
		status_step = Step4_ChangePassword;
		break;
	default :
		// nothing
		break;
	}

}

/*
 * Description :
 * A function that open door
 * the first get password from user
 * the second check password form EEPROM
 * if matching  call function SUCCESS_OPEN_DOOR() else check if user try 3time filed go step5 error system
 * if user not try 3time filed go the same step to try enter password
 */

void OPEN_DOOR (void)
{
	uint8 data;
	UART_sendByte(STEP_2_OPEN_DOOR);
	GET_PASSWORD();
	SEND_PASSWORD();
	data= UART_recieveByte();

	if(data == SUCCESS_MC)
	{
		counter_try=0;
		SUCCESS_OPEN_DOOR();
		status_step = Step2_main_Options;
	}
	if(data == ERROR_MC)
	{
		counter_try++;
		if(counter_try ==MAX_TRYING )
		{
			LCD_clearScreen();
			status_step=Step5;
		}
		else
		{
			ERROR_MASSEGE();
			status_step = Step3_OpenDoor;
		}
	}

}


/*
 * Description :
 * A function that change password
 * the first get password from user
 * send password to EEPROM and receive
 * if send SUCCESS_MC go step1 to create password
 * if send ERROR_MC the 3 time go to step5 (system error)
 *
 */
void CHANGE_PASSWORD(void)
{
	uint8 data;
	UART_sendByte(STEP_3_CHANGE_PASSWORD);
	GET_PASSWORD();
	SEND_PASSWORD();
	data= UART_recieveByte();

	if(data == SUCCESS_MC)
	{
		counter_try=0;
		status_step = Step1_CreatePassword;
	}
	if(data == ERROR_MC)
	{
		counter_try++;
		if(counter_try ==MAX_TRYING )
		{
			LCD_clearScreen();
			status_step=Step5;
		}
		else
		{
			ERROR_MASSEGE();
			status_step = Step4_ChangePassword;
		}
	}

}



/*
 * Description :
 * A function that error system
 * send buzzer turn on
 * display error message
 * delay 1 min
 */
void ERROR_SYSTEM (void)
{
	UART_sendByte(STEP_4_BUZZER_ON);
	LCD_displayStringRowColumn(0, 0, " ERROR SYSTEM ");
	LCD_displayStringRowColumn(1, 0, " Max TRY PASS ");
	delaySeconds(60);
	status_step = Step2_main_Options;
}


/*
 * Description :
 * A function that get password from user when press '='
 *
 */

void GET_PASSWORD (void)
{
	uint8 i=0 ,key;
	// rest the password to five zero
	for(i =0 ; i< NUMBER_FOR_PASSWORD ; i++)
	{
		password[i]=0;
	}

	LCD_displayStringRowColumn(0, 0, "plz enter pass:");
	LCD_moveCursor(1, 0);
	i=0;

	while(1)
	{
		key = KEYPAD_getPressedKey();
		if(key != '=' )
		{
			LCD_displayCharacter('*');
			password[i]=key;
			i++;
		}
		else
		{
			LCD_clearScreen();
			break;
		}
	}
}


/*
 * Description :
 * A function that get second password from user when press '='
 * to compare two password
 */
void GET_SECOND_PASSWORD (void)
{
	uint8 i=0 ,key;

	LCD_displayStringRowColumn(0, 0, "plz re-enter the");
	LCD_displayStringRowColumn(1, 0, "same pass:");
	LCD_moveCursor(1,10);

	while(1)
	{
		key = KEYPAD_getPressedKey();
		if(key != '=' )
		{
			LCD_displayCharacter('*');
			checkPassword[i]=key;
			i++;
		}
		else
		{
			LCD_clearScreen();
			break;
		}
	}
}


/*
 * Description :
 * A function that open door
 * receive from control_ECU and display receive
 *
 */
void SUCCESS_OPEN_DOOR (void)
{
	LCD_clearScreen();
	while(UART_recieveByte()!= DOOR_OPEN );
	LCD_displayString("Door isUnlocking");
	while(UART_recieveByte()!= DOOR_CLOSED );
	LCD_clearScreen();
	LCD_displayString("Door isLocking");
	delaySeconds(15);
	LCD_clearScreen();
}


/*
 * Description :
 * A function that display error message
 */
void ERROR_MASSEGE (void)
{
	LCD_clearScreen();
	LCD_displayString("pass not correct");
	delaySeconds(2);
	LCD_clearScreen();

}


/*
 * Description :
 * A function that send password to control_ECU
 */
void SEND_PASSWORD(void)
{
	uint8 i;
	UART_sendByte(START_SEND_PASSWOED_ASK);
	while(UART_recieveByte()!= START_SEND_PASSWOED );
	for(i=0 ; i < NUMBER_FOR_PASSWORD ; i++)
	{
		UART_sendByte(password[i]);
	}

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
