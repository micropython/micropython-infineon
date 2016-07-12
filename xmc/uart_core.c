#include <unistd.h>
#include "py/mpconfig.h"

#include <Class/Device/CDCClassDevice.h>

#include "VirtualSerial.h"
#include <Core/USBTask.h>
/*
 * Core UART functions to implement for a port
 */

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    while(!CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface)) {
        CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
    }
    return CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
}

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    while (len--) {
        CDC_Device_SendByte(&VirtualSerial_CDC_Interface, *str++);
    }
}
