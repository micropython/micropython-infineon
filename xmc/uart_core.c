#include <unistd.h>
#include "py/mpconfig.h"

#include <Libraries/XMCLib/inc/xmc_gpio.h>
#include <Libraries/XMCLib/inc/xmc_uart.h>
#include <Libraries/XMCLib/inc/xmc_common.h>
/*
 * Core UART functions to implement for a port
 */

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    while(!(XMC_USIC_CH_GetReceiveBufferStatus(XMC_UART1_CH0)));
    return XMC_UART_CH_GetReceivedData(XMC_UART1_CH0);
    }

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    while (len--) {
        XMC_UART_CH_Transmit(XMC_UART1_CH0, *str++);
    }
}
