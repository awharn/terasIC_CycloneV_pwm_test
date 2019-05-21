#include "address_map_arm.h"
extern int PWM_mode;
extern unsigned int period;
extern int Remaining_Time;
extern int * HPS_Ctrl;
extern int * HPS_EOI;
extern int * HPS_IS;
/****************************************************************************************
 * Clock - Interrupt Service Routine                                
 *                                                                          
 * Controls the PWM output
 ***************************************************************************************/
void clock_ISR( void )
{
	*HPS_EOI; //Clear End of Interrupt
	Set_PWM(period, Get_Percent());
	return;
}
