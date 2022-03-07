#ifndef __ARDRTOS_KERNEL_PORTS_STM32__
#define __ARDRTOS_KERNEL_PORTS_STM32__

//#define COOP_ONLY 1
#ifndef ARDRTOS_NO_WARNINGS
    #warning currently, preemptive mode is untested.
    #ifndef ARDRTOS_NO_WARNINGS_DECL
        #warning to dissable future warnings, define ARDRTOS_NO_WARNINGS.
    #endif
    #define ARDRTOS_NO_WARNINGS_DECL
#endif

#define ISR(vector, ...)            \
        void vector (void) __attribute__ ((interrupt("interrupt"))) __VA_ARGS__; \
        void vector (void)

//SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;

#define CSWITCH(v) ISR(v, NOOP) 

// this is not preferable. I desire for 
#define CONTEXT_SWITCHER_ISR_VECT PendSV_Handler

#define ARDRTOS_YIELD SCB->SHCSR |= SCB_SHCSR_PENDSVACT_Msk;
//#define ARDRTOS_YIELD CONTEXT_SWITCHER_ISR_VECT();

// sets up pendsv
#define ARDRTOS_SETUP \
    //__NVIC_SetVector(PendSV_IRQn, (uint32_t)&CONTEXT_SWITCHER_ISR_VECT);

#define ARDRTOS_INTERUPTABLE

#endif // !__ARDRTOS_KERNEL_PORTS_STM32__