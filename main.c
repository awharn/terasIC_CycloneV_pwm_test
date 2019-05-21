#include "address_map_arm.h"
float Get_Percent(void);
void Set_PWM(unsigned int, float);
void disable_A9_interrupts (void);
void set_A9_IRQ_stack (void);
void config_GIC (void);
void enable_A9_interrupts (void);

volatile int * switches = (int *) 0xFF200040; //Switches are at this address, can change at any moment
volatile int * HPS_Load = (int *) 0xFFC08000; //HPS timer 0 Load Value
volatile int * HPS_Ctrl = (int *) 0xFFC08008; //HPS timer 0 Control Register
volatile int * HPS_EOI = (int *) 0xFFC0800C; //HPS timer 0 End of Interrupt Register
volatile int * HPS_IS = (int *) 0xFFC08010; //HPS timer 0 Interrupt Status
volatile int * JP2 = (int *) 0xFF200070; //JP2 parallel port
volatile int * JP2_DIR = (int *) 0xFF200074; //JP2 parallel port direction
unsigned int period = 1000; //1000 microseconds in 1 millisecond
int PWM_mode = 0; //0 = output off; 1 = output on
int Remaining_Time = 0; //Remaining time when PWM switches

int main(void)
{
	//Prepare JP2 directional control
	*JP2_DIR = 0x1;
	//Prepare interrupts
	disable_A9_interrupts();
	set_A9_IRQ_stack();
	config_GIC();
	enable_A9_interrupts();
	*HPS_Ctrl = 0x3; //Enable interrupts, enable autoload, enable clock
	while (1) //Wait for an interrupt
	{
		;
	}
}
	
float Get_Percent()
{
	//Read Switches
	float percent = 0.0f;
	float switch_value = (*switches & 0x3FF); //Get the present switch value
	percent = (switch_value / 1023) * 100.0; //Get the percent of 1023
	return percent;
}

void Set_PWM(unsigned int period, float percent1)
{
	//float switch_value = (int) (*switches & 0x3FF); //Get the present switch value
	//percent = (switch_value / 1023) * 100.0; //Get the percent of 1023
	if (PWM_mode == 0)
	{
		int time = 0;
		PWM_mode = 1; //Flag that output is ON
		time = (int) (percent1 * period); //Find ON time in microseconds
		Remaining_Time = ((period * 100) - time); //Find OFF time in microseconds
		*HPS_Ctrl = 0x2; //Enable interrupts, autoload, and disable clock
		*HPS_Load = time;
		*HPS_Ctrl = 0x3; //Enable interrupts, autoload, and clock
		if (time != 0)
		{
			*JP2 = 0x1; //Enables Output
			return;
		}
	}
	if (PWM_mode == 1)
	{
		PWM_mode = 0; //Flag that Output is OFF
		*HPS_Ctrl = 0x2; //Enable interrupts, enable autoload, and disable clock
		*HPS_Load = Remaining_Time;
		*HPS_Ctrl = 0x3; //Enable interrupts, autoload, and clock
		if (Remaining_Time != 0)
		{
			*JP2 = 0x0; //Disables output
		}
		return;
	}
}
//100 MHz = 0.01 microsecond cycles
//Therefore Period * Percent * 100 = 1000 microseconds / 1 millisecond maximum
