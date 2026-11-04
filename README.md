# __ArdRTOS__
___
## __What is ArdRTOS__
ArdRTOS is a simple cooperative real time operating system to facilitate rapid development without requiring extensive knowledge of real time operating systems.

## __Why make ArdRTOS__
While taking a class in college about freeRTOS, I realized that there were some simplifications and modernizations that could be taken to make using an RTOS dramatically easier and straight forward to use. While ArdRTOS may not be as flexible or mature, ArdRTOS prioritizes ease of use above all other design elements.

Primarily, this RTOS is for people who don't want to learn how to actually USE an RTOS. Some concessions are made for ease of use that are sub-optimal for professional usage, but it means that I can obscure as much as possible behind a simple to use library.

## __How do I use it?__
The folder named "examples" is full of examples to use as a short tutorial to using ArdRTOS. For users using the Arduino IDE, those examples are found in the standard location for all library examples.
___
## __Feature Road Map__
- [X] (alpha) get the kernel working
    - [X] cooperative OS to work
    - [X] preemptive OS to work (un-canceled)
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
- [ ] (2.1) Scheduler Changes
    - [ ] (2.1) Delay calculations in scheduler space instead of task space.
        - [ ] implement weak functions for people who use ISR's for preemption. 
            - inline PRE_SWITCH and POST_SWITCH for enabling and dissabling ISRs.
        - [ ] implement a number of implementations for preemption based on boards I own.
            - [ ] Atmel 328
            - [ ] PIC18F22K83
            - [ ] ARM Cortex M4
            - [ ] ESP8266
            - [ ] ESP32
    - [ ] (2.1) Investigate the use of linked lists to store tasks.
        - currently limited to 8 tasks unless you modify the .h file.
    - [ ] (2.1) Investigate a priority system for high priority tasks.
        - priority could go to tasks that are added first? 
            - This would be smaller and easier, but not as intuitive.
        - priority could be stored in a separate variable? 
            - This would be more flexible for future implementations.

        - would priority be blocking or timeshare?
            - blocking is more deterministic, but timeshare is more forgiving. 
            - blocking is harder to implement, but timeshare could just be yield x times for your priority.
- [ ] Extra features
    - [ ] Thread pooling
        - give a class that behaves like the scheduler for multiple functions to share a stack easily.
        - [ ] Rework the mutexes to be able to give each threadpool thread a seporate ID for mutexes and signals.

### __How can I contribute?__
You are free to open a ticket to contribute to this project in all of the usual github ways, as long as you do not use AI in the process of creating any changes. The only vibing allowed here is the celebrations after all the test cases pass.

You can ask for my PlatformIO folder at any time for test cases and 