

/* -------------------------------------------------------------------------- */
/* -------------- DEPENDANCIES ---------------------------------------------- */

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"
#include "slog_cfg.h"
#include "slog_inner.h"


/* -------------------------------------------------------------------------- */
/* -------------- PRIVATE MACROS -------------------------------------------- */

#define LOG_CONF_LINE_LEN                   128
#define LOG_CONF_VALUE_MAX                  32


/* -------------------------------------------------------------------------- */
/* -------------- PRIVATE VARIABLES ----------------------------------------- */

/* slog config object */
static slog_cfg_t slog_cfg;


/* -------------------------------------------------------------------------- */
/* -------------- PRIVATE FUNCTIONS DEFINITION ------------------------------ */

/**
 * set log remote val to default
 */
static void slog_set_remote_default()
{
    slog_set_output_remote_enabled(false);
    slog_set_output_remote_socket(-1);
    memset(slog_cfg.remoter.host, 0, sizeof(slog_cfg.remoter.host) / sizeof(slog_cfg.remoter.host[0]));
    slog_set_output_remote_port(0);
}

/**
 * set log filter default value
 */
static void slog_set_filter_default(void)
{
    slog_set_filter_level(VERBOSE);
    slog_set_filter_tag("");
    slog_set_filter_kw("");
}

static int slog_get_config(char *ptr, char *linedata, char *result, int len)
{
    char *tmp = NULL, *end = NULL;
    int cplen = 0;
    memset(result, 0, len);
    if (strstr(linedata, ptr)) {
        tmp = strchr(linedata, '=');
        if (NULL != tmp) {
            end = strchr(tmp, ';');
            cplen = end - tmp;
            if (cplen != 1) {
                strncpy(result, tmp + 1, cplen - 1);
                result[cplen] = '\0';
            }
            return 0;
        }
    }

    return -1;
}

static int level_value_trans(const char *value)
{
    if (NULL == value) {
        slog_error_inner("arg value is null, set default level VERBOSE.");
        return VERBOSE;
    }

    int level = -1;

    if (!strncasecmp(value, "ASSERT", 6)) {
        level = ASSERT;
    } else if (!strncasecmp(value, "ERROR", 5)) {
        level = ERROR;
    } else if (!strncasecmp(value, "WARN", 4)) {
        level = WARN;
    } else if (!strncasecmp(value, "INFO", 4)) {
        level = INFO;
    } else if (!strncasecmp(value, "DEBUG", 5)) {
        level = DEBUG;
    } else if (!strncasecmp(value, "VERBOSE", 7)) {
        level = VERBOSE;
    } else {
        slog_error_inner("log config parameter level invalid, set default level VERBOSE.");
        level = VERBOSE;
    }

    return level;
}

/**
 * check parameter remote host ip address is valid or not.
 *
 * @param ip remote ip address
 * 
 * @return -1 invalid, 0 valid
 */
static int slog_config_remote_host_check(const char *ip)
{
    int dots = 0; /* counts of '.' */
    int setions = 0; /* sum of every section */

    /* '.' in first position is invalid */
    if (NULL == ip || *ip == '.') {
        return -1;
    }

    while (*ip) {
        if (*ip == '.') {
            dots++;
            if (setions >= 0 && setions <= 255) {
                setions = 0;
                ip++;
                continue;
            }
            return -1;
        } else if (*ip >= '0' && *ip <= '9') {
            setions = setions * 10 + (*ip - '0'); /* calculate section sum divided by '.' */
        } else {
            return -1;
        }
        ip++;
    }

    /* check the last section */
    if (setions >= 0 && setions <= 255) {
        if (dots == 3) {
            return 0;
        }
    }

    return -1;
}

/**
 * check parameter remote port is valid or not, range from 0 to 65535
 *
 * @param value port number
 * 
 * @return -1 invalid, 0 valid
 */
static int slog_config_remote_port_check(const char *value)
{
    if (NULL == value) {
        return -1;
    }

    int len = strlen(value);
    if (len <= 0 || len > SLOG_REMOTE_PORT_MAX_LEN) {
        return -1;
    }

    int i = 0;
    for (i = 0; i < len; ++i) {
        if (0 == isdigit(value[i])) {
            return -1;
        }
    }

    if (atoi(value) > 65535) {
        return -1;
    }

    return 0;
}

/**
 * check parameter cpu core, only numeric is valid, range from 0 to 999
 *
 * @param value cpu core number
 * 
 * @return -1 invalid, 0 valid
 */
static int slog_config_core_check(const char *value)
{
    if (NULL == value) {
        return -1;
    }

    int len = strlen(value);
    if (len > SLOG_CPU_CORE_MAX_LEN) {
        return -1;
    }

    int i = 0;
    for (i = 0; i < len; ++i) {
        if (0 == isdigit(value[i])) {
            return -1;
        }
    }

    return 0;
}


