/**
 * @file ThreadPool.h
 * @author Alex Olson (sleepnow2@gmail.com)
 * @brief provides a basic form of sharing the stack between multiple tasks.
 * @version 0.2
 * @date 2026-09-24
 * 
 * @copyright MIT Copyright (c) 2026 Alex Olson. All rights reserved. details at bottom of file.
 * 
 */

#ifndef __THREAD_POOL_STACK_H__
#define __THREAD_POOL_STACK_H__

#include "../ArdRTOS.h"

class _TASK_POOL
{
protected:

    // the task to perform
    osFuncCall task;
    // the argument to give it
    void* argument;
    // when this task last run
    unsigned long last;
public:
    // how often this "task" is supposed to run
    unsigned long delay;
protected:
    // whether this task is supposed to be running
    bool running : 1;
    // whether this task is only supposed to be run once (maybe a digester of data)
    bool one_shot : 1;
public:
    _TASK_POOL();

    /// @brief checks to see whether this task in the thread pool has been set yet.
    /// @return 
    bool isEmpty();

    void setTask(osFuncCall task);
    void setTask(osFuncCallArg task, void* arg);

    /// @brief enables the task attached to this handle
    void enable();
    /// @brief disables the task attached to this handle
    void disable();

    /// @brief puts the task into one shot mode, meaning it is run once per time it is enabled.
    void oneShot();
    /// @brief puts the task into continuous mode, meaning it will never stop! (until told to)
    void continuous();

    /// @brief checks the handle to see if it is time to run this function. if so, run it.
    void run();
};

/// @brief 
class ThreadPool {
protected:
    _TASK_POOL *tasks;
    uint8_t task_num;
    uint8_t current_task;

    /// @brief returns the current task pool handle for manipulation.
    /// @return the current task pool handle for manipulation internally.
    _TASK_POOL* getTaskPoolHandle();

public:
    ThreadPool(uint8_t task_num);
    ~ThreadPool();

    /// @brief adds a new task into this threadpool 
    /// @param new_task the new task to add
    /// @param arg the argument that is supposed to be handed to the new task
    /// @param delay how long tasks should delay for between execution
    /// @return whether the task was added successfully or not
    bool addTask(osFuncCallArg new_task, void* arg, unsigned long delay);

    /// @brief adds a new task into this threadpool 
    /// @param new_task the new task to add
    /// @param delay how long tasks should delay for between execution
    /// @return whether the task was added successfully or not
    bool addTask(osFuncCall new_task, unsigned long delay);

    /// @brief this is so it can take up the standard osFuncCallArg format, and can thus be run as a thread;
    /// @warning DO NOT USE THIS! it is a private task handled by the OS, but it needed to be public so the OS can use it.
    static void runTasks(void* _self);

    TaskID getTaskID();
};

#endif // !__THREAD_POOL_STACK_H__

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