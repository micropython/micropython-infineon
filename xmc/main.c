#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "lib/utils/pyexec.h"

#include "gccollect.h"
#include "modnetwork.h"
#include "readline.h"
#include "storage.h"

#include <Libraries/XMCLib/inc/xmc_gpio.h>
#include <Libraries/XMCLib/inc/xmc_uart.h>

static char *stack_top;
static char heap[16 * 1024];
#define LED1    P5_9
#define LED2    P5_8
#define UART_TX P2_14
#define UART_RX P2_15

XMC_GPIO_CONFIG_t uart_tx =
{
  .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT2,
  .output_strength = XMC_GPIO_OUTPUT_STRENGTH_MEDIUM
};

XMC_GPIO_CONFIG_t uart_rx =
{
  .mode = XMC_GPIO_MODE_INPUT_TRISTATE
};

XMC_GPIO_CONFIG_t led =
{
  .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
  .output_strength = XMC_GPIO_OUTPUT_STRENGTH_MEDIUM
};

XMC_UART_CH_CONFIG_t uart_config =
{
  .data_bits = 8U,
  .stop_bits = 1U,
  .baudrate = 115200U
};

int main(int argc, char **argv) {
    XMC_UART_CH_Init(XMC_UART1_CH0, &uart_config);
    XMC_UART_CH_SetInputSource(XMC_UART1_CH0, XMC_UART_CH_INPUT_RXD, USIC1_C0_DX0_P2_15);
    XMC_UART_CH_Start(XMC_UART1_CH0);
    XMC_GPIO_Init(UART_TX,&uart_tx);
    XMC_GPIO_Init(UART_RX,&uart_rx);
    XMC_GPIO_Init(LED1,&led);
    XMC_GPIO_Init(LED2,&led);

    XMC_GPIO_ToggleOutput(LED2);

    int stack_dummy;
    stack_top = (char*)&stack_dummy;

soft_reset:

    #if MICROPY_ENABLE_GC
    gc_init(heap, heap + sizeof(heap));
    #endif
    mp_init();

    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_)); // current dir
    mp_obj_list_init(mp_sys_argv, 0);

    readline_init0();

    for (;;) {
        if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
            if (pyexec_raw_repl() != 0) {
                break;
            }
        } else {
            if (pyexec_friendly_repl() != 0) {
                break;
            }
        }
    }

    printf("PYB: soft reboot\n");

    mp_deinit();

    goto soft_reset;
}

void gc_collect(void) {
    // WARNING: This gc_collect implementation doesn't try to get root
    // pointers from CPU registers, and thus may function incorrectly.
    void *dummy;
    gc_collect_start();
    gc_collect_root(&dummy, ((mp_uint_t)stack_top - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    return NULL;
}

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(uint n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

void nlr_jump_fail(void *val) {
}

void NORETURN __fatal_error(const char *msg) {
    while (1);
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif
