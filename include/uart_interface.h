#pragma once

#include <zephyr.h>
#include <driver.h>
#include <drivers/uart.h>
#include <usb/usb_device.h>
#include <sys/ring_buffer.h>

#include "motor.h"

#define UART_STACK_SIZE 512
#define UART_PRIORITY 2
#define UART_RING_BUF_SIZE 512

typedef struct uart_interface
{
    /* UART device */
    struct device* dev;

    /* motors we're responsible for */
    struct motor_state* motors;

    /* RX buffer */
    uint8_t rx_buf[UART_RING_BUF_SIZE];
    struct ring_buf ring_buf;

    /* thread info */
    k_thread_stack_t stack_array[UART_STACK_SIZE];
    struct k_stack stack;
    struct k_thread thread;
    k_tid_t tid;

    /* synchronization */
    struct k_mutex in_use;
} uart_interface;

void uart_start(struct uart_interface*, motor_state*);
void uart_entry(void*, void*, void*);