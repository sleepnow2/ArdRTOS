
#ifndef __ARD_RTOS_TASK_H__
#define __ARD_RTOS_TASK_H__

#include <stdint.h>

struct Task {
	uint32_t _validTime;
	uint16_t _sp;
};

bool createTask(osFuncCall job, uint16_t stackSize);

#endif // !__ARD_RTOS_TASK_H__

