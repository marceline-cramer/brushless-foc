#pragma once

#include <zephyr.h>

#define UART_STACK_SIZE 500
#define UART_PRIORITY 2

typedef struct uart_interface
{
    /* thread info */
    k_thread_stack_t stack_array[UART_STACK_SIZE];
    struct k_stack stack;
    struct k_thread thread;
    k_tid_t tid;

} uart_interface;

void uart_start(struct uart_interface*);
void uart_entry(void*, void*, void*);