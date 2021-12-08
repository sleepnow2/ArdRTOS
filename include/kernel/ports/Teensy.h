#ifndef __ARDRTOS_KERNEL_PORTS_TEENSY__
#define __ARDRTOS_KERNEL_PORTS_TEENSY__

#define COOP_ONLY 1
    #ifndef ARDRTOS_NO_WARNINGS
        #warning currently, only cooporative mode is supported on teensy archetectures.
        #ifndef ARDRTOS_NO_WARNINGS_DECL
            #warning to dissable future warnings, define ARDRTOS_NO_WARNINGS.
        #endif
        #define ARDRTOS_NO_WARNINGS_DECL
    #endif

    #define ISR(vector, ...)            \
        extern "C" void vector (void) __attribute__ ((interrupt)) __VA_ARGS__; \
        void vector (void)

    #define CONTEXT_SWITCHER_ISR_VECT ContextSwitcherFuncName

#endif // !__ARDRTOS_KERNEL_PORTS_TEENSY__