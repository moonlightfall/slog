
#ifndef __SLOG_EVENT_H
#define __SLOG_EVENT_H

/* -------------------------------------------------------------------------- */
/* -------------- DEPENDANCIES ---------------------------------------------- */

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <sys/time.h>

#include "slog_fifo.h"


/* -------------------------------------------------------------------------- */
/* -------------- PUBLIC MACROS --------------------------------------------- */

#define SLOG_EVENT_BUF_MAXLEN                (8192)


/* -------------------------------------------------------------------------- */
/* -------------- PUBLIC TYPES ---------------------------------------------- */

typedef struct slog_event_head
{
	uint32_t slog_event_length;
	uint8_t slog_level;
	uint8_t slog_tag_len;
	uint8_t slog_file_len;
	uint8_t slog_func_len;
	uint32_t slog_line;
	struct timeval slog_time;
}__attribute__((packed)) slog_event_head_t;

typedef struct slog_event
{
    struct slog_event_head slog_head;
	char *tag;
	char *file;
	char *func;
	char *log_info;
} slog_event_t;


/* -------------------------------------------------------------------------- */
/* -------------- PUBLIC FUNCTIONS PROTOTYPES ------------------------------- */

void slog_event_buf_set(void *slog_buf, const char *tag, uint8_t tag_len,
			const char *file, uint8_t file_len, const char *func, uint8_t func_len, 
			uint32_t line, uint8_t level, const char *format, va_list args);

void slog_event_fifo_put(struct kfifo *fifo, void *slog_event_buf);

size_t slog_event_fifo_get(struct kfifo *fifo, void *slog_event_buf);


#endif /* __SLOG_EVENT_H */
/* ============== EOF ======================================================= */