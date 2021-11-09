/**
 * @file sceduler.h
 * @author Alex Olson (aolson@mail.bradley.edu)
 * @brief handles the sceduler class. This is the main interface with the kernel.
 * @version 0.1
 * @date 2021-05-04
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

/*
########   #######  ########  ########  ######
##     ## ##     ## ##     ##    ##    ##    ##
##     ## ##     ## ##     ##    ##    ##
########  ##     ## ########     ##     ######
##        ##     ## ##   ##      ##          ##
##        ##     ## ##    ##     ##    ##    ##
##         #######  ##     ##    ##     ######
*/

#if defined(__AVR__) || defined(ESP8266)
    //! ATMEL + ESP8266 archetectures are ported here
    // timer 0 overflow interrupt
    #if defined(TIMER0_COMPA_vect)
        #define CONTEXT_SWITCHER_ISR_VECT TIMER0_COMPA_vect
    #else
        #ifndef ARDRTOS_NO_WARNINGS
            #warning timer 0 overflow interrupt not found. cooporative mode only.
            #ifndef ARDRTOS_NO_WARNINGS_DECL
                #warning to dissable future warnings, define ARDRTOS_NO_WARNINGS.
            #endif
            #define ARDRTOS_NO_WARNINGS_DECL
        #endif
        #ifndef COOP_ONLY
            #define COOP_ONLY 1
        #endif // !COOP_ONLY
    #endif

    // timer 0 mask register
    #if defined(TIMSK)
        #define OS_TIMSK TIMSK
    #elif defined(TIMSK0)
        #define OS_TIMSK TIMSK0
    #else
        #ifndef ARDRTOS_NO_WARNINGS
            #warning	Timer 0 compa interrupt not found. cooporative mode only.
            #ifndef ARDRTOS_NO_WARNINGS_DECL
                #warning to dissable future warnings, define ARDRTOS_NO_WARNINGS.
            #endif
            #define ARDRTOS_NO_WARNINGS_DECL
        #endif
        #ifndef COOP_ONLY
            #define COOP_ONLY 1
        #endif // !COOP_ONLY
    #endif

    // timer 0 overflow mask bit
    #if defined(OCIE0A)
        #define OS_TIMSK_ENABLE OCIE0A
    #else
        #ifndef ARDRTOS_NO_WARNINGS
            #warning	timer 0 overflow mask bit not found. cooporative mode only.
            #ifndef ARDRTOS_NO_WARNINGS_DECL
                #warning to dissable future warnings, define ARDRTOS_NO_WARNINGS.
            #endif
            #define ARDRTOS_NO_WARNINGS_DECL
        #endif
        #ifndef COOP_ONLY
            #define COOP_ONLY 1
        #endif // !COOP_ONLY
    #endif

    // timer 0 overflow amount register
    #if defined(OCR0A)
        #define OS_TIMER_COMP_REG OCR0A
    #else
        #ifndef ARDRTOS_NO_WARNINGS
            #warning	timer 0 overflow amount register not found. cooporative mode only.
            #ifndef ARDRTOS_NO_WARNINGS_DECL
                #warning to dissable future warnings, define ARDRTOS_NO_WARNINGS.
            #endif
            #define ARDRTOS_NO_WARNINGS_DECL
        #endif
        #ifndef COOP_ONLY
            #define COOP_ONLY 1
        #endif // !COOP_ONLY
    #endif

    #ifdef COOP_ONLY
        #define ARDRTOS_YIELD\
        CONTEXT_SWITCHER_ISR_VECT()
        // enable, dissable, and setup are skipped over if COOP_ONLY is defined.
    #else
        // a custom yield function for each archetecture
        #define ARDRTOS_YIELD\
            sl = 0;\
            CONTEXT_SWITCHER_ISR_VECT()
        // enables the RTOS context switcher.
        #define ARDRTOS_ENABLE if (!cooporative) bitSet(OS_TIMSK, OS_TIMSK_ENABLE)
        // dissables the RTOS context switcher.
        #define ARDRTOS_DISSABLE if (!cooporative) bitClear(OS_TIMSK, OS_TIMSK_ENABLE)
        // sets up the RTOS timer if needed.
        #define ARDRTOS_SETUP\
            OS_TIMER_COMP_REG = 20;\
            enable()
    #endif /* !COOP_ONLY*/

