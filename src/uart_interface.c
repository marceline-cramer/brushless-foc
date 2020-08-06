#include "uart_interface.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(uart, UNIVERSAL_LOG_LEVEL);

void uart_interrupt(void* uart_ptr)
{
    uart_interface* uart = (uart_interface*) uart_ptr;

    while(uart_irq_update(uart->dev) && uart_irq_is_pending(uart->dev))
    {
        /* receive from UART */
        if(uart_irq_rx_ready(uart->dev))
        {
            char buf[UART_IRQ_BUF_SIZE];
            int recv_len = uart_fifo_read(uart->dev, buf, UART_IRQ_BUF_SIZE);
            LOG_DBG("Received %d characters", recv_len);

            /* place message in ring buffer */
            k_mutex_lock(&uart->rx.in_use, K_FOREVER);
            int rb_len = ring_buf_put(&uart->rx.ring_buf, buf, recv_len);
            k_mutex_unlock(&uart->rx.in_use);

            if(rb_len < recv_len)
            {
                LOG_ERR("Dropped %u bytes", recv_len - rb_len);
            }

            LOG_DBG("tty fifo -> ringbuf %d bytes", rb_len);
        }

        if(uart_irq_tx_ready(uart->dev))
        {
            uint8_t buffer[UART_IRQ_BUF_SIZE];
            int rb_len, send_len;

            /* grab message from ring buffer */
            k_mutex_lock(&uart->tx.in_use, K_FOREVER);
            rb_len = ring_buf_get(&uart->tx.ring_buf, buffer, UART_IRQ_BUF_SIZE);
            k_mutex_unlock(&uart->tx.in_use);

            if (!rb_len) {
                LOG_DBG("Ring buffer empty, disable TX IRQ");
                uart_irq_tx_disable(uart->dev);
                continue;
            }

            send_len = uart_fifo_fill(uart->dev, buffer, rb_len);
            if (send_len < rb_len) {
                LOG_ERR("Drop %d bytes", rb_len - send_len);
            }

            LOG_DBG("ringbuf -> tty fifo %d bytes", send_len);
        }
    }
}

void uart_start(struct uart_interface* uart, motor_state *motors)
{
    LOG_DBG("Initializing UART");
    k_mutex_init(&uart->in_use);
    k_stack_init(&uart->stack, uart->stack_array, UART_STACK_SIZE);

    k_mutex_init(&uart->rx.in_use);
    ring_buf_init(&uart->rx.ring_buf, UART_RING_BUF_SIZE, uart->rx.buf);

    k_mutex_init(&uart->tx.in_use);
    ring_buf_init(&uart->tx.ring_buf, UART_RING_BUF_SIZE, uart->tx.buf);

    uart->motors = motors;
    uart->command_len = 0;

    LOG_DBG("Binding UART device");
    uart->dev = device_get_binding(STRINGIFY(UART_DEVICE_NAME));
    if(uart->dev == NULL)
    {
        LOG_ERR("UART device not found\n");
        return;
    }

    LOG_DBG("Set UART callback");
    uart_irq_callback_user_data_set(uart->dev, uart_interrupt, (void*) uart);
	uart_irq_rx_enable(uart->dev);

    LOG_DBG("Starting UART thread");
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
        char c;
        int rb_len;

        do
        {
            /* poll UART for input */
            k_mutex_lock(&uart->rx.in_use, K_FOREVER);
            rb_len = ring_buf_get(&uart->rx.ring_buf, &c, 1);
            k_mutex_unlock(&uart->rx.in_use);

            if(rb_len)
            {
                LOG_DBG("Received %c", c);

                /* transmit */
                uart_printc(uart, c);

                /* append the character */
                uart->command_buf[uart->command_len++] = c;

                /* if it's a newline, execute the command */
                if(c == '\r' || c == '\n')
                {
                    uart->command_buf[uart->command_len] = '\0';
                    uart->command_len = 0;

                    printk("Executing %s\n", uart->command_buf);
                }
            }
        } while(rb_len);

        k_msleep(1);
    }
}

/**
 * @brief Prints a single character to a UART interface.
 * @param uart The UART interface to print to
 * @param c The character being printed
 */
void uart_printc(uart_interface* uart, char c)
{
    k_mutex_lock(&uart->tx.in_use, K_FOREVER);
    ring_buf_put(&uart->tx.ring_buf, &c, 1);
    k_mutex_unlock(&uart->tx.in_use);
    uart_irq_tx_enable(uart->dev);
}

/**
 * @brief Prints a string to a UART interface, followed by a newline.
 * @param uart The UART interface to print to
 * @param str The C-string being printed
 */
void uart_println(uart_interface* uart, const char* str)
{
    k_mutex_lock(&uart->tx.in_use, K_FOREVER);
    ring_buf_put(&uart->tx.ring_buf, str, strlen(str));
    ring_buf_put(&uart->tx.ring_buf, "\r\n", 2);
    k_mutex_unlock(&uart->tx.in_use);
    uart_irq_tx_enable(uart->dev);
}