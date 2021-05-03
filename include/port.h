#ifndef __ARDRTOS_PORT_H__
#define __ARDRTOS_PORT_H__

#include <Arduino.h>

#define ARDRTOS_REG_COUNT 32
#define ARDRTOS_FUNC_CALL_STACK_SIZE 2

//! OS TIMER ISR DEFINITIONS BEGIN
#define SCEDULER_MASK_REG 		    TIMSK0
#define SCEDULER_OVF_REG            OCR1A

#define SCEDULER_VECT               TIMER1_COMPA_vect

#define SCEDULER_MASK_BIT	        OCIE0A

#define SCEDULER_MASK_SET()         bitSet(SCEDULER_MASK_REG, SCEDULER_MASK_BIT)
#define SCEDULER_MASK_CLEAR()       bitClear(SCEDULER_MASK_REG, SCEDULER_MASK_BIT)
#define SCEDULER_SET_OVF()          SCEDULER_OVF_REG = 0xFF
//! OS TIMER ISR DEFINITIONS END

//! OS CONTEXT MACROS BEGIN
#pragma region SAVE_CONTEXT
/**
 * @brief saves the current context that the program was running in.
 * the current state of the program counter does not matter as when this is fired
 * from the ISR, the program counter is pushed onto the stack to begin with, and 
 * when this is fired from the yield statement above, the asm("CALL") statement pushes the current
 * program counter + 2 onto the stack, so that when we restore context later, it restores to just after OS_Yield
 * gets called.
 * 
 * @param curr_sp the uint16_t variable that will hold the stack pointer after these opperations are done
 */
#define OS_SaveContext(curr_sp) \
    asm volatile(\
		" PUSH	r0  \n\t"\
		" push  r1  \n\t"\
		" PUSH	r2  \n\t"\
		" PUSH	r3  \n\t"\
		" PUSH	r4  \n\t"\
		" PUSH	r5  \n\t"\
		" PUSH	r6  \n\t"\
		" PUSH	r7  \n\t"\
		" PUSH	r8  \n\t"\
		" PUSH	r9  \n\t"\
		" PUSH	r10 \n\t"\
		" PUSH	r11 \n\t"\
		" PUSH	r12 \n\t"\
		" PUSH	r13 \n\t"\
		" PUSH	r14 \n\t"\
		" PUSH	r15 \n\t"\
		" PUSH	r16 \n\t"\
		" PUSH	r17 \n\t"\
		" PUSH	r18 \n\t"\
		" PUSH	r19 \n\t"\
		" PUSH	r20 \n\t"\
		" PUSH	r21 \n\t"\
		" PUSH	r22 \n\t"\
		" PUSH	r23 \n\t"\
		" PUSH	r24 \n\t"\
		" PUSH	r25 \n\t"\
		" PUSH	r26 \n\t"\
		" PUSH	r27 \n\t"\
		" PUSH	r28 \n\t"\
		" PUSH	r29 \n\t"\
		" PUSH	r30 \n\t"\
		" PUSH	r31 \n\t"\
        /* save the current flag register to stack */\
		" IN	r0, __SREG__	\n\t"\
		" PUSH	r0				\n\t"\
		);\
		/* write stack pointer to a uint16_t var given in the macro parameter */\
		curr_sp=SP
#pragma endregion
#pragma region LOAD_CONTEXT

/**
 * @brief loads the context off of the stack location pointed to by curr_sp. 
 * 
 * @param curr_sp the stack pointer that currently contains the address of the stack that is to be restored.
 */
#define OS_LoadContext(curr_sp) \
	SP = curr_sp;\
    asm volatile(\
        /* load the special flags from off of the stack */\
		" POP	r0"			"\n\t"\
		" OUT	__SREG__, r0"	"\n\t"\
        /* load the rest of the registers from the stack */\
		" POP	r31 \n\t"\
		" POP	r30 \n\t"\
		" POP	r29 \n\t"\
		" POP	r28 \n\t"\
		" POP	r27 \n\t"\
		" POP	r26 \n\t"\
		" POP	r25 \n\t"\
		" POP	r24 \n\t"\
		" POP	r23 \n\t"\
		" POP	r22 \n\t"\
		" POP	r21 \n\t"\
		" POP	r20 \n\t"\
		" POP	r19 \n\t"\
		" POP	r18 \n\t"\
		" POP	r17 \n\t"\
		" POP	r16 \n\t"\
		" POP	r15 \n\t"\
		" POP	r14 \n\t"\
		" POP	r13 \n\t"\
		" POP	r12 \n\t"\
		" POP	r11 \n\t"\
		" POP	r10 \n\t"\
		" POP	r9  \n\t"\
		" POP	r8  \n\t"\
		" POP	r7  \n\t"\
		" POP	r6  \n\t"\
		" POP	r5  \n\t"\
		" POP	r4  \n\t"\
		" POP	r3  \n\t"\
		" POP	r2  \n\t"\
		" POP	r1  \n\t"\
		" POP	r0  \n\t"\
		);
#pragma endregion
//! OS CONTEXT MACROS END

#endif