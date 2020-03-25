// REAL-TIME O.S. ROUND ROBIN NON-PREEMPTIVE SCHEDULING

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK82F25615.h"
#include "fsl_debug_console.h"

#include "kernel.h"

tcbType tcbArray[NUMBER_OF_THREADS]; // declare an array of tcbType (defined in header file as a structure) without initialize it.

tcbType* currentTcbArrayPtr; // a pointer to a tcbType

uint32_t tcbStack[NUMBER_OF_THREADS][STACK_SIZE]; // two dimensional array



void kernel_stackInit(int i)
{
	tcbArray[i].stackPt = &tcbStack[i][STACK_SIZE - 16];

	tcbStack[i][STACK_SIZE - 1] = 0x01000000; // bit 24 of the stack of each thread serve to set Thumb operating state

	// only for debug purpose
	tcbStack[i][STACK_SIZE - 3] = 0x14141414; // R14
	tcbStack[i][STACK_SIZE - 4] = 0x12121212; // R12
	tcbStack[i][STACK_SIZE - 5] = 0x03030303; // R3
	tcbStack[i][STACK_SIZE - 6] = 0x02020202; // R2
	tcbStack[i][STACK_SIZE - 7] = 0x01010101; // R1
	tcbStack[i][STACK_SIZE - 8] = 0x00000000; // R0


	tcbStack[i][STACK_SIZE - 9] = 0x11111111; // R11
	tcbStack[i][STACK_SIZE - 10] = 0x10101010; // R10
	tcbStack[i][STACK_SIZE - 11] = 0x09090909; // R9
	tcbStack[i][STACK_SIZE - 12] = 0x08080808; // R8
	tcbStack[i][STACK_SIZE - 13] = 0x07070707; // R7
	tcbStack[i][STACK_SIZE - 14] = 0x06060606; // R6
	tcbStack[i][STACK_SIZE - 15] = 0x05050505; // R5
	tcbStack[i][STACK_SIZE - 16] = 0x04040404; // R4
}

void kernel_addThread(void(*task0)(void), void(*task1)(void), void(*task2)(void), void(*task3)(void))
{
	__asm volatile ("cpsid i" : : : "memory"); // This operation must be "atomic", so disable all interrupts


	tcbArray[0].nextTcbArrayPtr = &tcbArray[1];
	tcbArray[1].nextTcbArrayPtr = &tcbArray[2];
	tcbArray[2].nextTcbArrayPtr = &tcbArray[3];
	tcbArray[3].nextTcbArrayPtr = &tcbArray[0];

	currentTcbArrayPtr = &tcbArray[0];

	kernel_stackInit(0);
	tcbStack[0][STACK_SIZE - 2] = (uint32_t)(task0); // the position where storing instruction of this task

	kernel_stackInit(1);
	tcbStack[1][STACK_SIZE - 2] = (uint32_t)(task1);

	kernel_stackInit(2);
	tcbStack[2][STACK_SIZE - 2] = (uint32_t)(task2);

	kernel_stackInit(3);
	tcbStack[3][STACK_SIZE - 2] = (uint32_t)(task3);


	__asm volatile ("cpsie i" : : : "memory");
}

__attribute__((naked)) // compiler must not add extra assembly
void kernel_schedulerLaunch(void)
{
	__asm volatile ("cpsid i" : : : "memory");

	__asm ("LDR R0, =currentTcbArrayPtr"); // R0 = *currentPt
	__asm ("LDR R1, [R0]"); // R2 = currentPt
	__asm ("LDR SP, [R1]"); // SP = currentPt -> stcakPt
	__asm ("POP {R4-R11}");
	__asm ("POP {R0-R3}");
	__asm ("POP {R12}");
	__asm ("ADD SP, SP, #4");
	__asm ("POP {LR}");
	__asm ("ADD SP, SP, #4");

	__asm volatile ("cpsie i" : : : "memory");

	__asm ("BX LR");

}

void kernel_init(uint32_t quanta)
{
	SCB->SHP[2] |= 0xFFFFFFFF; //(1 << 31); // set priority of this core interrupt at 127 (max is 255 and it's the highest priority level for core interrupt)

	SysTick->LOAD =	quanta * (BUS_FREQ / 1000) - 12 ;  // value in ms... minus 12 for exception latency; ...in other words, the deadline of each task

	SysTick->VAL = 0;

	SysTick->CTRL |= (1 << 2) | (1 << 1) | (1 << 0); // clock source is processor clock, interrupt enabled, counter enabled

	kernel_schedulerLaunch();
}

__attribute__((naked)) // compiler must not add extra assembly
void SysTick_Handler(void) // CONTEXT SWITCH; entering this routine R0, R1, R2, R3, R12, LR(R13), PC(R14), PSR(R15), hare automatically saved into stack
{
	__asm volatile ("cpsid i" : : : "memory");

//	 save the context:
	__asm ("PUSH {R4-R11}"); // Save R4, R5, R6, R7, R8, R9, R10, R11 into the stack
	__asm ("LDR R0, =currentTcbArrayPtr"); // R0 now point to currentPt

	__asm ("LDR R1, [R0]"); // R1 = currentPt
	__asm ("STR SP, [R1]"); //

	// load the context:

	__asm ("LDR R1, [R1, #4]"); // R1 = currentPt->next (4 because the next address increment by 4 byte, each data is 32 bit longer...)
	__asm ("STR R1, [R0]"); // currentPt = R1
	__asm ("LDR SP, [R1]"); // SP = currentPt->stackPt
	__asm ("POP {R4-R11}"); // restore registers pushed into the stack

	__asm volatile ("cpsie i" : : : "memory");

	__asm ("BX LR");
}


void SVC_call(void)
{
	__asm volatile ("SVC 0" );
}


void SVC_Handler(void)
{
	__asm("TST LR, #4"); // 0b0100... read the return address of next instruction after this SVC exception

	__asm("ITE EQ"); // if-then-else
	__asm("MRSEQ R0, MSP"); // if equal to zero, copy main stack pointer (MSP register) in R0
	__asm("MRSNE R0, PSP"); // if not equal to zero, copy program stack pointer (PSP register) in R0

	__asm("MOV     R0, #1");
	__asm("BL    C_script_SVC_Handler");
}

void C_script_SVC_Handler(unsigned int *svc_args )
{
	unsigned int svc_number = ((char*)svc_args[6]) [-2];

}
