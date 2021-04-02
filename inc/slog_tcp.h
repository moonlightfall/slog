

#ifndef __SLOG_TCP_H
#define __SLOG_TCP_H


/* -------------------------------------------------------------------------- */
/* -------------- PUBLIC FUNCTIONS PROTOTYPES ------------------------------- */

int slog_remote_init(void);

void slog_remote_deinit(void);

int slog_remote_write(const char *log, size_t size);


#endif  /* __SLOG_TCP_H */
/* ============== EOF ======================================================= */