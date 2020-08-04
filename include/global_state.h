#pragma once

#include "global_settings.h"

#include "motor.h"
#include "uart_interface.h"
#include "spi_interface.h"
#include "i2c_interface.h"
#include "can_interface.h"

typedef struct global_state
{
    /* motor states */
    struct motor_state motors[MOTOR_NUM];

    /* interfacing states */
    struct uart_interface uart_client;
    struct spi_interface spi_client;
    struct i2c_interface i2c_client;
    struct can_interface can_client;
} global_state;