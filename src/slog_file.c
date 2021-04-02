
/* -------------------------------------------------------------------------- */
/* -------------- DEPENDANCIES ---------------------------------------------- */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"
#include "slog_cfg.h"
#include "slog_file.h"


/* -------------------------------------------------------------------------- */
/* -------------- PRIVATE MACROS -------------------------------------------- */

#ifdef linux
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x)   (x)
#define unlikely(x) (x)
#endif

/* log file max size */
#define SLOG_FILE_MAX_SIZE                   (10 * 1024 * 1024)  /* 10MB */

/* log file max rotate file count */
#define SLOG_FILE_MAX_ROTATE                 5

#define SUFFIX_LEN                           10

#define FILE_PATH_SIZE                       512


/* -------------------------------------------------------------------------- */
/* -------------- PRIVATE TYPES --------------------------------------------- */

typedef struct slog_file_cfg_s {
    char *name;              /* file name */
    size_t max_size;         /* file max size */
    short max_rotate;        /* max rotate file count */
} slog_file_cfg_t;


/* -------------------------------------------------------------------------- */
/* -------------- PRIVATE VARIABLES ----------------------------------------- */

static FILE *fp = NULL;
static int fd = -1;
static slog_file_cfg_t local_cfg;


/* -------------------------------------------------------------------------- */
/* -------------- PRIVATE FUNCTIONS DEFINITION ------------------------------ */

static char *get_self_directory(char *buf, int count)
{
    int i;

    int rslt = readlink("/proc/self/exe", buf, count - 1);
    if (rslt < 0 || (rslt >= count - 1)) {
        return NULL;
    }

    buf[rslt] = '\0';
    for (i = rslt; i >= 0; i--) {
        if (buf[i] == '/') {
            buf[i + 1] = '\0';
            break;
        }
    }
    return buf;
}

static int slog_file_config(slog_file_cfg_t *cfg)
{
    local_cfg.name = cfg->name;
    local_cfg.max_size = cfg->max_size;
    local_cfg.max_rotate = cfg->max_rotate;

    fp = fopen(local_cfg.name, "a+");
    if (fp) {
        fd = fileno(fp);
        return 0;
    } else {
        fd = -1;
        return -1;
    }
}

/*
 * Reopen file
 */
static bool slog_file_reopen(void)
{
    FILE *tmp_fp;

    tmp_fp = fopen(local_cfg.name, "a+");
    if (tmp_fp) {
        if (fp) {
            fclose(fp);
        }

        fp = tmp_fp;
        fd = fileno(fp);
        return true;
    }

    return false;
}

/*
 * rotate the log file xxx.log.n-1 => xxx.log.n, and xxx.log => xxx.log.0
 */
static void slog_file_rotate(void)
{
    /* mv xxx.log.n-1 => xxx.log.n, and xxx.log => xxx.log.0 */
    short n;
    char oldpath[256], newpath[256];
    size_t base = strlen(local_cfg.name);

    memcpy(oldpath, local_cfg.name, base);
    memcpy(newpath, local_cfg.name, base);

    for (n = local_cfg.max_rotate - 1; n >= 0; --n) {
        snprintf(oldpath + base, SUFFIX_LEN, n ? ".%hd" : "", n - 1);
        snprintf(newpath + base, SUFFIX_LEN, ".%hd", n);
        rename(oldpath, newpath);
    }
}

/*
 * Check if it needed retate
 */
static bool slog_file_retate_check(void)
{
    struct stat statbuf;
    statbuf.st_size = 0;
    if (stat(local_cfg.name, &statbuf) < 0) {
        return false;
    }

    if ((unsigned int)statbuf.st_size > local_cfg.max_size) {
        return true;
    }

    return false;
}


/* -------------------------------------------------------------------------- */
/* -------------- PUBLIC FUNCTIONS DEFINITION ------------------------------- */

int slog_file_init(void)
{
    if (!slog_get_output_file_enabled()) {
        return 0;
    }

    int result = 0;
    slog_file_cfg_t cfg;
    char path[FILE_PATH_SIZE] = { 0 };
    char *log_path = NULL;

    log_path = get_self_directory(path, FILE_PATH_SIZE);
    if (log_path) {
        strcat(path, SLOG_FILE_NAME);
        cfg.name = path;
    } else {
        cfg.name = SLOG_FILE_NAME;
    }

    cfg.name = SLOG_FILE_NAME;
    cfg.max_size = SLOG_FILE_MAX_SIZE;
    cfg.max_rotate = SLOG_FILE_MAX_ROTATE;

    result = slog_file_config(&cfg);

    return result;
}

void slog_file_write(const char *log, size_t size)
{
    if (NULL == log) {
        return;
    }

    struct stat statbuf;
    statbuf.st_size = 0;

    fstat(fd, &statbuf);

    if (unlikely((unsigned int)statbuf.st_size > local_cfg.max_size)) {
#if SLOG_FILE_MAX_ROTATE > 0
	    if (slog_file_retate_check()) {
            /* rotate the log file */
            slog_file_rotate();
        }

        if (!slog_file_reopen()) {
            return;
        }
#else
        return ;
#endif
    }

    fwrite(log, size, 1, fp);

    fflush(fp);
    fsync(fd);
}

void slog_file_deinit(void)
{
    if (NULL != fp) {
        fflush(fp);
        fsync(fd);

        fclose(fp);
        fp = NULL;
    }
}


/* ============== EOF ======================================================= */