

#ifndef __SLOG_INNER_H
#define __SLOG_INNER_H

/* -------------------------------------------------------------------------- */
/* -------------- DEPENDANCIES ---------------------------------------------- */

#include "logger.h"


/* -------------------------------------------------------------------------- */
/* -------------- PUBLIC MACROS --------------------------------------------- */

#define slog_debug_inner(format, ...)   do \
{\
        slog_inner(DEBUG, __FILENAME__, __LINE__, (const char*)format, ##__VA_ARGS__); \
}while(0)

#define slog_warn_inner(format, ...)   do \
{\
        slog_inner(WARN, __FILENAME__, __LINE__, (const char*)format, ##__VA_ARGS__); \
}while(0)

#define slog_error_inner(format, ...)   do \
{\
        slog_inner(ERROR, __FILENAME__, __LINE__, (const char*)format, ##__VA_ARGS__); \
}while(0)

void slog_inner(uint8_t level, const char *file, long line, const char *format, ...);


#endif  /* __SLOG_INNER_H */
/* ============== EOF ======================================================= */