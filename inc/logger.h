#ifndef __LOGGER_H
#define __LOGGER_H

#ifdef __cplusplus                     
extern "C" {
#endif

/* -------------------------------------------------------------------------- */
/* -------------- DEPENDANCIES ---------------------------------------------- */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>

/* -------------------------------------------------------------------------- */
/* -------------- PUBLIC MACROS --------------------------------------------- */

/* Log configuration file name */
#define LOG_CONFIG_FILE             "slog.conf"

/* Log storage file name */
#define SLOG_FILE_NAME              "slog.log"

/* Filename without path */
#define __FILENAME__                (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1) : (__FILE__))

/* output log's level */
#define ASSERT                      0
#define ERROR                       1
#define WARN                        2
#define INFO                        3
#define DEBUG                       4
#define VERBOSE                     5


/* -------------------------------------------------------------------------- */
/* -------------- PUBLIC FUNCTIONS PROTOTYPES ------------------------------- */

/**
 * init log
 */
int log_init(void);

/**
 * fini log
 */
void log_fini(void);

#define slog_assert(tag, fmt, ...)   do \
{\
        slog(ASSERT, (const char *)tag, sizeof(tag) - 1, __FILENAME__, strlen(__FILENAME__), \
        __func__, sizeof(__func__) - 1, __LINE__, (const char*)fmt, ##__VA_ARGS__); \
}while(0)

#define slog_error(tag, fmt, ...)   do \
{\
        slog(ERROR, (const char *)tag, sizeof(tag) - 1, __FILENAME__, strlen(__FILENAME__), \
        __func__, sizeof(__func__) - 1, __LINE__, (const char*)fmt, ##__VA_ARGS__); \
}while(0)

#define slog_warn(tag, fmt, ...)   do \
{\
        slog(WARN, (const char *)tag, sizeof(tag) - 1, __FILENAME__, strlen(__FILENAME__), \
        __func__, sizeof(__func__) - 1, __LINE__, (const char*)fmt, ##__VA_ARGS__); \
}while(0)

#define slog_info(tag, fmt, ...)   do \
{\
        slog(INFO, (const char *)tag, sizeof(tag) - 1, __FILENAME__, strlen(__FILENAME__), \
        __func__, sizeof(__func__) - 1, __LINE__, (const char*)fmt, ##__VA_ARGS__); \
}while(0)

#define slog_debug(tag, fmt, ...)   do \
{\
        slog(DEBUG, (const char *)tag, sizeof(tag) - 1, __FILENAME__, strlen(__FILENAME__), \
        __func__, sizeof(__func__) - 1, __LINE__, (const char*)fmt, ##__VA_ARGS__); \
}while(0)

#define slog_verbose(tag, fmt, ...)   do \
{\
        slog(VERBOSE, (const char *)tag, sizeof(tag) - 1, __FILENAME__, strlen(__FILENAME__), \
        __func__, sizeof(__func__) - 1, __LINE__, (const char*)fmt, ##__VA_ARGS__); \
}while(0)


void slog(uint8_t level, const char *tag, size_t tag_len, const char *file, size_t file_len, \
          const char *func, size_t func_len, long line, const char *format, ...);


#ifdef __cplusplus
}
#endif


#endif  /* __LOGGER_H */
/* ============== EOF ======================================================= */
