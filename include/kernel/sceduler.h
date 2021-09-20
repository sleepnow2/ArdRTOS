/**
 * @file sceduler.h
 * @author Alex Olson (aolson@mail.bradley.edu)
 * @brief handles the sceduler class. This is the main interface with the kernel.
 * @version 0.1
 * @date 2021-05-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef SCEDULER_H_
#define SCEDULER_H_

//! OS CONTEXT MACROS BEGIN

/*
 .d88888b.   .d8888b.          .d8888b.                              .d8888b.                    888                     888
d88P" "Y88b d88P  Y88b        d88P  Y88b                            d88P  Y88b                   888                     888
888     888 Y88b.             Y88b.                                 888    888                   888                     888
888     888  "Y888b.           "Y888b.    8888b.  888  888  .d88b.  888         .d88b.  88888b.  888888 .d88b.  888  888 888888
888     888     "Y88b.            "Y88b.     "88b 888  888 d8P  Y8b 888        d88""88b 888 "88b 888   d8P  Y8b `Y8bd8P' 888
888     888       "888              "888 .d888888 Y88  88P 88888888 888    888 888  888 888  888 888   88888888   X88K   888
Y88b. .d88P Y88b  d88P        Y88b  d88P 888  888  Y8bd8P  Y8b.     Y88b  d88P Y88..88P 888  888 Y88b. Y8b.     .d8""8b. Y88b.
 "Y88888P"   "Y8888P" 88888888 "Y8888P"  "Y888888   Y88P    "Y8888   "Y8888P"   "Y88P"  888  888  "Y888 "Y8888  888  888  "Y888
*/

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
#define __OS_SaveContext(sp_address) \
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
		" IN	r0, __SREG__	\n\t"\
		" PUSH	r0				\n\t"\
		" WDR 					\n\t"\
	);\
	asm volatile(\
        /* save the current flag register to stack */\
		"lds   r26, [" #sp_address "]			 \n\t"\
		"lds   r27, [" #sp_address "]+1			 \n\t"\
		"in    r0, __SP_L__          \n\t"\
		"st    x+, r0                \n\t"\
		"in    r0, __SP_H__          \n\t"\
		"st    x+, r0                \n\t"\
		:\
		: [sp_address] "m" (sp_address)\
		: "r0", "r26", "r27"\
	)
	/* write stack pointer to a uint16_t var given in the macro parameter */

/*
 .d88888b.   .d8888b.          888                            888  .d8888b.                    888                     888
d88P" "Y88b d88P  Y88b         888                            888 d88P  Y88b                   888                     888
888     888 Y88b.              888                            888 888    888                   888                     888
888     888  "Y888b.           888      .d88b.   8888b.   .d88888 888         .d88b.  88888b.  888888 .d88b.  888  888 888888
888     888     "Y88b.         888     d88""88b     "88b d88" 888 888        d88""88b 888 "88b 888   d8P  Y8b `Y8bd8P' 888
888     888       "888         888     888  888 .d888888 888  888 888    888 888  888 888  888 888   88888888   X88K   888
Y88b. .d88P Y88b  d88P         888     Y88..88P 888  888 Y88b 888 Y88b  d88P Y88..88P 888  888 Y88b. Y8b.     .d8""8b. Y88b.
 "Y88888P"   "Y8888P" 88888888 88888888 "Y88P"  "Y888888  "Y88888  "Y8888P"   "Y88P"  888  888  "Y888 "Y8888  888  888  "Y888
*/

/**
 * @brief loads the context off of the stack location pointed to by curr_sp. 
 * 
 * @param curr_sp the stack pointer that currently contains the address of the stack that is to be restored.
 */
#define __OS_LoadContext(sp_address) \
    asm volatile(\
        /* load the special flags from off of the stack */\
		" lds   r26, [" #sp_address "]			 \n\t"\
		" lds   r27, [" #sp_address "]+1			 \n\t"\
		" ld   r0, x+                \n\t"\
		" out  __SP_L__, r0          \n\t"\
		" ld   r0, x+                \n\t"\
		" out  __SP_H__, r0          \n\t"\
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
		:\
		: [sp_address] "m" (sp_address)\
	)

/*
 .d88888b.   .d8888b.          .d8888b.  8888888888 88888888888 .d8888b.  8888888b.
d88P" "Y88b d88P  Y88b        d88P  Y88b 888            888    d88P  Y88b 888   Y88b
888     888 Y88b.             Y88b.      888            888    Y88b.      888    888
888     888  "Y888b.           "Y888b.   8888888        888     "Y888b.   888   d88P
888     888     "Y88b.            "Y88b. 888            888        "Y88b. 8888888P"
888     888       "888              "888 888            888          "888 888
Y88b. .d88P Y88b  d88P        Y88b  d88P 888            888    Y88b  d88P 888
 "Y88888P"   "Y8888P" 88888888 "Y8888P"  8888888888     888     "Y8888P"  888
*/

#define __OS_SetSP(sp_address)\
	asm volatile(\
		"lds  r26, [" #sp_address "]	\n\t"\
		"lds  r27, [" #sp_address "]+1	\n\t"\
		"ld   r0, x+					\n\t"\
		"out  __SP_L__, r0				\n\t"\
		"ld   r0, x+					\n\t"\
		"out  __SP_H__, r0				\n\t"\
		:\
		: [sp_address] "m" (sp_address)\
		: "r0", "r26", "r27"\
	)

//! OS CONTEXT MACROS END

/**
 * @brief This is the main interface with the kernel that most people will interact with. nothing too fancy.
 * 
 */
class Sceduler {
public:
    /**
     * @brief Construct a new Sceduler object. There really should only be one, but there can be more for no reason.
     */
    Sceduler();

    /**
     * @brief Create a task to be run, prepare the memory, and 
     * 
     * @param task the task
     * @param stackSize how much memory you are going to use for this task
     */
    void addTask(Task &task, uint16_t stackSize);
	/**
     * @brief Create a task to be run, prepare the memory, and 
     * 
     * @param loop the loop function to use
     * @param stackSize how much memory you are going to use for this task
     */
    void addTask(osFuncCall &loop, uint16_t stackSize);
	/**
     * @brief Create a task to be run, prepare the memory, and 
     * 
     * @param setup the setup function to call before doing loop
	 * @param loop the loop function to use
     * @param stackSize how much memory you are going to use for this task
     */
    void addTask(osFuncCall &setup, osFuncCall &loop, uint16_t stackSize);

    /**
     * @brief begins ArdRTOS after tasks asigned.
     */
    void begin();

	/**
	 * @brief calls the context switcher to move onto the next task
	 * 
	 */
	void yield();

	/**
	 * @brief untill the amount time spesified passes, it calls the context switcher.
	 * 
	 * @param ms how long to wait
	 */
	void delay(uint32_t ms);

	/**
	 * @brief untill the time spesified approaches, it calls the context switcher.
	 * 
	 * @param ms the time to wait till
	 */
	void delay_untill(uint32_t ms);

	/**
	 * @brief enables the sceduler ISR
	 * 
	 */
	void enable();

	/**
	 * @brief dissables the sceduler ISR without dissabling the rest of interrupts as a whole.
	 * 
	 */
	void dissable();
};

#endif /* SCEDULER_H_ */