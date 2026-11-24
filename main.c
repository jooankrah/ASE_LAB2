/*
 *	Project Owner	: TU Chemnitz
 *	Project Name	: ASE Tutorial Unit-2
 *	File Name		: main.c
 *	Author			: ASE Admin
 *  Created on		: 25 Feb 2024
 */

/* Includes ******************************************************************/
#include "init.h"


/* Global variables **********************************************************/
int speed = 0;
int rpm = 0;
static volatile int engine_on = 0;

int calculated_speed(int pot_value){
	if (pot_value < 0)
{
    pot_value = 0;
}

if (pot_value > 4095)
{
    pot_value = 4095;
}

return (pot_value * 240) / 4095;
}

int calculated_rpm(){
	static int rpm_value = 8000;
	static int step = -250;
	uint current_rpm =(int)rpm_value;

	rpm_value += step;

	if(rpm_value <= 0){
		rpm_value = 0;
		step = 250;
	}
	else if(rpm_value >= 8000){
		rpm_value = 8000;
		step = -250;
	}
	return current_rpm;
}

void can_send_engine_on(void){

	/* TO-DO: your task implementations **************************************/
	// write the correct CODE value to this register to instruct the CAN controller to transmit the CAN message
	CAN_0.BUF[CAN_TX_ENGINE_MB].CS.B.CODE = 0xC;
	LED_Tx = !LED_Tx; // toggle the LED_Tx to indicate a message has been transmitted
}

void can_send_speed(int value){
	/* TO-DO: your task implementations **************************************/
	CAN_0.BUF[CAN_TX_SPEED_MB].DATA.B[0] = value; // set the first byte of the payload to the speed value
	CAN_0.BUF[CAN_TX_SPEED_MB].CS.B.CODE = 0xC; // instruct the CAN controller to transmit the CAN message
	LED_Tx = !LED_Tx; // toggle the LED_Tx to indicate a message has been transmitted
}

void can_send_rpm(int value){
	/* TO-DO: your task implementations **************************************/
	CAN_0.BUF[CAN_TX_RPM_MB].DATA.B[0] = value & 0xFF; // set the first byte of the payload to the lower 8 bits of the rpm value
	CAN_0.BUF[CAN_TX_RPM_MB].DATA.B[1] = (value >> 8) & 0xFF; // set the second byte of the payload to the upper 8 bits of the rpm value
	CAN_0.BUF[CAN_TX_RPM_MB].CS.B.CODE = 0xC; // instruct the CAN controller to transmit the CAN message
	LED_Tx = !LED_Tx; // toggle the LED_Tx to indicate a message has been transmitted
}

void can_send_lights(void){
	/* TO-DO: your task implementations **************************************/
	int lights_state = 0x00; // initialize the lights state variable to 0
	if (SW1 == 0 && SW2 == 1)
	{
		// left turn signal
		lights_state = 0x01; // set the lights state to 1 if SW1 is pressed and SW2 is not pressed
	}
	else if (SW1 == 1 && SW2 == 0)
	{
		// right turn signal
		lights_state = 0x02; // set the lights state to 2 if SW2 is pressed and SW1 is not pressed
	}
	else if (SW1 == 0 && SW2 == 0)
	{
		// hazard lights
		lights_state = 0x03; // set the lights state to 3 if both SW1 and SW2 are pressed
	}
	else
	{
		lights_state = 0x00; // set the lights state to 0 if neither SW1 nor SW2 are pressed
	}
	CAN_0.BUF[CAN_TX_LIGHTS_MB].DATA.B[0] = lights_state; // set the first byte of the payload to the lights state
	CAN_0.BUF[CAN_TX_LIGHTS_MB].CS.B.CODE = 0xC; // instruct the CAN controller to transmit the CAN message
	LED_Tx = !LED_Tx; // toggle the LED_Tx to indicate a message has been transmitted
}

void can_send(void){

	/* TO-DO: your task implementations **************************************/
	if (!engine_on){
		return; // if the engine is not on, do not send any CAN messages
	}
	speed = calculated_speed(POT); // read the potentiometer value and calculate the speed
	rpm = calculated_rpm(); // calculate the rpm value
	can_send_engine_on();
	can_send_speed(speed);
	can_send_rpm(rpm);
	can_send_lights();

	/*************************************************************************/
}


/*
 * @brief	Main program
 *
 * @param	void
 * @retval	int
 */
int main(void) {
	/* peripherals initialization */
	peripheralsInit();



	/* TO-DO: your task implementations **************************************/

	/* Configure and start timer channels */
	/* PIT timer channel 1, period = 1000 ms */

	/*************************************************************************/


	/* main program */
	while(1){
		/* System function */
		systemFunction();

		/* TO-DO: your task implementations **********************************/

		can_send();

		/*********************************************************************/

		/* 10 ms OS delay */
		osalThreadDelayMilliseconds(10UL);
	}
}


/*
 * @brief	CAN receive interrupt
 *
 * @param   CANDriver   canp	The CAN controller which received a message
 * @param	CANRxFrame	crfp	The message which was received
 * @retval	void
 */
void can_receive(CANRxFrame crfp) {
    if(crfp.IDE == CAN_IDE_STD) // a standard message frame has been received
    {
        /* TO-DO: your task implementations ******************************/
		LED_Rx = !LED_Rx; // toggle the LED_Rx to indicate a message has been received

        if (crfp.SID == 0x11) // check the Message ID
        {
			engine_on = 1; // set the engine_on flag to indicate that a message with ID 0x11 has been received
			LED_U1 = !LED_U1; // toggle the LED_U1 to indicate a message with ID 0x11 has been received
		}
		else if (crfp.SID == 0x10) // check the Message ID
		{
			engine_on = 0; // set the engine_on flag to indicate that a message with ID 0x10 has been received
			LED_P = 0; 
		}
    }
}


/*
 * @brief	PIT timer channel 1 IRQ callback
 *
 * @param	void
 * @retval	void
 */
void PIT_Channel_1(void){
	/* TO-DO: your task implementations **************************************/


}


/*
 * @brief	PIT timer channel 2 IRQ callback
 *
 * @param	void
 * @retval	void
 */
void PIT_Channel_2(void){
	/* TO-DO: your task implementations **************************************/

}


/*
 * @brief	PIT timer channel 3 IRQ callback
 *
 * @param	void
 * @retval	void
 */
void PIT_Channel_3(void){
	/* TO-DO: your task implementations **************************************/


}
