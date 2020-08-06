#include <sys/printk.h>

#include "global_state.h"

struct global_state state;

void main(void)
{
	printk("Hello DiscordDrive!\nRunning on board %s\n", CONFIG_BOARD);

	/* initialize motors */
	for(int i = 0; i < MOTOR_NUM; i++)
	{
		printk("Initializing motor %d\n", i);
		motor_start(&state.motors[i]);
	}

	/* initialize interfaces */
	uart_start(&state.uart, state.motors);
	spi_start(&state.spi);
	i2c_start(&state.i2c);
	can_start(&state.can);
}