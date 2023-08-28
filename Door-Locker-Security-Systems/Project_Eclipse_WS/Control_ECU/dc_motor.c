 /******************************************************************************
 *
 * Module: DC MOTOR
 *
 * File Name: DC MOTOR.h
 *
 * Description: Source file for the DC MOTOR
 *
 * Author: Mohamed Taha
 *
 *******************************************************************************/


#include "dc_motor.h"
#include "gpio.h"
#include "pwm.h"

void DcMotor_Init(void)
{
	// put two pin output to motor
	GPIO_setupPinDirection(DC_MOTOR_OUT_FIRST_PORT_ID ,DC_MOTOR_OUT_FIRST_PIN_ID ,PIN_OUTPUT);
	GPIO_setupPinDirection(DC_MOTOR_OUT_SECOND_PORT_ID ,DC_MOTOR_OUT_SECOND_PIN_ID ,PIN_OUTPUT);
   // by default  close motor
	GPIO_writePin(DC_MOTOR_OUT_FIRST_PORT_ID ,DC_MOTOR_OUT_FIRST_PIN_ID ,LOGIC_LOW );
	GPIO_writePin(DC_MOTOR_OUT_SECOND_PORT_ID ,DC_MOTOR_OUT_SECOND_PIN_ID ,LOGIC_LOW );

}
// need the direction and speed and not return
void DcMotor_Rotate(DcMotor_State state,uint8 speed)
{
	switch(state)
	{
	case STOP :
		//stop motor
		GPIO_writePin(DC_MOTOR_OUT_FIRST_PORT_ID ,DC_MOTOR_OUT_FIRST_PIN_ID ,LOGIC_LOW );
		GPIO_writePin(DC_MOTOR_OUT_SECOND_PORT_ID ,DC_MOTOR_OUT_SECOND_PIN_ID ,LOGIC_LOW );
		//put speed to pwm driver
		PWM_Timer0_Start(speed);

	break;
	case CW :
		//clock wise direction
		GPIO_writePin(DC_MOTOR_OUT_FIRST_PORT_ID ,DC_MOTOR_OUT_FIRST_PIN_ID,LOGIC_HIGH );
		GPIO_writePin(DC_MOTOR_OUT_SECOND_PORT_ID ,DC_MOTOR_OUT_SECOND_PIN_ID ,LOGIC_LOW );
		//put speed to pwm driver
		PWM_Timer0_Start(speed);

	break;
	case A_CW :

		// anti clock wise
		GPIO_writePin(DC_MOTOR_OUT_FIRST_PORT_ID ,DC_MOTOR_OUT_FIRST_PIN_ID,LOGIC_LOW );
		GPIO_writePin(DC_MOTOR_OUT_SECOND_PORT_ID ,DC_MOTOR_OUT_SECOND_PIN_ID ,LOGIC_HIGH );
		//put speed to pwm driver
		PWM_Timer0_Start(speed);

	break;
	}

}
