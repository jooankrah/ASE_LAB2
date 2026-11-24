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
int rpm = 8000;
static volatile int engine_on = 0;
static volatile int lights_state = 0;
static volatile int indicator_phase = 0;

int calculated_speed(int pot_value){
	if (pot_value < 0) {
		pot_value = 0;
	}
	if (pot_value > 4095) {
		pot_value = 4095;
	}
	return (pot_value * 240) / 4095;
}

int calculated_rpm(){
	static int rpm_value = 8000;
	static int step = -250;
	int current_rpm = rpm_value;

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
}

void can_send_speed(int value){
	/* TO-DO: your task implementations **************************************/
	CAN_0.BUF[CAN_TX_SPEED_MB].DATA.B[0] = value; // set the first byte of the payload to the speed value
	CAN_0.BUF[CAN_TX_SPEED_MB].CS.B.CODE = 0xC; // instruct the CAN controller to transmit the CAN message
}

void can_send_rpm(int value){
	/* TO-DO: your task implementations **************************************/
	CAN_0.BUF[CAN_TX_RPM_MB].DATA.B[0] = value & 0xFF; // set the first byte of the payload to the lower 8 bits of the rpm value
	CAN_0.BUF[CAN_TX_RPM_MB].DATA.B[1] = (value >> 8) & 0xFF; // set the second byte of the payload to the upper 8 bits of the rpm value
	CAN_0.BUF[CAN_TX_RPM_MB].CS.B.CODE = 0xC; // instruct the CAN controller to transmit the CAN message
}

void can_send_lights(void){
	/* TO-DO: your task implementations **************************************/
	if (SW1 == 0 && SW2 == 1)
	{
		lights_state = 0x01;
	}
	else if (SW1 == 1 && SW2 == 0)
	{
		lights_state = 0x02;
	}
	else if (SW1 == 1 && SW2 == 1)
	{
		lights_state = 0x03;
	}
	else
	{
		lights_state = 0x00;
	}
	CAN_0.BUF[CAN_TX_LIGHTS_MB].DATA.B[0] = lights_state;
	CAN_0.BUF[CAN_TX_LIGHTS_MB].CS.B.CODE = 0xC;
}

void can_send(void){

	/* TO-DO: your task implementations **************************************/
	if (!engine_on){
		return;
	}
	speed = calculated_speed(POT); // read the potentiometer value and calculate the speed
	can_send_engine_on();
	can_send_speed(speed);
	can_send_rpm(rpm);
	can_send_lights();
	LED_Tx = !LED_Tx;
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
	PIT_ConfigureTimer(1, 100);
	PIT_StartTimer(1);
	PIT_ConfigureTimer(2, 1000);
	PIT_StartTimer(2);
	PIT_ConfigureTimer(3, 500);
	PIT_StartTimer(3);
	LED_P = 0;
	LED_U1 = 0;
	LED_U3 = 0;

	/*************************************************************************/


	/* main program */
	while(1){
		/* System function */
		systemFunction();

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
			LED_P = 1;
		}
		else if (crfp.SID == 0x10) // check the Message ID
		{
			engine_on = 0; // set the engine_on flag to indicate that a message with ID 0x10 has been received
			LED_P = 0;
			lights_state = 0;
			LED_U1 = 0;
			LED_U3 = 0;
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
	PIT.CHANNEL[1].TFLG.R = 1U;
	if (engine_on) {
		can_send();
	}
}


/*
 * @brief	PIT timer channel 2 IRQ callback
 *
 * @param	void
 * @retval	void
 */
void PIT_Channel_2(void){
	/* TO-DO: your task implementations **************************************/
	PIT.CHANNEL[2].TFLG.R = 1U;
	if (engine_on) {
		rpm = calculated_rpm();
	}
}


/*
 * @brief	PIT timer channel 3 IRQ callback
 *
 * @param	void
 * @retval	void
 */
void PIT_Channel_3(void){
	/* TO-DO: your task implementations **************************************/
	PIT.CHANNEL[3].TFLG.R = 1U;
	switch (lights_state) {
	case 0x01:
		indicator_phase = !indicator_phase;
		LED_U1 = indicator_phase;
		LED_U3 = 0;
		break;
	case 0x02:
		indicator_phase = !indicator_phase;
		LED_U1 = 0;
		LED_U3 = indicator_phase;
		break;
	case 0x03:
		indicator_phase = !indicator_phase;
		LED_U1 = indicator_phase;
		LED_U3 = indicator_phase;
		break;
	default:
		indicator_phase = 0;
		LED_U1 = 0;
		LED_U3 = 0;
		break;
	}
}
