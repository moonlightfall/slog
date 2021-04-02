
#ifndef __SLOG_BUF_H
#define __SLOG_BUF_H


/* -------------------------------------------------------------------------- */
/* -------------- PUBLIC FUNCTIONS PROTOTYPES ------------------------------- */

int slog_buffer_init(void);

void slog_buffer_put(void *slog_event_buf);

size_t slog_buffer_get(void *slog_event_buf);

bool slog_buffer_is_empty(void);

void slog_buffer_deinit(void);


#endif  /* __SLOG_BUF_H */
/* ============== EOF ======================================================= */