/* -------------------------------------------------------------------------- */
/* -------------- PUBLIC FUNCTIONS DEFINITION ------------------------------- */

/**
 * set output enable or disable
 *
 * @param enabled TRUE: enable FALSE: disable
 */
void slog_set_output_enabled(bool enabled)
{
    slog_cfg.output_enabled = enabled;
}

/**
 * get output is enable or disable
 *
 * @return enable or disable
 */
bool slog_get_output_enabled(void)
{
    return slog_cfg.output_enabled;
}

void slog_set_output_file_enabled(bool enabled)
{
    slog_cfg.output_file_enabled = enabled;
}

bool slog_get_output_file_enabled(void)
{
    return slog_cfg.output_file_enabled;
}

void slog_set_output_terminal_enabled(bool enabled)
{
    slog_cfg.output_terminal_enabled = enabled;
}

bool slog_get_output_terminal_enabled(void)
{
    return slog_cfg.output_terminal_enabled;
}

void slog_set_output_remote_enabled(bool enabled)
{
    slog_cfg.remoter.output_remote_enabled = enabled;
}

bool slog_get_output_remote_enabled(void)
{
    return slog_cfg.remoter.output_remote_enabled;
}

/**
 * set log filter's level
 *
 * @param level level
 */
void slog_set_filter_level(uint8_t level)
{
    slog_cfg.filter.level = level;
}

/**
 * get log filter's level
 */
uint8_t slog_get_filter_level(void)
{
    return slog_cfg.filter.level;
}

/**
 * set log filter's tag
 *
 * @param tag tag
 */
void slog_set_filter_tag(const char *tag)
{
    strncpy(slog_cfg.filter.tag, tag, SLOG_FILTER_TAG_MAX_LEN - 1);
    slog_cfg.filter.tag[SLOG_FILTER_TAG_MAX_LEN] = '\0';
}

char *slog_get_filter_tag(char *filter_tag)
{
    if (NULL == filter_tag) {
        /* log */
    }
    memcpy(filter_tag, slog_cfg.filter.tag, SLOG_FILTER_TAG_MAX_LEN);

    return filter_tag;
}

/**
 * set log filter's keyword
 *
 * @param keyword keyword
 */
void slog_set_filter_kw(const char *keyword)
{
    strncpy(slog_cfg.filter.keyword, keyword, SLOG_FILTER_KW_MAX_LEN - 1);
    slog_cfg.filter.keyword[SLOG_FILTER_KW_MAX_LEN - 1] = '\0';
}

/**
 * set log output remote host.
 *
 * @param host remote host ip address
 */
void slog_set_output_remote_host(const char *host)
{
    strncpy(slog_cfg.remoter.host, host, SLOG_REMOTE_HOST_MAX_LEN - 1);
    slog_cfg.remoter.host[SLOG_REMOTE_HOST_MAX_LEN - 1] = '\0';
}

/**
 * get log output remote host.
 */
void slog_get_output_remote_host(char *host)
{
    memcpy(host, slog_cfg.remoter.host, SLOG_REMOTE_HOST_MAX_LEN);
}

/**
 * set log output remote port.
 *
 * @param port remote port
 */
void slog_set_output_remote_port(unsigned int port)
{
    slog_cfg.remoter.port = port;
}

/**
 * get log output remote port.
 */
unsigned int slog_get_output_remote_port(void)
{
    return slog_cfg.remoter.port;
}

/**
 * set log output remote socket fd.
 *
 * @param fd socket fd
 */
void slog_set_output_remote_socket(int fd)
{
    slog_cfg.remoter.socket_fd = fd;
}

/**
 * get log output remote port.
 */
int slog_get_output_remote_socket(void)
{
    return slog_cfg.remoter.socket_fd;
}

/**
 * set log bind cpu core
 *
 * @param core_number cpu core number
 */
void slog_set_cpu_core(int core_number)
{
    slog_cfg.cpu_core = core_number;
}

/**
 * get log bind cpu core
 *
 */
int slog_get_cpu_core(void)
{
    return slog_cfg.cpu_core;
}

void slog_set_config_default()
{
    slog_set_output_enabled(true);
    slog_set_output_file_enabled(true);
    slog_set_output_terminal_enabled(true);

    slog_set_cpu_core(-1);

    slog_set_filter_default();

    slog_set_remote_default();
}

