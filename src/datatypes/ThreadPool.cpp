/**
 * @file ThreadPool.cpp
 * @author Alex Olson (sleepnow2@gmail.com)
 * @brief provides a basic form of sharing the stack between multiple tasks.
 * @version 0.2
 * @date 2026-11-04
 * 
 * @copyright MIT Copyright (c) 2026 Alex Olson. All rights reserved. details at bottom of file.
 * 
 */

#include "../ArdRTOS.h"

/*
        ████████  █████  ███████ ██   ██         ██████   ██████   ██████  ██
           ██    ██   ██ ██      ██  ██          ██   ██ ██    ██ ██    ██ ██
           ██    ███████ ███████ █████           ██████  ██    ██ ██    ██ ██
           ██    ██   ██      ██ ██  ██          ██      ██    ██ ██    ██ ██
███████    ██    ██   ██ ███████ ██   ██ ███████ ██       ██████   ██████  ███████


*/

_TASK_POOL::_TASK_POOL():
    task(nullptr), 
    argument(nullptr),
    delay(0),
    running(true), 
    one_shot(false) 
{
    continuous();
    enable();
};

bool _TASK_POOL::isEmpty() {
    return task==nullptr;
}

void _TASK_POOL::setTask(osFuncCall task) {
    this->task = task;
}
void _TASK_POOL::setTask(osFuncCallArg task, void* arg) {
    this->task = (osFuncCall)task;
    this->argument = arg;
}

/// @brief enables the task attached to this handle
void _TASK_POOL::enable() {running = true;}
/// @brief disables the task attached to this handle
void _TASK_POOL::disable() {running = false;}

/// @brief puts the task into one shot mode, meaning it is run once per time it is enabled.
void _TASK_POOL::oneShot() {one_shot = true;}
/// @brief puts the task into continuous mode, meaning it will never stop! (until told to)
void _TASK_POOL::continuous() {one_shot = false;}

/// @brief checks the handle to see if it is time to run this function. if so, run it.
void _TASK_POOL::run() {
    // are we supposed to be running?
    if (!running) return;

    // cache the current millisecond time.
    auto m = millis();
    
    // if 
    if (m - last >= delay) {
        // step the counter forward until we aren't scheduled to IMMEDIATELY fire again...
        while (m - last >= delay) last += delay;

        // if this task is supposed to be a one shot task, then disable it.
        if (one_shot) disable();

        if (argument == nullptr) {
            task();
        } else {
            (*(osFuncCallArg*)task)(argument);
        }
        OS.yield();
    }
}

/*
████████ ██   ██ ██████  ███████  █████  ██████  ██████   ██████   ██████  ██
   ██    ██   ██ ██   ██ ██      ██   ██ ██   ██ ██   ██ ██    ██ ██    ██ ██
   ██    ███████ ██████  █████   ███████ ██   ██ ██████  ██    ██ ██    ██ ██
   ██    ██   ██ ██   ██ ██      ██   ██ ██   ██ ██      ██    ██ ██    ██ ██
   ██    ██   ██ ██   ██ ███████ ██   ██ ██████  ██       ██████   ██████  ███████


*/

_TASK_POOL* ThreadPool::getTaskPoolHandle() {
    return tasks+current_task;
}

ThreadPool::ThreadPool(uint8_t task_num): current_task(0), task_num(task_num) {
    tasks = (_TASK_POOL*)calloc(task_num, sizeof(_TASK_POOL));
}
ThreadPool::~ThreadPool(){
    free(tasks);
}

bool ThreadPool::addTask(osFuncCallArg new_task, void* arg, unsigned long delay) {
    if (current_task >= task_num) return false;
    tasks[current_task].setTask(new_task, arg);
    tasks[current_task].delay = delay;
    tasks[current_task].enable();
    current_task += 1;
    return true;
}

bool ThreadPool::addTask(osFuncCall new_task, unsigned long delay) {
    return addTask((osFuncCallArg) new_task, nullptr, delay);
}

void ThreadPool::runTasks(void* _self) {
    ThreadPool *self = (ThreadPool*)_self;
    
    for (self->current_task = 0; self->current_task < self->task_num; self->current_task++) {
        // if it is empty (we didn't fill it) then skip.
        if (self->tasks[self->current_task].isEmpty()) {continue;}
        
        // run the task in this pool.
        self->tasks[self->current_task].run();
    }
}

TaskID ThreadPool::getTaskID() {
    return tasks+current_task;
}

/**
 * MIT License
 * 
 * Copyright (c) 2022 Alex Olson
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */