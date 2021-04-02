
/* -------------------------------------------------------------------------- */
/* -------------- DEPENDANCIES ---------------------------------------------- */

#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>

#include "slog_event.h"


/* -------------------------------------------------------------------------- */
/* -------------- PRIVATE VARIABLES ----------------------------------------- */

static pthread_mutex_t fifo_lock = PTHREAD_MUTEX_INITIALIZER;


/* -------------------------------------------------------------------------- */
/* -------------- PUBLIC FUNCTIONS DEFINITION ------------------------------- */

void slog_event_buf_set(void *slog_buf, const char *tag, uint8_t tag_len,
			const char *file, uint8_t file_len, const char *func, uint8_t func_len,
			uint32_t line, uint8_t level, const char *format, va_list args)
{
    int total_length = 0;
    int format_length = 0;
    struct timeval current_time;
    gettimeofday(&current_time, NULL);

	/* set event head */
	((slog_event_head_t*)slog_buf)->slog_level = level;
	((slog_event_head_t*)slog_buf)->slog_tag_len = tag_len;
	((slog_event_head_t*)slog_buf)->slog_file_len = file_len;
    ((slog_event_head_t*)slog_buf)->slog_func_len = func_len;
	((slog_event_head_t*)slog_buf)->slog_line = line;
	((slog_event_head_t*)slog_buf)->slog_time = current_time;

    /* set tag */
    memcpy((char*)slog_buf + sizeof(slog_event_head_t), tag, tag_len);

	/* set file */
    memcpy((char*)slog_buf + sizeof(slog_event_head_t) + tag_len, file, file_len);

	/* set func */
    memcpy((char*)slog_buf + sizeof(slog_event_head_t) + tag_len + file_len, func, func_len);

	/* set format */
    format_length = vsnprintf(((char *)slog_buf + sizeof(slog_event_head_t) + tag_len + file_len + func_len), \
                              SLOG_EVENT_BUF_MAXLEN - (sizeof(slog_event_head_t) + tag_len + file_len + func_len), \
                              format, args);

    /* set total length */
	total_length = sizeof(slog_event_head_t) + tag_len + file_len + func_len + format_length;
	((slog_event_head_t*)slog_buf)->slog_event_length = total_length;

	return;
}

void slog_event_fifo_put(struct kfifo *fifo, void *slog_event_buf)
{
    pthread_mutex_lock(&fifo_lock);
    if (kfifo_avail(fifo) >= ((slog_event_head_t*)slog_event_buf)->slog_event_length) {
        kfifo_in(fifo, slog_event_buf, ((slog_event_head_t*)slog_event_buf)->slog_event_length);
    }
    pthread_mutex_unlock(&fifo_lock);
}

size_t slog_event_fifo_get(struct kfifo *fifo, void *slog_event_buf)
{
	uint32_t log_event_body_length = 0;

	if (sizeof(slog_event_head_t) != kfifo_out(fifo, slog_event_buf, sizeof(slog_event_head_t))) {
		return 0;
	}

	log_event_body_length = ((slog_event_head_t*)slog_event_buf)->slog_event_length - sizeof(slog_event_head_t);

	if (log_event_body_length != kfifo_out(fifo, ((char*)slog_event_buf) + sizeof(slog_event_head_t), log_event_body_length)) {
		return 0;
	}

	return ((slog_event_head_t*)slog_event_buf)->slog_event_length;
}


/* ============== EOF ======================================================= */