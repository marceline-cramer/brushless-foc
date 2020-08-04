#include "foc.h"

void foc_start(struct motor_state *motor)
{
    /* initialize stack */
    k_stack_init(&motor->foc_stack, motor->foc_stack_array, FOC_STACK_SIZE);

    /* start thread */
    motor->foc_tid = k_thread_create(
        &motor->foc_thread, motor->foc_stack_array,
        FOC_STACK_SIZE,
        foc_entry, (void*) motor, NULL, NULL,
        FOC_PRIORITY, 0, K_NO_WAIT);
    k_thread_start(motor->foc_tid);
}

void foc_entry(void* motor_ptr, void* dummy2, void* dummy3)
{
    ARG_UNUSED(dummy2);
    ARG_UNUSED(dummy3);

    /* get the motor we're responsible for */
    struct motor_state *motor = (motor_state*) motor_ptr;
    if(!motor) return;

    while(true)
    {
        /* wait for our motor to become available */
        k_mutex_lock(&motor->in_use, K_FOREVER);

        /* perform processing here */

        /* release our motor */
        k_mutex_unlock(&motor->in_use);
        k_msleep(100);
    }
}