/*
 *	Project Owner	: TU Chemnitz
 *	Project Name	: ASE Tutorial Unit-2
 *	File Name		: can.h
 *	Author			: ASE Admin
 *  Created on		: 25 Feb 2024
 */

#ifndef CAN_H_
#define CAN_H_

/* Includes ******************************************************************/
#include "can_lld.h"
#include "can_lld_cfg.h"


/* TO-DO: your task implementations ******************************************/

/* Macro definitions *********************************************************/
#define	CAN_MASK_REGISTER			0x7FE // 
#define	CAN_ACCEPTANCE_REGISTER  	0x10 // 
#define CAN_TX_ENGINE_MB	8U
#define CAN_TX_SPEED_MB	9U
#define CAN_TX_RPM_MB	10U
#define CAN_TX_LIGHTS_MB	11U


/*****************************************************************************/


/* Function prototypes *******************************************************/
void CAN_Init(void);
void CAN_TxMessage(CANTxFrame txMsg, canmbx_t mbx);

#endif /* CAN_H_ */
