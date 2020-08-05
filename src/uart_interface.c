#include "uart_interface.h"

void uart_start(struct uart_interface* uart)
{
    /* initialize stack */
    k_stack_init(&uart->stack, uart->stack_array, UART_STACK_SIZE);

    /* start thread */
    uart->tid = k_thread_create(
        &uart->thread, uart->stack_array,
        UART_STACK_SIZE,
        uart_entry, NULL, NULL, NULL,
        UART_PRIORITY, 0, K_NO_WAIT);
    k_thread_start(uart->tid);
}

void uart_entry(void* dummy1, void* dummy2, void* dummy3)
{
    ARG_UNUSED(dummy1);
    ARG_UNUSED(dummy2);
    ARG_UNUSED(dummy3);
}