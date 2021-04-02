

#ifndef __SLOG_FILE_H
#define __SLOG_FILE_H


/* -------------------------------------------------------------------------- */
/* -------------- PUBLIC FUNCTIONS PROTOTYPES ------------------------------- */

int slog_file_init(void);

void slog_file_write(const char *log, size_t size);

void slog_file_deinit(void);


#endif  /* __SLOG_FILE_H */
/* ============== EOF ======================================================= */