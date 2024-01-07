# EDUCATIONAL-FRDM-K82F-2
EDUCATIONAL: Soft Real Time O.S. with Round Robin Task Scheduling - NXP ARM Cortex M4F

This is a basical code pattern for a Round Robin scheduler that assign a fixed time quanta for each task execution. 
SysTick timer and it's core interrupt are used to trigger and change context.

In this example, four tasks execute a simple increment of variable into a infinite "while" loop, to demostrate the context switch and that the four different counters are incremented by the same quantity at each step.

Mutex and semaphore are not yet implemented. Work in Progress. 


