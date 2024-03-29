////***********************************************************************************
//// Include files
////***********************************************************************************
//#include "letimer.h"
//
////***********************************************************************************
//// defined files
////***********************************************************************************
//
//
////***********************************************************************************
//// global variables
////***********************************************************************************
//
//
////***********************************************************************************
//// function prototypes
////***********************************************************************************


//***********************************************************************************
// Include files
//***********************************************************************************
#include "letimer.h"
#include "i2c.h"
#include "sleep_routines.h"
#include "gpio.h"
#include "em_gpio.h"
//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************


/************** toggles SCL line 9 times and resets I2C line ***************/
// inputs: none
// outputs: none
/***************************************************************************/
void i2c_reset()
{
	for(int i = 0; i < 9; i++)
	{
		GPIO_PinOutClear(SCLport, SCLpin);
		GPIO_PinOutSet(SCLport, SCLpin);
	}
	if(I2C0->STATE & I2C_STATE_BUSY)
	{
		I2C0->CMD = I2C_CMD_ABORT;
	}
   // I2C0->CMD = I2C_CMD_CLEARPC;
}

//****************************************************************************************************************
// brief: letimer initialization routine for comp0 and comp1 interrupts every three seconds
// inputs: none
// outputs: none
//****************************************************************************************************************
void letimer0_init(void){
	unsigned int	seconds_ticks, on_ticks;
	unsigned int	letimer0_tick_seconds;
	unsigned int	pres_cal, effective_pres_cal;

	LETIMER_Init_TypeDef letimer0_values;

	if (Letimer0_EM == EM4) letimer0_tick_seconds = Letimer0_EM3_Ticks;
	else letimer0_tick_seconds = Letimer0_Std_Ticks;

	// second ticks required before prescaling
	seconds_ticks = Letimer0_Period * letimer0_tick_seconds - 1;	// minus 1 due to count starts from 0 and not 1


	// seconds_ticks: comp0
	// on_ticks: comp1
	pres_cal = 0;
	effective_pres_cal = 1;
	while ((seconds_ticks / Letimer0_16bit_Ticks) > 0){
		pres_cal++;
		letimer0_tick_seconds = letimer0_tick_seconds >> 1;
		seconds_ticks = Letimer0_Period * letimer0_tick_seconds - 1;	// minus 1 due to count starts from 0 and not 1
		effective_pres_cal = effective_pres_cal << 1;
	}

	on_ticks = seconds_ticks - (Letimer0_On_Time * letimer0_tick_seconds) / 1000;  //Adjust for milli-seconds

	if (effective_pres_cal > 1) CMU_ClockPrescSet(cmuClock_LETIMER0, effective_pres_cal);

	// Initialize LETIMER0
	letimer0_values.bufTop = false;			// Comp1 will not be used to load comp0, but used to create an on-time/duty cycle
	letimer0_values.comp0Top = true;		// load comp0 into cnt register when count register underflows enabling continuous looping
	letimer0_values.debugRun = false;		// disable stop letimer0 during debug to assist in debugging
	letimer0_values.enable = false;			// will enable after set up is complete
	letimer0_values.out0Pol = 0;			// Not outputting any value
	letimer0_values.out1Pol = 0;			// Not outputting any value
	letimer0_values.repMode = letimerRepeatFree;	// Set up Letimer0 for free running for continuous looping
	letimer0_values.ufoa0 = letimerUFOANone;		// No action of outputs on underflow
	letimer0_values.ufoa1 = letimerUFOANone;		// No action of outputs on underflow

	LETIMER_Init(LETIMER0, &letimer0_values);		// Initialize LETIMER0

	LETIMER_CompareSet(LETIMER0, 0, seconds_ticks);				// load comp0 register
	LETIMER_CompareSet(LETIMER0, 1, on_ticks);					// load comp1 register

	LETIMER0->IFC = LETIMER_IFC_COMP0 | LETIMER_IFC_COMP1;	// clear any residual interrupt
	LETIMER0->IEN |= LETIMER_IEN_COMP0 | LETIMER_IEN_COMP1;	// enable interrupts at the peripheral

	Sleep_Block_Mode(EM3);			// Set lowest sleep mode setting for LETIMER0

	NVIC_EnableIRQ(LETIMER0_IRQn);				// Enable the peripheral interrupt at the core


}
//************************************************************************************
// brief: LETIMER interrupt service routine handles comp0 and comp1 interrupts
// inputs: letimer interrupt flag, scheduler event
// outputs: new scheduler event indicating which interrupts occurred
//************************************************************************************
void LETIMER0_IRQHandler(void){
	unsigned int int_flag;

	CORE_ATOMIC_IRQ_DISABLE	();		// prevent nesting of interrupts for initial interrupt handling
	int_flag = LETIMER0->IF;		// store cause of interrupt
	LETIMER0->IFC = int_flag;		// clear cause  of interrupt

/***********wait for sensor to initialize on COMP0**********/
	if (int_flag & LETIMER_IF_COMP0)
	{
		scheduler_event |= FLAG3;

	}


	/****************** comp1 interrupt, read temp and send via UART *********************/
	if (int_flag & LETIMER_IF_COMP1)
	{
		scheduler_event |= FLAG4;
	}
	CORE_ATOMIC_IRQ_ENABLE();		// re-enabling interrupts
}


