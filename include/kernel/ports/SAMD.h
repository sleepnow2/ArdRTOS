#ifndef __ARDRTOS_KERNEL_PORTS_SAMD__
#define __ARDRTOS_KERNEL_PORTS_SAMD__

/*
NOTES:
    PendSV_Handler might be usefull for this. it is a dedicated interrupt that, as far as i know,
    has its bit set with systick. the major upside of this is that it has an interrupt priority of min
    so it lets systick and all other interrupts go thorugh.
*/

#define COOP_ONLY 1
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

#define ISR(vector, ...)            \
        extern "C" void vector (void) __attribute__ ((interrupt)) __VA_ARGS__; \
        void vector (void)


#define CONTEXT_SWITCHER_ISR_VECT PendSV_Handler

#endif // !__ARDRTOS_KERNEL_PORTS_SAMD__