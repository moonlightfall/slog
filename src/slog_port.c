

/* -------------------------------------------------------------------------- */
/* -------------- DEPENDANCIES ---------------------------------------------- */

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

#include "logger.h"
#include "slog_cfg.h"
#include "slog_port.h"
#include "slog_file.h"
#include "slog_tcp.h"


/* -------------------------------------------------------------------------- */
/* -------------- PRIVATE MACROS -------------------------------------------- */

/**
 * CSI(Control Sequence Introducer/Initiator) sign
 */
#define CSI_START                      "\033["
#define CSI_END                        "\033[0m"

/* output log front color */
#define F_BLACK                        "30;"
#define F_RED                          "31;"
#define F_GREEN                        "32;"
#define F_YELLOW                       "33;"
#define F_BLUE                         "34;"
#define F_MAGENTA                      "35;"
#define F_CYAN                         "36;"
#define F_WHITE                        "37;"

/* output log background color */
#define B_NULL
#define B_BLACK                        "40;"
#define B_RED                          "41;"
#define B_GREEN                        "42;"
#define B_YELLOW                       "43;"
#define B_BLUE                         "44;"
#define B_MAGENTA                      "45;"
#define B_CYAN                         "46;"
#define B_WHITE                        "47;"

/* output log fonts style */
#define S_BOLD                         "1m"
#define S_UNDERLINE                    "4m"
#define S_BLINK                        "5m"
#define S_NORMAL                       "22m"
/* output log default color definition: [front color] + [background color] + [show style] */

#define SLOG_COLOR_ASSERT              (F_MAGENTA B_NULL S_BOLD)
#define SLOG_COLOR_ERROR               (F_RED B_NULL S_BOLD)
#define SLOG_COLOR_WARN                (F_YELLOW B_NULL S_NORMAL)
#define SLOG_COLOR_INFO                (F_CYAN B_NULL S_NORMAL)
#define SLOG_COLOR_DEBUG               (F_GREEN B_NULL S_NORMAL)
#define SLOG_COLOR_VERBOSE             (F_BLUE B_NULL S_NORMAL)


/* -------------------------------------------------------------------------- */
/* -------------- PRIVATE VARIABLES ----------------------------------------- */

/* color output info */
static const char *color_output_info[] = {
        [ASSERT]  = SLOG_COLOR_ASSERT,
        [ERROR]   = SLOG_COLOR_ERROR,
        [WARN]    = SLOG_COLOR_WARN,
        [INFO]    = SLOG_COLOR_INFO,
        [DEBUG]   = SLOG_COLOR_DEBUG,
        [VERBOSE] = SLOG_COLOR_VERBOSE,
};


/* -------------------------------------------------------------------------- */
/* -------------- PUBLIC FUNCTIONS DEFINITION ------------------------------- */

/**
 * slog port initialize
 *
 * @return result
 */
int slog_port_init(void)
{
    /* close printf buffer */
    setbuf(stdout, NULL);

    if (0 != slog_file_init()) {
        return -1;
    }

    if (0 != slog_remote_init()) {
        slog_set_output_remote_enabled(false);
    }

    return 0;
}

void slog_port_deinit(void)
{
    slog_file_deinit();

    slog_remote_deinit();
}

/**
 * output log port interface
 *
 * @param log output of log
 * @param size log size
 */
void slog_port_output(int level, const char *log, size_t size)
{
    if (slog_get_output_terminal_enabled()) {
        /* output to terminal with color */
        printf(CSI_START"%s%.*s"CSI_END, color_output_info[level], (int)size, log);
    }

    if (slog_get_output_file_enabled()) {
        /* write the file */
        slog_file_write(log, size);
    }

    if (slog_get_output_remote_enabled()) {
        /* output to remote */
        if (-1 == slog_remote_write(log, size)) {
            slog_set_output_remote_enabled(false);
        }
    }
}


/* ============== EOF ======================================================= */