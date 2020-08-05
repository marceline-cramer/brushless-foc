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
	uart_start(&state.uart);
	spi_start(&state.spi);
	i2c_start(&state.i2c);
	can_start(&state.can);
}