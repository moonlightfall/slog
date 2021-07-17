
/* -------------------------------------------------------------------------- */
/* -------------- DEPENDANCIES ---------------------------------------------- */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <pthread.h>

#include "logger.h"
#include "slog_cfg.h"
#include "slog_buf.h"
#include "slog_port.h"
#include "slog_event.h"
#include "slog_async.h"
#include "slog_inner.h"
#include "slog_compiler.h"

/* -------------------------------------------------------------------------- */
/* -------------- PRIVATE VARIABLES ----------------------------------------- */

static int slog_is_init = 0;


/* -------------------------------------------------------------------------- */
/* -------------- PUBLIC FUNCTIONS DEFINITION ------------------------------- */

int log_init(void)
{
    if (unlikely(slog_is_init)) {
        return 0;
    }

    /* initialize slog resources */
    if (0 != slog_buffer_init()) {
        slog_error_inner("slog_buffer_init error");
        return -1;
    }

    /* set default log config */
    slog_set_config_default();

    /* parse and set log config */
    slog_config_parse();

    /* port initialize */
    if (0 != slog_port_init()) {
        slog_error_inner("slog_port_init error");
        return -1;
    }

    if (0 != slog_async_init()) {
        slog_error_inner("slog_async_init error");
        return -1;
    }

    slog_is_init = 1;

    return 0;
}

void log_fini(void)
{
    if (unlikely(!slog_is_init)) {
        return;
    }

    /* set slog_is_init to 0 */
    __sync_sub_and_fetch(&slog_is_init, 1);

    slog_buffer_deinit();

    slog_port_deinit();
}

void slog(uint8_t level, const char *tag, size_t tag_len, const char *file, size_t file_len, \
          const char *func, size_t func_len, long line, const char *format, ...)
{
    if (unlikely(!slog_is_init)) {
        slog_error_inner("slog has not init");
        return;
    }

    /* check output enabled */
    if (!slog_get_output_enabled()) {
        return;
    }

    if (level > slog_get_filter_level()) {
        return;
    }

    // char filter_tag[SLOG_FILTER_TAG_MAX_LEN] = { 0 };
    // slog_get_filter_tag(filter_tag);
    // if ( (strlen(filter_tag) >= 1) && !strstr(tag, filter_tag)) {
    //     return;
    // }

    /* length limit to uint8_t */
    if (tag_len >= 255 || file_len >= 255 || func_len >= 255) {
        slog_error_inner("tag or file or func length too long, abandon");
        return;
    }

    va_list args;
    char slog_event_buf[SLOG_EVENT_BUF_MAXLEN] = { 0 };

    va_start(args, format);
    slog_event_buf_set(slog_event_buf, tag, tag_len,
			file, file_len, func, func_len, line, level,
			format, args);
    va_end(args);

    slog_buffer_put(slog_event_buf);
}


/* ============== EOF ======================================================= */