#elif defined(TEENSYDUINO)
    //! TEENSY archetectures are ported here
    #define COOP_ONLY 1
    #ifndef ARDRTOS_NO_WARNINGS
        #warning currently, only cooporative mode is supported on teensy archetectures.
        #ifndef ARDRTOS_NO_WARNINGS_DECL
            #warning to dissable future warnings, define ARDRTOS_NO_WARNINGS.
        #endif
        #define ARDRTOS_NO_WARNINGS_DECL
    #endif

    #  define ISR(vector, ...)            \
        void vector (void) __attribute__ ((interrupt)) __VA_ARGS__; \
        void vector (void)

    #define CONTEXT_SWITCHER_ISR_VECT ContextSwitcherFuncName
#elif defined(_STM32_DEF_)
    //! STM32 archetectures are ported here
    #define COOP_ONLY 1
    #ifndef ARDRTOS_NO_WARNINGS
        #warning currently, only cooporative mode is supported on stm32 archetectures.
        #ifndef ARDRTOS_NO_WARNINGS_DECL
                        #warning to dissable future warnings, define ARDRTOS_NO_WARNINGS.
        #endif
        #define ARDRTOS_NO_WARNINGS_DECL
    #endif
    #  define ISR(vector, ...)            \
        void vector (void) __attribute__ ((interrupt)) __VA_ARGS__; \
        void vector (void)

    #define CONTEXT_SWITCHER_ISR_VECT ContextSwitcherFuncName
#elif defined(_SAMD_INCLUDED_)
    //! SAM archetectures are ported here

/*
NOTES:
    PendSV_Handler might be usefull for this. it is a dedicated interrupt that, as far as i know,
    has its bit set with systick. the major upside of this is that it has an interrupt priority of min
    so it lets systick and all other interrupts go thorugh.
*/

    #ifndef ARDRTOS_NO_WARNINGS
        #warning currently, preemptive mode is untested.
        #ifndef ARDRTOS_NO_WARNINGS_DECL
            #warning to dissable future warnings, define ARDRTOS_NO_WARNINGS.
        #endif
        #define ARDRTOS_NO_WARNINGS_DECL
    #endif

    #ifdef COOP_ONLY
        #define ARDRTOS_YIELD\
        CONTEXT_SWITCHER_ISR_VECT()
        // enable, dissable, and setup are skipped over if COOP_ONLY is defined.
    #else
        // a custom yield function for each archetecture
        #define ARDRTOS_YIELD\
            sl = 0;\
            CONTEXT_SWITCHER_ISR_VECT()
        // enables the RTOS context switcher.
        #define ARDRTOS_ENABLE if (!cooporative) bitSet(OS_TIMSK, OS_TIMSK_ENABLE)
        // dissables the RTOS context switcher.
        #define ARDRTOS_DISSABLE if (!cooporative) bitClear(OS_TIMSK, OS_TIMSK_ENABLE)
        // sets up the RTOS timer if needed.
        #define ARDRTOS_SETUP\
            OS_TIMER_COMP_REG = 20;\
            enable()
    #endif /* !COOP_ONLY*/

    #  define ISR(vector, ...)            \
        void vector (void) __attribute__ ((interrupt)) __VA_ARGS__; \
        void vector (void)


    #define CONTEXT_SWITCHER_ISR_VECT PendSV_Handler
