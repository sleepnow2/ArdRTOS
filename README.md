# __ArdRTOS__
___
## __What is ArdRTOS__
ArdRTOS is a simple cooperative real time operating system to facilitate rapid development without requiring extensive knowledge of real time operating systems.

## __Why make ArdRTOS__
While taking a class in college about freeRTOS, I realized that there were some simplifications and modernizations that could be taken to make using an RTOS dramatically easier and straight forward to use. While ArdRTOS may not be as flexible or mature, ArdRTOS prioritizes ease of use above all other design elements.

## __How do I use it?__
The folder named "examples" is full of examples to use as a short tutorial to using ArdRTOS. For users using the Arduino IDE, those examples are found in the standard location for all library examples.
___
## __Feature Road Map__
- [X] (alpha) get the kernel working
    - [X] cooperative OS to work
    - [X] preemptive OS to work (CANCELED)
- [X] (beta) get intra task communication and synchronization
    - [X] queues
    - [X] stacks
    - [X] semaphores/Mutexes
- [X] (1.0) generate documentation and examples
    - [X] dyoxygen documentation for entire project
    - [X] examples for every major aspect of the RTOS
        - [X] setting up tasks
        - [X] giving arguments to tasks
        - [X] Delay and yield
        - [X] integrating interrupts
        - [X] Stacks
        - [X] Queues
        - [X] Semaphores
- [ ] ESP support
    - [ ] ESP8266 support
    - [ ] ESP32 support

### __How can I contribute?__
Currently, this is under private development as a senior project out of Bradley University by an ECE student. Until the senior project is complete, no contributions by members outside of the senior project will be accepted... for obvious reasons.