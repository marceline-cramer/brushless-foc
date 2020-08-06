#pragma once

#include <string.h>

#include <zephyr.h>
#include <driver.h>
#include <drivers/uart.h>
#include <usb/usb_device.h>
#include <sys/ring_buffer.h>

#include "motor.h"

#define UART_DEVICE_NAME UART_2
#define UART_STACK_SIZE 512
#define UART_PRIORITY 0
#define UART_RING_BUF_SIZE 512
#define UART_IRQ_BUF_SIZE 64
#define UART_COMMAND_SIZE 128

typedef struct uart_interface
{
    /* UART device */
    struct device* dev;

    /* motors we're responsible for */
    struct motor_state* motors;

    /* RX buffer */
    struct rx_buf
    {
        uint8_t buf[UART_RING_BUF_SIZE];
        struct ring_buf ring_buf;
        struct k_mutex in_use;
    } rx;

    /* TX buffer */
    struct tx_buf
    {
        uint8_t buf[UART_RING_BUF_SIZE];
        struct ring_buf ring_buf;
        struct k_mutex in_use;
    } tx;

    /* command buffer */
    char command_buf[UART_COMMAND_SIZE];
    int command_len;

    /* thread info */
    k_thread_stack_t stack_array[UART_STACK_SIZE];
    struct k_stack stack;
    struct k_thread thread;
    k_tid_t tid;

    /* synchronization */
    struct k_mutex in_use;
} uart_interface;

void uart_start(struct uart_interface*, motor_state*);
void uart_interrupt(void*);
void uart_entry(void*, void*, void*);

void uart_printc(uart_interface*, char);
void uart_println(uart_interface*, const char*);