#ifndef __ARDRTOS_KERNEL_PORTS_INIT__
#define __ARDRTOS_KERNEL_PORTS_INIT__

#if defined(__AVR__)
    #include "kernel/ports/Atmega.h"
#elif  defined(ESP8266)
    #include "kernel/ports/ESP8266.h"
#elif defined(TEENSYDUINO)
    #include "kernel/ports/Teensy.h"
#elif defined(_STM32_DEF_)
    #include "kernel/ports/STM32.h"
#elif defined(_SAMD_INCLUDED_)
    #include "kernel/ports/SAMD.h"
#else
    #include "kernel/ports/unknown.h"
#endif

#endif //!__ARDRTOS_KERNEL_PORTS_INIT__