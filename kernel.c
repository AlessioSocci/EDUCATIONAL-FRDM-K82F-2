// REAL-TIME O.S. COOPERATIVE - NON PREEMPTIVE SCHEDULING

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK82F25615.h"
#include "fsl_debug_console.h"

#include "kernel.h"

uint32_t task0_stack[40];
uint32_t task1_stack[40];

uint32_t* task0_stack_ptr = &task0_stack[40];
uint32_t* task1_stack_ptr = &task1_stack[40];

tcbType tcbs[NUMBER_OF_THREADS];

tcbType* currentPt;

int32_t tcb_stack[NUMBER_OF_THREADS][STACK_SIZE];


void kernel_stackInit(int i)
{
	tcbs[i].stackPt = &tcb_stack[i][STACK_SIZE - 16];

	tcb_stack[i][STACK_SIZE - 1] = 0x01000000; // bit 24 of the stack of each thread is to set Thumb operating state
}

uint8_t kernel_addThread(void(*task0)(void), void(*task1)(void), void(*task2)(void))
{
	__disable_irq(); // This opeartion must be "atomic", so disable all interrupt

	tcbs[0].nextPt = &tcbs[1];
	tcbs[1].nextPt = &tcbs[2];
	tcbs[2].nextPt = &tcbs[0];

	kernel_stackInit(0);
	tcb_stack[0][STACK_SIZE - 2] = (int32_t)(task0);

	kernel_stackInit(1);
	tcb_stack[1][STACK_SIZE - 2] = (int32_t)(task1);

	kernel_stackInit(2);
	tcb_stack[2][STACK_SIZE - 2] = (int32_t)(task2);

	currentPt = &tcbs[0];

	__enable_irq();

	return 1;
}


void kernel_schedulerLaunch(void)
{
	__disable_irq();

	__asm volatile ("LDR R0, =currentPt");
	__asm volatile ("LDR R2, [R0]"); // R2 = currentPt
	__asm volatile ("LDR SP, [R2]"); // SP = currentPt->stackPt
	__asm volatile ("POP {R4-R11}");
	__asm volatile ("POP {R0-R3}");
	__asm volatile ("POP {R12}");
	__asm volatile ("ADD SP, SP, #4");
	__asm volatile ("POP {LR}");
	__asm volatile ("ADD SP, SP, #4");

	__enable_irq();

	__asm ("BX LR");
}

void kernel_init(uint32_t quanta)
{
	SysTick->LOAD =	quanta * (75000000/1000);//0xBA00;

	SCB->SHP[2] |= (1 << 31) ; // set priority of this core interrupt at 127 (max is 255 and it's the highest priority level for core interrupt)

	SysTick->VAL |= 0;

	SysTick->CTRL |= (1 << 2) | (1 << 1) | (1 << 0); // clock source is processor clock, interrupt enabled, counter enabled

	kernel_schedulerLaunch();
}

void SysTick_Handler(void) // CONTEXT SWITCH: Save R0, R1, R2, R3, R12, LR(R13), PC(R14), PSR(R15),
{
	__disable_irq();

	// save the context:

	__asm ("PUSH {R4-R11}"); // Save R4, R5, R6, R7, R8, R9, R10, R11,
	__asm ("LDR R0, = currentPt"); // R0 now point to currentPt
	__asm ("LDR R1, [R0]"); // R1 = currentPt
	__asm ("STR SP, [R1]");

	// load the context:

	__asm ("LDR R1, [R1, #4]"); // R1 = currentPt->next (4 because the next address increment by 4 byte, each data is 32 bit longer...)
	__asm ("STR R1, [R0]"); // currentPt = R0
	__asm ("LDR SP, [R1]"); // SP = currentPt->stackPt
	__asm ("POP {R4-R11}");

	__enable_irq();

	__asm ("BX LR");
}

