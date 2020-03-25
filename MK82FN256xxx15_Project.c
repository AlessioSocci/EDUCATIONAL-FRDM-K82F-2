/*
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    MK82FN256xxx15_Project.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK82F25615.h"
#include "fsl_debug_console.h"
/* TODO: insert other include files here. */

#include "kernel.h"

/* TODO: insert other definitions and declarations here. */

uint32_t count0, count1, count2, count3 = 0;

void task0(void)
{
	while(1)
	{
		count0++;
	}
}

void task1(void)
{
	while(1)
	{
		count1++;
	}
}

void task2(void)
{
	while(1)
	{
		count2++;
	}
}

void task3(void)
{
	while(1)
	{
		count3++;
	}
}


/*
 * @brief   Application entry point.
 */
int main(void)
{
  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */

    SIM->SCGC5 |= (1 << 9); // Clock on PORT A
    SIM->SCGC5 |= (1 << 10); // Clock on PORT B
   	SIM->SCGC5 |= (1 << 11); // Clock on PORT C
   	SIM->SCGC5 |= (1 << 12); // Clock on PORT D
   	SIM->SCGC5 |= (1 << 13); // Clock on PORT E

    PORTB->PCR[0] |= (1 << 8); // Alt 1 mode for GPIO
    PORTB->PCR[1] |= (1 << 8); // Alt 1 mode for GPIO
    PORTB->PCR[2] |= (1 << 8); // Alt 1 mode for GPIO

    GPIOB->PDDR |= (1 << 0); // PTB0 as output
    GPIOB->PDDR |= (1 << 1); // PTB1 as output
    GPIOB->PDDR |= (1 << 2); // PTB2 as output

    GPIOB->PSOR |= (1 << 0);
    GPIOB->PSOR |= (1 << 1);
    GPIOB->PSOR |= (1 << 2);

//    SVC_call(); // example of SVC routine calling

    kernel_addThread(&task0, &task1, &task2, &task3);

    kernel_init(2);

    while(1);

    return 1;
}
