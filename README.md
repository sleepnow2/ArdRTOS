# __ArdRTOS__
## __table of contents__
1. [Introduction](#Introduction)
    1. [Who are you and what have you done with my RTOS!?!](#Who-are-you-and-what-have-you-done-with-my-RTOS!?!)
    1. [Why ArdRTOS](#Why-ArdRTOS?)
    1. [what makes this different from FreeRTOS?](#What-makes-this-different-from-FreeRTOS?)
1. [How do I use it?](#How-do-I-use-it?)
1. [what is in the future?](#what-is-in-the-future?)
___
## __Introduction__
### __Who are you and what have you done with my RTOS!?!__
    My name is Alex Olson, and don't worry... your pressious little RTOS still exists. I just made it... prettier.
### __Why ArdRTOS?__
    while taking a class in college about freeRTOS, I realized that there were some simplifications that I could make to the API in accessing freeRTOS. Thus, I decided to make this! hopefully, this turns out to be a simpler, less bloated, more intuitive RTOS for use by both proffessionals and ametures alike.
### __What makes this different from FreeRTOS?__
    This is different in that this RTOS takes away some features of FreeRTOS in a strategic way in order to reduce the number of mistakes possible while keeping things intuitive.
    tasks no longer need handles. queues are classes, semaphores are objects, and the sceduler is a class that is handled in a similar and familiar way as to using Serial or Wire.
____
## __How do I use it?__
    TODO: insert a link to example projects here
___
## __what is in the future?__
- [ ] (alpha) get the kernel working
    - The kernel is the heart and soul of the RTOS. whether you use a cooporative or preemptive RTOS, you need a kernel to switch contexts from one task to another. this is obvious.
    - [ ] get cooporative OS to work by relinquishing controll of tasks.
    - [ ] get preemptive OS to work by using
- [ ] (beta) get intra task communication and syncronization
    - no matter what RTOS you use, there is always some kind of 
    - [ ] queues
    - [ ] semaphores
- [ ] (1.0) generate documentation and examples
    - [ ] add an array of features like stack overflow detection (not the __good__ type of stack overflow, the __bad__ type of stack overflow.)
        - [ ] manual
        - [ ] automatic (include into kernel)
    - [ ] dyoxygen documentation for entire project
    - [ ] examples for each and every aspect of the RTOS 
         - can repeat the same example for multiple items if it clearly demonstrates and annotates the use of that function
- [ ] (1.0 +) get intra processor communication and syncronization
    - you do not see this supported natively in many RTOS. However, if we include this into our RTOS natively, we can open up the idea of cluster compute with cheap, modular components while keeping our API very similar to the API for intra task communication. Naturally this will not be as fast as intra task communication, but it will be a very interresting feature that no other RTOS offers.
    - [ ] queues
        - [ ] single consumer queue - this queue structure is so that when someone posts an item onto the queue, each item is non fungable and can only be taken by one process.
        - [ ] multi consumer queue - this queue structure is so that when someone posts an item onto the queue, each item is fungable and everyone gets a copy to work with it if they are listening.
    - [ ] semaphores

    And like always... we are squashing bugs and taking suggestions at every turn
### __How can I contribute?__
    currently, this is under private development as a senior project out of Bradley University by a lowly ECE major student. so untill the project is done, no contributions by members outside of the senior project for obvious reasons.

    once this senior project is done, I will release this for open source contribution to go the way of Linux, FreeRTOS, Python, FireFox, and Blender.
- for those who have a burning desire to help
    - you can contribute by submitting code, examples, comments, and helping people understand this peice of middleware which you now use for whatever reason that you use it.
    - you can directly contribute by making port files for different microcontrollers so that we can spread our support for a vast array of microcontrollers for everyone and any project.
- for anyone regardless of your stature
    - you can contribute by submitting bug reports, questions, and by making suggestions that might become the future of the entire project.
    - you can spread the love like a dirty, dirty virus and infect your friends with this simple, easy to use RTOS that is just so blazingly easy to use.