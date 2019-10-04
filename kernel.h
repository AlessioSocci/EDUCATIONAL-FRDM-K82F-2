
#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK82F25615.h"
#include "fsl_debug_console.h"


extern uint32_t* task0_stack_ptr;
extern uint32_t* task1_stack_ptr;

#define NUMBER_OF_THREADS 			3
#define STACK_SIZE 					100

struct tcb	// thread control block must have a pointer to stack, pointer to the next thread. Additionally can have...
{
	// main elements of tcb:
	int32_t* stackPt;
	struct tcb* nextPt;
};


typedef struct tcb tcbType;


uint8_t kernel_addThread(void(*task0)(void), void(*task1)(void), void(*task2)(void));
void kernel_init(uint32_t quanta);
void SVC_call(void);

#endif /* KERNEL_H_ */
