#ifndef __ARDRTOS_KERNEL_PORTS_UNKNOWN__
#define __ARDRTOS_KERNEL_PORTS_UNKNOWN__

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
    #define ISR(vector, ...)            \
        extern "C" void vector (void) __attribute__ ((interrupt)) __VA_ARGS__; \
        void vector (void)
#endif

#define CONTEXT_SWITCHER_ISR_VECT ContextSwitcherFuncName

#endif // !__ARDRTOS_KERNEL_PORTS_UNKNOWN__