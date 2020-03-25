
#ifndef KERNEL_H_
#define KERNEL_H_


#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK82F25615.h"
#include "fsl_debug_console.h"


#define BUS_FREQ 					75000000

#define NUMBER_OF_THREADS 			4
#define STACK_SIZE 					100 // reserve one hundred bytes  * 4 = 400 bytes (it's a 32 bit machine) for each task (thread)


struct tcb	// thread control block must have a pointer to stack, pointer to the next thread.
{
	// main elements of tcb:
	uint32_t* stackPt;
	struct tcb* nextTcbArrayPtr;
};


typedef struct tcb tcbType;


void kernel_addThread(void(*task0)(void), void(*task1)(void), void(*task2)(void), void(*task3)(void));
void kernel_init(uint32_t quanta);
void kernel_schedulerLaunch();
void SVC_call(void);

#endif /* KERNEL_H_ */
