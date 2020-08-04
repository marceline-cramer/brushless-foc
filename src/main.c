#include "global_state.h"

void main()
{
	/* create global state */
	struct global_state state;

    /* initialize motors */ 
	for(int i = 0; i < MOTOR_NUM; i++)
	{
		motor_start(&state.motors[i]);
	}

	/* initialize interfaces */
	// uart_start(&state.uart_client);
	// spi_start(&state.spi_client);
	// i2c_start(&state.i2c_client);
	// can_start(&state.can_client);
}