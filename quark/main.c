#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "py/mpconfig.h"
#include "py/mphal.h"
#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "lib/utils/pyexec.h"

#include "mcu_api.h"
#include "mcu_errno.h"

void do_str(const char *src, mp_parse_input_kind_t input_kind) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
    if (lex == NULL) {
        printf("MemoryError: lexer could not allocate memory\n");
        return;
    }

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, MP_EMIT_OPT_NONE, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}

static char *stack_top;
static char heap[4096];

void mcu_main()
{
    int stack_dummy;
    stack_top = (char*)&stack_dummy;

#if MICROPY_ENABLE_GC
    gc_init(heap, heap + sizeof(heap));
#endif
    mp_init();
#if MICROPY_REPL_EVENT_DRIVEN
    pyexec_event_repl_init();
    for (;;) {
        int c = mp_hal_stdin_rx_chr();
        if (pyexec_event_repl_process_char(c)) {
            break;
        }
    }
#else
    pyexec_friendly_repl();
#endif
    //do_str("print('hello world!', list(x+1 for x in range(10)), end='eol\\n')", MP_PARSE_SINGLE_INPUT);
    //do_str("for i in range(10):\r\n  print(i)", MP_PARSE_FILE_INPUT);
    mp_deinit();
}

void gc_collect(void) {
    // WARNING: This gc_collect implementation doesn't try to get root
    // pointers from CPU registers, and thus may function incorrectly.
    void *dummy;
    gc_collect_start();
    gc_collect_root(&dummy, ((mp_uint_t)stack_top - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
    gc_dump_info();
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    debug_print(DBG_WARNING, "Can't read code from files yet");
    return NULL;
}

mp_import_stat_t mp_import_stat(const char *path) {
    debug_print(DBG_WARNING, "Can't import modules yet");
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(uint n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    debug_print(DBG_WARNING, "open() builtin not supported");
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, (mp_fun_kw_t) mp_builtin_open);

void nlr_jump_fail(void *val) {
    debug_print(DBG_ERROR, "nlr_jump_fail");
}

void NORETURN __fatal_error(const char *msg) {
    debug_print(DBG_FATAL, msg);
    while (1)
        mcu_sleep(100000);
}
