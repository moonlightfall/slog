

/* -------------------------------------------------------------------------- */
/* -------------- DEPENDANCIES ---------------------------------------------- */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include "slog_cfg.h"
#include "slog_buf.h"
#include "slog_spec.h"
#include "slog_port.h"
#include "slog_inner.h"
#include "slog_async.h"
#include "slog_event.h"


/* -------------------------------------------------------------------------- */
/* -------------- PRIVATE FUNCTIONS DEFINITION ------------------------------ */

static void *async_output(void *arg)
{
    int ret = -1;
    uint8_t log_level;
    size_t slog_event_buf_len, slog_format_log_len;
    char slog_format_buf[SLOG_FORMAT_BUF_SIZE] = { 0 };
    char slog_event_buf[SLOG_EVENT_BUF_MAXLEN] = { 0 };

    /* block sig */
    sigset_t sig_block;
    sigemptyset(&sig_block);
    sigaddset(&sig_block, SIGSEGV);
    sigaddset(&sig_block, SIGABRT);
    sigaddset(&sig_block, SIGTERM);
    sigaddset(&sig_block, SIGBUS);
    ret = pthread_sigmask(SIG_BLOCK, &sig_block, NULL);
    if (0 != ret) {
        slog_error_inner("log output thread set sig mask error: %s", strerror(ret));
    }

    int core_num = slog_get_cpu_core();
    long num = sysconf(_SC_NPROCESSORS_CONF);
    if ( (core_num < num) && (core_num >= 0) ) {
        cpu_set_t mask;
        CPU_ZERO(&mask);
        CPU_SET(core_num, &mask);
        slog_debug_inner("log output thread set affinity to core: %d", core_num);
        ret = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &mask);
        if (0 != ret) {
            slog_error_inner("log output thread set affinity error: %s", strerror(ret));
        }
    } else if (-1 == core_num) {
        slog_debug_inner("log output thread set default core affinity");
    } else {
        slog_error_inner("core number invalid, should less than %ld(start from 0)", num);
    }

    ret = pthread_setname_np(pthread_self(), "log_output");
    if (0 != ret) {
        slog_error_inner("set log output thread name to log_output error");
    }

    while (1) {
        /* gets and outputs the log */
        while (!slog_buffer_is_empty()) {
            memset(slog_format_buf, 0, sizeof(slog_format_buf));
            memset(slog_event_buf, 0, sizeof(slog_event_buf));

            slog_event_buf_len = slog_buffer_get(slog_event_buf);
            if (0 == slog_event_buf_len) {
                break;
            }

            slog_format_log_len = format_log(slog_format_buf, slog_event_buf);
            if (slog_format_log_len > 0) {
                log_level = ((slog_event_head_t*)slog_event_buf)->slog_level;
                slog_port_output(log_level, slog_format_buf, slog_format_log_len);
            }
        }

        /* check every 3ms */
        usleep(3000);
    }
    return NULL;
}


/* -------------------------------------------------------------------------- */
/* -------------- PUBLIC FUNCTIONS DEFINITION ------------------------------- */

/**
 * asynchronous output mode initialize
 */
int slog_async_init(void)
{
    int ret = -1;
    pthread_t async_output_thread;
    pthread_attr_t thread_attr;

    ret = pthread_attr_init(&thread_attr);
    if (0 != ret) {
        slog_error_inner("log output thread pthread_attr_init error: %s", strerror(ret));
        return -1;
    }

    ret = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    if (0 != ret) {
        slog_error_inner("log output thread pthread_attr_setdetachstate error: %s", strerror(ret));
        return -1;
    }

    ret = pthread_create(&async_output_thread, &thread_attr, async_output, NULL);
    if (0 != ret) {
        slog_error_inner("log output thread pthread_create error: %s", strerror(ret));
        return -1;
    }

    ret = pthread_attr_destroy(&thread_attr);
    if (0 != ret) {
        slog_error_inner("log output thread pthread_attr_destroy error: %s", strerror(ret));
        return -1;
    }

    return 0;
}


/* ============== EOF ======================================================= */