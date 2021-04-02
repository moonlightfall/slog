

/* -------------------------------------------------------------------------- */
/* -------------- DEPENDANCIES ---------------------------------------------- */

#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

#include "log2.h"
#include "slog_buf.h"
#include "slog_fifo.h"
#include "slog_event.h"
#include "slog_inner.h"


/* -------------------------------------------------------------------------- */
/* -------------- PRIVATE MACROS -------------------------------------------- */

#define SLOG_BUF_SIZE                   (1024 * 1024 * 16)  /* 16MB */
#define SLOG_BUF_FREE_WAIT_TIME         (1000)  /* 50ms each time, 50s total */


/* -------------------------------------------------------------------------- */
/* -------------- PRIVATE TYPES --------------------------------------------- */

typedef struct slog_buf_s {
	char *log_buf;
	size_t log_buf_size;
	struct kfifo *log_fifo;
} slog_buf_t;


/* -------------------------------------------------------------------------- */
/* -------------- PRIVATE VARIABLES ----------------------------------------- */

static slog_buf_t slog_buf;


/* -------------------------------------------------------------------------- */
/* -------------- PUBLIC FUNCTIONS DEFINITION ------------------------------- */

/**
 * slog buffer initialize.
 *
 * @return result
 */
int slog_buffer_init(void)
{
    int result = -1;
    size_t log_size = 0;

    /* common shared mem */
	if (!is_power_of_2(SLOG_BUF_SIZE)) {
		log_size = roundup_pow_of_two(SLOG_BUF_SIZE);
	} else {
		log_size = SLOG_BUF_SIZE;
	}
	slog_buf.log_buf_size = log_size;

	slog_buf.log_buf = (char *)calloc(1, slog_buf.log_buf_size);
    if (NULL == slog_buf.log_buf) {
		slog_error_inner("calloc error");
        return -1;
    }

    slog_buf.log_fifo = malloc(sizeof(struct kfifo));
    if (NULL == slog_buf.log_fifo) {
		slog_error_inner("malloc error");
        return -1;
    }

	result = kfifo_init(slog_buf.log_fifo, slog_buf.log_buf, slog_buf.log_buf_size);

	return result;
}

void slog_buffer_put(void *slog_event_buf)
{
    slog_event_fifo_put(slog_buf.log_fifo, slog_event_buf);
}

size_t slog_buffer_get(void *slog_event_buf)
{
	return slog_event_fifo_get(slog_buf.log_fifo, slog_event_buf);
}

bool slog_buffer_is_empty(void)
{
	if (NULL == slog_buf.log_fifo) {
		return true;
	}

	return kfifo_is_empty(slog_buf.log_fifo);
}

void slog_buffer_deinit(void)
{
    int count = 0;

	while (!slog_buffer_is_empty()) {
		usleep(50000);
		if (++count >= SLOG_BUF_FREE_WAIT_TIME) {
			break;
		}
	}

    if (NULL != slog_buf.log_buf) {
		free(slog_buf.log_buf);
		slog_buf.log_buf = NULL;
	}

    if (NULL != slog_buf.log_fifo) {
		free(slog_buf.log_fifo);
		slog_buf.log_fifo = NULL;
	}
}


/* ============== EOF ======================================================= */