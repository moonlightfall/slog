
#ifndef __SLOG_CFG_H
#define __SLOG_CFG_H

#ifdef __cplusplus                     
extern "C" {
#endif

/* -------------------------------------------------------------------------- */
/* -------------- DEPENDANCIES ---------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>


/* -------------------------------------------------------------------------- */
/* -------------- PUBLIC MACROS --------------------------------------------- */

/* output filter's tag max length */
#define SLOG_FILTER_TAG_MAX_LEN              16

/* output filter's keyword max length */
#define SLOG_FILTER_KW_MAX_LEN               16

#define SLOG_REMOTE_HOST_MAX_LEN             16
#define SLOG_REMOTE_PORT_MAX_LEN             5
#define SLOG_CPU_CORE_MAX_LEN                3


/* -------------------------------------------------------------------------- */
/* -------------- PUBLIC TYPES ---------------------------------------------- */

/* output log's filter */
typedef struct slog_filter_s {
    uint8_t level;
    char tag[SLOG_FILTER_TAG_MAX_LEN + 1];
    char keyword[SLOG_FILTER_KW_MAX_LEN + 1];
} slog_filter_t;

typedef struct slog_remote_s {
    bool output_remote_enabled;
    int socket_fd;
    char host[SLOG_REMOTE_HOST_MAX_LEN];
    unsigned int port;
} slog_remote_t;

typedef struct slog_cfg_s {
    bool output_enabled;
    bool output_file_enabled;
    bool output_terminal_enabled;
    int cpu_core;
    slog_filter_t filter;
    slog_remote_t remoter;
} slog_cfg_t;

/* -------------------------------------------------------------------------- */
/* -------------- PUBLIC FUNCTIONS PROTOTYPES ------------------------------- */

void slog_set_output_enabled(bool enabled);
bool slog_get_output_enabled(void);

void slog_set_output_file_enabled(bool enabled);
bool slog_get_output_file_enabled(void);

void slog_set_output_terminal_enabled(bool enabled);
bool slog_get_output_terminal_enabled(void);

void slog_set_output_remote_enabled(bool enabled);
bool slog_get_output_remote_enabled(void);

void slog_set_filter_level(uint8_t level);
uint8_t slog_get_filter_level(void);

void slog_set_filter_tag(const char *tag);
char *slog_get_filter_tag(char *filter_tag);

void slog_set_filter_kw(const char *keyword);

void slog_set_output_remote_host(const char *host);
void slog_get_output_remote_host(char *host);

void slog_set_output_remote_port(unsigned int port);
unsigned int slog_get_output_remote_port(void);

void slog_set_output_remote_socket(int fd);
int slog_get_output_remote_socket(void);

void slog_set_cpu_core(int core_number);
int slog_get_cpu_core(void);

void slog_set_config_default();

int slog_config_parse(void);


#ifdef __cplusplus
}
#endif


#endif /* __SLOG_CFG_H */
/* ============== EOF ======================================================= */