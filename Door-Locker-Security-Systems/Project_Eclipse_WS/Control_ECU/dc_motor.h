 /******************************************************************************
 *
 * Module: DC MOTOR
 *
 * File Name: DC MOTOR.h
 *
 * Description: header file for the DC MOTOR
 *
 * Author: Mohamed Taha
 *
 *******************************************************************************/

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include "std_types.h"


/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define DC_MOTOR_OUT_FIRST_PORT_ID                 PORTD_ID
#define  DC_MOTOR_OUT_FIRST_PIN_ID                 PIN7_ID

#define DC_MOTOR_OUT_SECOND_PORT_ID                PORTD_ID
#define DC_MOTOR_OUT_SECOND_PIN_ID                 PIN6_ID


/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/

typedef enum
{
	STOP,CW,A_CW
}DcMotor_State;



/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/*
 * Description :
 * A function to initialize the motor
 */
void DcMotor_Init(void);
/*
 * Description :
 * A function to control on motor ,off , direction and speed
 */
void DcMotor_Rotate(DcMotor_State state,uint8 speed);



#endif /* DC_MOTOR_H_ */