int slog_config_parse(void)
{
    FILE *fp = NULL;
    char linedata[LOG_CONF_LINE_LEN] = { 0 };
    char value[LOG_CONF_VALUE_MAX] = { 0 };
    int enable = 0, level = 0;

    memset(linedata, 0, LOG_CONF_LINE_LEN);
    fp = fopen(LOG_CONFIG_FILE, "r");
	if (!fp) {
        slog_error_inner("open log config file %s error: %s", LOG_CONFIG_FILE, strerror(errno));
		return -1;
	}

    while (fgets(linedata, sizeof(linedata) - 1, fp)) {
        // enable setting
        if (0 == slog_get_config("OUTPUT_ENABLE", linedata, value, LOG_CONF_VALUE_MAX)) {
            if (0 == strncasecmp(value, "false", 5)) {
                enable = 0;
            } else if (0 == strncasecmp(value, "true", 4)) {
                enable = 1;
            } else {
                slog_error_inner("log config get parameter OUTPUT_ENABLE error, set default true.");
                enable = 1;
            }
            slog_set_output_enabled(enable);
        }
        if (0 == slog_get_config("OUTPUT_FILE_ENABLE", linedata, value, LOG_CONF_VALUE_MAX)) {
            if (0 == strncasecmp(value, "false", 5)) {
                enable = 0;
            } else if (0 == strncasecmp(value, "true", 4)) {
                enable = 1;
            } else {
                slog_error_inner("log config get parameter OUTPUT_FILE_ENABLE error, set default true.");
                enable = 1;
            }
            slog_set_output_file_enabled(enable);
        }
        if (0 == slog_get_config("OUTPUT_TERMINAL_ENABLE", linedata, value, LOG_CONF_VALUE_MAX)) {
            if (0 == strncasecmp(value, "false", 5)) {
                enable = 0;
            } else if (0 == strncasecmp(value, "true", 4)) {
                enable = 1;
            } else {
                slog_error_inner("log config get parameter OUTPUT_TERMINAL_ENABLE error, set default true.");
                enable = 1;
            }
            slog_set_output_terminal_enabled(enable);
        }
        // filter setting
        if (0 == slog_get_config("FILTER_KEYWORD", linedata, value, LOG_CONF_VALUE_MAX)) {
            if (strlen(value) > SLOG_FILTER_KW_MAX_LEN) {
                slog_error_inner("log config parameter FILTER_KEYWORD is too long.");
            }
            slog_set_filter_kw(value);
        }
        if (0 == slog_get_config("FILTER_LEVEL", linedata, value, LOG_CONF_VALUE_MAX)) {
            level = level_value_trans(value);
            slog_set_filter_level(level);
        }
        if (0 == slog_get_config("FILTER_TAG", linedata, value, LOG_CONF_VALUE_MAX)) {
            if (strlen(value) > SLOG_FILTER_TAG_MAX_LEN) {
                slog_error_inner("log config parameter FILTER_TAG is too long.");
            }
            slog_set_filter_tag(value);
        }
        if (0 == slog_get_config("CPU_CORE", linedata, value, LOG_CONF_VALUE_MAX)) {
            if (strlen(value) == 0) {
                slog_debug_inner("log config parameter CPU_CORE is not set.");
                continue;
            }
            if (slog_config_core_check(value) == 0) {
                slog_set_cpu_core(atoi(value));
            } else {
                slog_error_inner("log config parameter CPU_CORE: %s invalid.", value);
            }
        }
        if (0 == slog_get_config("OUTPUT_REMOTE_ENABLE", linedata, value, LOG_CONF_VALUE_MAX)) {
            if (0 == strncasecmp(value, "false", 5)) {
                enable = 0;
            } else if (0 == strncasecmp(value, "true", 4)) {
                enable = 1;
            } else {
                slog_error_inner("log config get parameter OUTPUT_REMOTE_ENABLE error, set default false.");
                enable = 0;
            }
            slog_set_output_remote_enabled(enable);
        }
        if (0 == slog_get_config("OUTPUT_REMOTE_HOST", linedata, value, LOG_CONF_VALUE_MAX)) {
            if (slog_config_remote_host_check(value) == 0) {
                slog_set_output_remote_host(value);
            } else {
                slog_error_inner("log config parameter OUTPUT_REMOTE_HOST: %s invalid, set output remote false.", value);
                slog_set_output_remote_enabled(false);
            }
        }
        if (0 == slog_get_config("OUTPUT_REMOTE_PORT", linedata, value, LOG_CONF_VALUE_MAX)) {
            if (slog_config_remote_port_check(value) == 0) {
                slog_set_output_remote_port(atoi(value));
            } else {
                slog_error_inner("log config parameter OUTPUT_REMOTE_PORT: %s invalid, set output remote false.", value);
                slog_set_output_remote_enabled(false);
            }
        }
    }

    if (fp) {
        fclose(fp);
    }

    return 0;
}


/* ============== EOF ======================================================= */