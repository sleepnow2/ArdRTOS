#ifndef __ARDRTOS_KERNEL_PORTS_ATMEGA__
#define __ARDRTOS_KERNEL_PORTS_ATMEGA__

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
    #define CONTEXT_SWITCHER_ISR_VECT TIMER0_COMPA_vect
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

#define CSWITCH(v) ISR(v, NOOP)

#ifdef NO_PRIORITIES
    #define ARDRTOS_YIELD\
    CONTEXT_SWITCHER_ISR_VECT()
    // enable, dissable, and setup are skipped over if COOP_ONLY is defined.
    #define ARDRTOS_SETUP 
#else
    // a custom yield function for each archetecture
    #define ARDRTOS_YIELD\
        sl = 0;\
        CONTEXT_SWITCHER_ISR_VECT()

    // sets up the RTOS timer if needed.
    #define ARDRTOS_SETUP\
        OS_TIMER_COMP_REG = 20;\
        OS_TIMSK |= 1<<OS_TIMSK_ENABLE;
#endif /* !COOP_ONLY*/

#endif // !__ARDRTOS_KERNEL_PORTS_ATMEGA__