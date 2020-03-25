# EDUCATIONAL-FRDM-K82F-2
EDUCATIONAL: Soft Real Time O.S. with Non - Preemptive Round Robin Task Scheduling - NXP ARM Cortex M4F

This is a basical code pattern for a Round Robin scheduler that assign a fixed time quanta for each task execution. 
SysTick timer and it's core interrupt is used to trigger and change context.

In this example, four tasks execute a simple variable increment in an internal infinite "while" loop, to demostrate the context switch and that the 4 different counters are incremented by the same quantity at each step.

ATTENTION Real Round Robin Scheduling is a Preemptive Task Scheduler, this script not implement its yet. Work in Progress. 


