#include "motor.h"
#include "foc.h"

void motor_start(struct motor_state* motor)
{
    /* initialize mutex */
    k_mutex_init(&motor->in_use);

    /* start FOC thread */
    foc_start(motor);

    /* initialize encoders */
    /* initialize drivers */
}