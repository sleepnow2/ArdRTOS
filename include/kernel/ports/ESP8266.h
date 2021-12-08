#ifndef __ARDRTOS_KERNEL_PORTS_ESP8266__
#define __ARDRTOS_KERNEL_PORTS_ESP8266__

#define COOP_ONLY 1
#ifndef ARDRTOS_NO_WARNINGS
    #warning currently, preemptive mode is untested.
    #ifndef ARDRTOS_NO_WARNINGS_DECL
        #warning to dissable future warnings, define ARDRTOS_NO_WARNINGS.
    #endif
    #define ARDRTOS_NO_WARNINGS_DECL
#endif

#define CSWITCH(vector)\
    void vector (void) __attribute__((optimize("O0"))); \
    void vector (void)

#define CONTEXT_SWITCHER_ISR_VECT ContextSwitcherFuncName

#define ARDRTOS_YIELD CONTEXT_SWITCHER_ISR_VECT()

#define ARDRTOS_SETUP

#endif //!__ARDRTOS_KERNEL_PORTS_ESP8266__