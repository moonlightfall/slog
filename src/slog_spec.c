

/* -------------------------------------------------------------------------- */
/* -------------- DEPENDANCIES ---------------------------------------------- */

#include <time.h>
#include <string.h>

#include "logger.h"
#include "slog_spec.h"
#include "slog_async.h"
#include "slog_event.h"
#include "slog_inner.h"


/* -------------------------------------------------------------------------- */
/* -------------- PRIVATE MACROS -------------------------------------------- */

/* output line number max length */
#define SLOG_LINE_NUM_MAX_LEN                16

/* output newline sign */
#define SLOG_NEWLINE_SIGN                    "\n"

#define SLOG_DEF_SIGN                        ' '


/* -------------------------------------------------------------------------- */
/* -------------- PRIVATE VARIABLES ----------------------------------------- */

/* level output info */
static const char *level_output_info[] = {
        [ASSERT]  = "ASSERT | ",
        [ERROR]   = "ERROR  | ",
        [WARN]    = "WARN   | ",
        [INFO]    = "INFO   | ",
        [DEBUG]   = "DEBUG  | ",
        [VERBOSE] = "VERBOSE| ",
};


/* -------------------------------------------------------------------------- */
/* -------------- PUBLIC FUNCTIONS DEFINITION ------------------------------- */

int format_log(char *slog_format_buf, const void *slog_event_buf)
{
    int log_len = 0;

    uint32_t slog_event_length = ((slog_event_head_t *)slog_event_buf)->slog_event_length;
    uint8_t level = ((slog_event_head_t *)slog_event_buf)->slog_level;
    uint32_t line = ((slog_event_head_t *)slog_event_buf)->slog_line;
    struct timeval log_time = ((slog_event_head_t *)slog_event_buf)->slog_time;
    uint8_t tag_len = ((slog_event_head_t *)slog_event_buf)->slog_tag_len;
    uint8_t file_len = ((slog_event_head_t *)slog_event_buf)->slog_file_len;
    uint8_t func_len = ((slog_event_head_t *)slog_event_buf)->slog_func_len;
    uint32_t slog_info_len = slog_event_length - (sizeof(slog_event_head_t) + tag_len + file_len + func_len);

    /* log time */
    slog_format_buf[log_len++] = '[';
    char cur_system_time[SLOG_TIME_FORMAT_LEN] = { 0 };
    struct tm *p = NULL;
    time_t timep = (time_t)log_time.tv_sec;
    p = localtime(&timep);
    if (p == NULL) {
        return -1;
    }

    int time_len = snprintf(cur_system_time, SLOG_TIME_FORMAT_LEN, "%04d-%02d-%02d %02d:%02d:%02d.%06ld", 
        p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, log_time.tv_usec);
    memcpy(slog_format_buf + log_len, cur_system_time, time_len);
    log_len += time_len;
    slog_format_buf[log_len++] = ']';

    slog_format_buf[log_len++] = ' ';

    /* log level */
    memcpy(slog_format_buf + log_len, level_output_info[level], strlen(level_output_info[level]));
    log_len += strlen(level_output_info[level]);

    /* log tag */
    memcpy(slog_format_buf + log_len, (slog_event_buf + sizeof(slog_event_head_t)), tag_len);
    log_len += tag_len;

    slog_format_buf[log_len++] = ' ';

    /* log file */
    slog_format_buf[log_len++] = '(';

    memcpy(slog_format_buf + log_len, (slog_event_buf + sizeof(slog_event_head_t) + tag_len), file_len);
    log_len += file_len;

    slog_format_buf[log_len++] = ' ';

    /* log func */
    memcpy(slog_format_buf + log_len, (slog_event_buf + sizeof(slog_event_head_t) + tag_len + file_len), func_len);
    log_len += func_len;

    slog_format_buf[log_len++] = ':';

    /* log line */
    char line_num[SLOG_LINE_NUM_MAX_LEN] = { 0 };
    int ret = snprintf(line_num, SLOG_LINE_NUM_MAX_LEN, "%d", line);
    memcpy(slog_format_buf + log_len, line_num, ret);
    log_len += ret;

    slog_format_buf[log_len++] = ')';

    slog_format_buf[log_len++] = ' ';

    if (log_len + slog_info_len >= SLOG_FORMAT_BUF_SIZE) {
        slog_error_inner("log too long, abandon");
        return -1;
    }

    /* log format */
    memcpy(slog_format_buf + log_len, (slog_event_buf + sizeof(slog_event_head_t) + tag_len + file_len + func_len), slog_info_len);
    log_len += slog_info_len;

    /* package newline sign */
    slog_format_buf[log_len++] = '\n';

    return log_len;
}


/* ============== EOF ======================================================= */