/* Adapted from Xilinx xtmrctr_polled_example.c and xtmrctr_intr_64bit_example.c */

#include "xparameters.h"
#include "xtmrctr.h"

#include <stdio.h>
/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are only defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define TMRCTR_DEVICE_ID	XPAR_TMRCTR_0_DEVICE_ID

/*
 * The following constants identify each of the two timer counters of the device */
#define TIMER_CNTR_0	 0
#define TIMER_CNTR_1	 1

/* The following constants are used to set the reset value of the timer counters */
#define RESET_VALUE_CNTR_0	 0
#define RESET_VALUE_CNTR_1	 0

/* This constant defines if the two timer counters are used in cascade mode */
#define TIMER_CASCADE_MODE 1

/************************** Function Prototypes ******************************/
int init_timer (int cascade_mode);
void start_timer(int cascade_mode);
u32 get_timer_val ();
u32 get_timer64_val (u32 *val0);
float conv_cycles_to_msecs (u32 cycles);
float conv2_cycles_to_msecs (u32 cycles1, u32 cycles0);

/************************** Variable Definitions *****************************/
XTmrCtr TimerCounter; /* The instance of the Tmrctr Device */

/*
int main(void)
{
	int Status, i, j;
	u32 Val, Val1;

	Status = init_timer(TIMER_CASCADE_MODE);
	if (Status != XST_SUCCESS) {
		xil_printf("Init Error\n");
		return XST_FAILURE;
	}

	Val = get_timer_val ();
	xil_printf("Time = %d\n", Val);

	start_timer(TIMER_CASCADE_MODE);
	Val = get_timer_val ();
	xil_printf("Time = %d\n", Val);

	for (j=0; j < 10; j++) {
		for (i=0; i < 99999999; i++);
		Val1 = get_timer64_val (&Val);
		xil_printf("Time64 = %x %x (%d msec)\n", Val1, Val,
				(unsigned int)conv2_cycles_to_msecs (Val1, Val));
	}

	start_timer(TIMER_CASCADE_MODE);
	Val1 = get_timer64_val (&Val);
	xil_printf("Time64 = %x %x (%d msec)\n", Val1, Val,
			(unsigned int)conv2_cycles_to_msecs (Val1, Val));

	for (j=0; j < 10; j++) {
		for (i=0; i < 99999999; i++);
		Val1 = get_timer64_val (&Val);
		xil_printf("Time64 = %x %x (%d msec)\n", Val1, Val,
				(unsigned int)conv2_cycles_to_msecs (Val1, Val));
	}

	start_timer(TIMER_CASCADE_MODE);
	Val1 = get_timer64_val (&Val);
	xil_printf("Time64 = %x %x (%d msec)\n", Val1, Val,
			(unsigned int)conv2_cycles_to_msecs (Val1, Val));

	return XST_SUCCESS;
}
*/

int init_timer (int cascade_mode)
{
	int Status;

	/*
	 * Initialize the timer counter so that it's ready to use,
	 * specify the device ID that is generated in xparameters.h
	 */
	Status = XTmrCtr_Initialize(&TimerCounter, TMRCTR_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to ensure that the hardware was built
	 * correctly, use the 1st timer in the device (0)
	 */
	Status = XTmrCtr_SelfTest(&TimerCounter, TIMER_CNTR_0);
	if (Status != XST_SUCCESS) {
	    return XST_FAILURE;
	}

	/* if mode is cascade also self-test the 2nd timer */
	if (cascade_mode) {
		Status = XTmrCtr_SelfTest(&TimerCounter, TIMER_CNTR_1);
		if (Status != XST_SUCCESS) {
		    return XST_FAILURE;
		}
	}

	/* Set a reset value for the timer counter(s) */
	XTmrCtr_SetResetValue(&TimerCounter, TIMER_CNTR_0, RESET_VALUE_CNTR_0);
	if (cascade_mode)XTmrCtr_SetResetValue(&TimerCounter, TIMER_CNTR_1, RESET_VALUE_CNTR_1);

	/* Enable the Autoreload mode of the timer counters. */
	if (cascade_mode) {
		XTmrCtr_SetOptions(&TimerCounter, TIMER_CNTR_0, XTC_AUTO_RELOAD_OPTION | XTC_CASCADE_MODE_OPTION);
		XTmrCtr_SetOptions(&TimerCounter, TIMER_CNTR_1, XTC_AUTO_RELOAD_OPTION);
	}
	else {
		XTmrCtr_SetOptions(&TimerCounter, TIMER_CNTR_0, XTC_AUTO_RELOAD_OPTION);
	}

	/* Reset the timer counters such that it's incrementing by default */
    XTmrCtr_Reset(&TimerCounter, TIMER_CNTR_0);
	XTmrCtr_Reset(&TimerCounter, TIMER_CNTR_1);
	return XST_SUCCESS;
}

void start_timer(int cascade_mode)
{
	if (cascade_mode) XTmrCtr_Reset(&TimerCounter, TIMER_CNTR_1);
	XTmrCtr_Start(&TimerCounter, TIMER_CNTR_0);
}


/* Returns value of timer 0 */
u32 get_timer_val ()
{
  return XTmrCtr_GetValue(&TimerCounter, TIMER_CNTR_0);
}

/* Returns values of two timers 1 and 0, as used when configured as a 64-bit counter */
u32 get_timer64_val (u32 *val0)
{
	u32 v11, v10, v0;

	/* The following are the steps for reading the 64-bit counter/timer:
	 * 1. Read the upper 32-bit timer/counter register (TCR1).
	 * 2. Read the lower 32-bit timer/counter register (TCR0).
	 * 3. Read the upper 32-bit timer/counter register (TCR1) again.
	 *   If the value is different from the 32-bit upper value read previously,
	 *   go back to previous step (reading TCR0).
	 *   Otherwise the 64-bit timer counter value is correct.
	 */
	v10 = XTmrCtr_GetValue(&TimerCounter, TIMER_CNTR_1);
	for (;;) {
		v0 = XTmrCtr_GetValue(&TimerCounter, TIMER_CNTR_0);
		v11 = XTmrCtr_GetValue(&TimerCounter, TIMER_CNTR_1);
		if (v11 == v10) {
			*val0 = v0;
			break;
		}
		v10 = v11;
	}
	return v11;
}

float conv_cycles_to_msecs (u32 cycles)
{
  return ((float)cycles * (float)1000 / (float)XPAR_CPU_CORE_CLOCK_FREQ_HZ);
}

float conv2_cycles_to_msecs (u32 cycles1, u32 cycles0)
{
	float cycles;
	cycles = (float)cycles1 * (float)0x10000;
	cycles *= (float)0x10000;
	cycles += (float)cycles0;
	cycles /= (float)XPAR_CPU_CORE_CLOCK_FREQ_HZ;
	return (cycles * (float)1000) ;
}
