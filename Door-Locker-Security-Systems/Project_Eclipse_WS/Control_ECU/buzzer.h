 /******************************************************************************
 *
 * Module: LCD
 *
 * File Name: lcd.h
 *
 * Description: Header file for the LCD driver
 *
 * Author:
 *
 *******************************************************************************/
#ifndef LED_H_
#define LED_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define BUZZER_PORT_ID                  PORTA_ID
#define BUZZER_PIN_ID                   PIN0_ID



/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/


void BUZZER_init(void);


void BUZZER_on(void);


void BUZZER_off(void);


#endif /* LED_H_ */
