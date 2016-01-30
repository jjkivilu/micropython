#include <sys/types.h>
#include <string.h>
#include "py/mphal.h"
#include "mcu_api.h"

int mp_hal_stdin_rx_chr(void) {
    unsigned char c = 0;
    host_receive(&c, 1);
    return c;
}

void mp_hal_stdout_tx_strn(const char *str, size_t len) {
    host_send((unsigned char *) str, len);
}

void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}

void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {
    while (len--) {
        if (*str == '\n') {
            mp_hal_stdout_tx_strn("\r", 1);
        }
        mp_hal_stdout_tx_strn(str++, 1);
    }
}

void mp_hal_delay_ms(mp_uint_t ms) {
    if (ms < 10)
        mcu_delay(ms * 1000);
    else
        mcu_sleep(ms / 10);
}

mp_uint_t mp_hal_ticks_ms(void) {
    return time_ms();
}

