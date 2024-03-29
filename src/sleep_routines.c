/* This code is originally Silicon Labs and copy righted by Silicon Labs� in 2015 and Silicon Labs� grants
 * permission to anyone to use the software for any purpose, including commercial applications, and to alter
 * it, and redistribute it freely subject that the origins is not miss represented, altered source version must
 * be plainly marked, and this notice cannot be altered or removed from any source distribution.
 *
 * Names of routines have been changed to confirm to the naming convention of the application developer.
 *
 * Routines include:
 *
 * void Sleep_Block_Mode(unsigned int EM);
 * void Sleep_UnBlock_Mode(unsigned int EM);
 * void Enter_Sleep(void);
 *
 */



//***********************************************************************************
// Include files
//***********************************************************************************
#include "sleep_routines.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
//********************************************************
// brief: block input sleep mode so board cannot enter that mode
// input: assigned energy mode
// output: none
//********************************************************
void Sleep_Block_Mode(unsigned int EM){
	CORE_ATOMIC_IRQ_DISABLE();

	lowest_energy_mode[EM]++;

	CORE_ATOMIC_IRQ_ENABLE();
}

//********************************************************
// brief: unblock input sleep mode so board cannot enter that mode
// input: assigned energy mode
// output: none
//********************************************************
void Sleep_UnBlock_Mode(unsigned int EM){
	CORE_ATOMIC_IRQ_DISABLE();

	if (lowest_energy_mode[EM] > 0) lowest_energy_mode[EM]--;

	CORE_ATOMIC_IRQ_ENABLE();
}

//********************************************************
// brief: enter highest energy mode defined by the sleep mode array
// input: none
// output: none
//********************************************************
void Enter_Sleep(void){

	if (lowest_energy_mode[EM0] > 0) {
		return;
	}
	else if (lowest_energy_mode[EM1] > 0) {
		return;
	}
	else if (lowest_energy_mode[EM2] > 0) {
			EMU_EnterEM1();
	}
	else if (lowest_energy_mode[EM3] > 0) {
		EMU_EnterEM2(true);
	}
	else EMU_EnterEM3(true);

	return;
}

