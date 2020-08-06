#include "uart_interface.h"

#define UART_DEVICE_NAME UART_2
#define UART_NODE DT_NODELABEL(UART_DEVICE_NAME)
#define UART_LABEL DT_GPIO_LABEL(UART_NODE, gpios)

/**
 * @brief Prints a C-string to a UART device.
 * 
 * @param dev UART device to print to
 * @param str C-string to print
 */
void uart_print(struct device* dev, const char* str)
{
    for(const char* c = str; *c; c++)
    {
        uart_poll_out(dev, *c);
    }
}


static void uart_interrupt(struct device* dev, void* uart_ptr)
{
    uart_interface* uart = (uart_interface*) uart_ptr;

    /* handle interrupts */
    while(uart_irq_update(dev) && uart_irq_is_pending(dev))
    {
        if(uart_irq_rx_ready(dev))
        {
            int recv_len, rb_len;
            uint8_t buffer[64];
            size_t len = MIN(ring_buf_space_get(&uart->ring_buf), sizeof(buffer));
            
            recv_len = uart_fifo_read(dev, buffer, len);

            rb_len = ring_buf_put(&uart->ring_buf, buffer, recv_len);
            if(rb_len < recv_len)
            {
                printk("ERROR: Drop %u bytes\n", recv_len - rb_len);
            }

            printk("tty fifo -> ring_buf %d bytes", rb_len);

            uart_irq_tx_enable(dev);
        }

        if(uart_irq_tx_ready(dev))
        {
            uint8_t buffer[64];
            int rb_len, send_len;

            rb_len = ring_buf_get(&uart->ring_buf, buffer, sizeof(buffer));
            if(!rb_len)
            {
                printk("ERROR: Ring buffer empty, disable TX IRQ\n");
                uart_irq_tx_disable(dev);
                continue;
            }

            send_len = uart_fifo_fill(dev, buffer, rb_len);
            if(send_len < rb_len)
            {
                printk("ERROR: Drop %d bytes", rb_len - send_len);
            }

            printk("ringbuf -> tty fifo %d bytes\n", send_len);
        }
    }
}

void uart_start(struct uart_interface* uart, motor_state *motors)
{
    /* initialize various data structures */
    k_mutex_init(&uart->in_use);
    k_stack_init(&uart->stack, uart->stack_array, UART_STACK_SIZE);
    ring_buf_init(&uart->ring_buf, UART_RING_BUF_SIZE, uart->rx_buf);
    uart->motors = motors;

    /* create UART device */
    uart->dev = device_get_binding("UART_2");
    if(uart->dev == NULL)
    {
        printk("ERROR: UART device not found\n");
        return;
    }
    uart_print(uart->dev, "UART is ready\r\n");

    /* initialize UART */
    uart_irq_callback_user_data_set(uart->dev, uart_interrupt, (void*) uart);
	uart_irq_rx_enable(uart->dev);

    /* start thread */
    uart->tid = k_thread_create(
        &uart->thread, uart->stack_array,
        UART_STACK_SIZE,
        uart_entry, (void*) uart, NULL, NULL,
        UART_PRIORITY, 0, K_NO_WAIT);
    k_thread_start(uart->tid);
}

void uart_entry(void* uart_ptr, void* dummy2, void* dummy3)
{
    ARG_UNUSED(dummy2);
    ARG_UNUSED(dummy3);

    uart_interface* uart = (uart_interface*) uart_ptr;

    while(true)
    {
        /* wait for UART to become available */
        k_mutex_lock(&uart->in_use, K_FOREVER);

        /* poll for input */

        /* release UART */
        k_mutex_unlock(&uart->in_use);
    }
}