#else
    //! unknown archetectures are tenatively ported here
    // the reason why coop only is needed is because preemptive mode requres a hardware timer.
    // since there is no standard way of setting up a timer across EVERY microcontroller in existance, we
    // are limited to calling the ISR ourselves cooporatively
    #define COOP_ONLY 1
    #warning unknown archetecture detected. ArdRTOS is limited to coop only or may not work at all.
    #ifndef ARDRTOS_NO_WARNINGS_DECL
        #warning to dissable future warnings, define ARDRTOS_NO_WARNINGS.
    #endif
    #define ARDRTOS_NO_WARNINGS_DECL

    // if ISR is not defined by the current archetecture, we can define it here. this allows the gcc compiler
    // to generate this function as though it is an interrupt.
    #ifndef ISR
        #  define ISR(vector, ...)            \
            void vector (void) __attribute__ ((interrupt)) __VA_ARGS__; \
            void vector (void)
    #endif

    #define CONTEXT_SWITCHER_ISR_VECT ContextSwitcherFuncName
#endif

/*
 ######   ######  ##     ## ######## ########  ##     ## ##       ######## ########
##    ## ##    ## ##     ## ##       ##     ## ##     ## ##       ##       ##     ##
##       ##       ##     ## ##       ##     ## ##     ## ##       ##       ##     ##
 ######  ##       ######### ######   ##     ## ##     ## ##       ######   ########
      ## ##       ##     ## ##       ##     ## ##     ## ##       ##       ##   ##
##    ## ##    ## ##     ## ##       ##     ## ##     ## ##       ##       ##    ##
 ######   ######  ##     ## ######## ########   #######  ######## ######## ##     ##
*/

/**
 * @brief This is the main interface with the kernel that most people will interact with. nothing too fancy.
 * 
 */
class Scheduler {
public:
    /**
     * @brief Construct a new Sceduler object. There really should only be one, but there can be more for no reason.
     */
    Scheduler();

	/**
     * @brief Create a task to be ran.
     * 
     * @param loop the loop function to use
     * @param stackSize how much memory you are going to use for this task
	 * @param priority how many time slots you want this task to take up
     */
    void addTask(osFuncCall loop, unsigned stackSize=0x40, unsigned char priority=1);
		/**
     * @brief Create a task to be ran with the given argument. 
     * 
     * @param loop the loop function to use
	 * @param arg a pointer to the argument to feed the function
     * @param stackSize how much memory you are going to use for this task
	 * @param priority how many time slots you want this task to take up
     */
    void addTask(osFuncCallArg loop, void *arg, unsigned stackSize=0x40, unsigned char priority=1);

    /**
     * @brief begins ArdRTOS after tasks are asigned in preemptive mode
     */
    void begin();

	/**
     * @brief simple alias for begin() if you want to be explicit in your usage of preemptive mode
     */
    void beginPreemptive();

	/**
     * @brief begins ArdRTOS after tasks are assigned in cooporative mode. 
     * the best way to turn ArdRTOS into a cooporative OS would be to define COOP_ONLY prior to inclusion.
     * this would compile the library completely differently as to save data and time.
     */
    void beginCooporative();

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
	void delay(unsigned long ms);

	/**
	 * @brief untill the time spesified approaches, it calls the context switcher.
	 * 
	 * @param ms the time to wait till
	 */
	void delayUntill(unsigned long ms);

		/**
	 * @brief fetches the taskID of the currently running task
	 * 
	 * @return uint8_t 
	 */
	TaskID getTaskID();

#ifndef COOP_ONLY
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

	/**
	 * @brief Set the Priority for the currently running task
	 * 
	 * @param char how many timeslots you want this task to take up
	 */
	void setPriority(unsigned char);

	/**
	 * @brief Set the Priority for another task
	 * 
	 * @param TaskID the task to set priority of
	 * @param char the priority you want to set that task to
	 */
	void setPriority(TaskID taskID, unsigned char priority);

	friend class Semaphore;
	friend class Mutex;
private:
	/**
	 * @brief gives your priority to another task
	 * 
	 * @param taskID the task you wish to donate too
	 */
	void donatePriority(TaskID taskID);
	
	/**
	 * @brief resets the priority of the currently running task
	 * 
	 */
	void resetPriority();
#endif // !COOP_ONLY
};

#endif /* SCHEDULER_H_ */