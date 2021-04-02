
/* -------------------------------------------------------------------------- */
/* -------------- DEPENDANCIES ---------------------------------------------- */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>

#include "slog_spec.h"
#include "slog_inner.h"


/* -------------------------------------------------------------------------- */
/* -------------- PRIVATE MACROS -------------------------------------------- */

#define SLOG_INNER_FILE_NAME              "log_inner.log"


/* -------------------------------------------------------------------------- */
/* -------------- PUBLIC FUNCTIONS DEFINITION ------------------------------- */

void slog_inner(uint8_t level, const char *file, long line, const char *format, ...)
{
	va_list args;
	FILE *fp = NULL;

    char cur_system_time[SLOG_TIME_FORMAT_LEN] = { 0 };
	struct timeval current_time;
    gettimeofday(&current_time, NULL);
    struct tm *p = NULL;
    time_t timep = current_time.tv_sec;
    p = localtime(&timep);
    if (p == NULL) {
        return;
    }

    snprintf(cur_system_time, SLOG_TIME_FORMAT_LEN - 1, "%04d-%02d-%02d %02d:%02d:%02d.%06ld", 
        p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, current_time.tv_usec);

	switch (level) {
	case DEBUG:
		fp = fopen(SLOG_INNER_FILE_NAME, "a");
		if (!fp) return;

		fprintf(fp, "%s DEBUG (%s:%ld) ", cur_system_time, file, line);
		break;
	case WARN:
		fp = fopen(SLOG_INNER_FILE_NAME, "a");
		if (!fp) return;

		fprintf(fp, "%s WARN  (%s:%ld) ", cur_system_time, file, line);
		break;
	case ERROR:
		fp = fopen(SLOG_INNER_FILE_NAME, "a");
		if (!fp) return;

		fprintf(fp, "%s ERROR (%s:%ld) ", cur_system_time, file, line);
		break;
	}

	/* writing file twice(time & msg) is not atomic
	 * may cause cross
	 * but avoid log size limit */
	va_start(args, format);
	vfprintf(fp, format, args);
	va_end(args);
	fprintf(fp, "\n");

	fclose(fp);
	return;
}


/* ============== EOF ======================================================= */