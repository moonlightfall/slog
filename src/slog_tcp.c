

/* -------------------------------------------------------------------------- */
/* -------------- DEPENDANCIES ---------------------------------------------- */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "slog_tcp.h"
#include "slog_cfg.h"
#include "slog_inner.h"

/* -------------------------------------------------------------------------- */
/* -------------- PUBLIC FUNCTIONS DEFINITION ------------------------------- */

/**
 * remote output mode initialize
 *
 * @return result
 */
int slog_remote_init(void)
{
    if (!slog_get_output_remote_enabled()) {
        return 0;
    }

    char host_address[SLOG_REMOTE_HOST_MAX_LEN] = { 0 };
    struct sockaddr_in host;
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == sockfd) {
        slog_error_inner("create socket error: %s", strerror(errno));
        return -1;
    }

    int flags = 0;
    if (-1 == (flags = fcntl(sockfd, F_GETFL, 0))) {
        flags = 0;
    }

    if (-1 == fcntl(sockfd, F_SETFL, flags | O_NONBLOCK)) {
        slog_error_inner("set remote socket nonblock failed: %s", strerror(errno));
    }

    slog_get_output_remote_host(host_address);

    bzero(&host, sizeof(host));
    host.sin_family = AF_INET;
    host.sin_port = htons(slog_get_output_remote_port());
    host.sin_addr.s_addr = inet_addr(host_address);

    if (-1 == connect(sockfd, (struct sockaddr *)&host, sizeof(host))) {
        slog_error_inner("connect %s:%d error: %s", host_address, slog_get_output_remote_port(), strerror(errno));
        return -1;
    }

    slog_set_output_remote_socket(sockfd);

    return 0;
}

/**
 * log remote output
 *
 * @return result
 */
int slog_remote_write(const char *log, size_t size)
{
    if (NULL == log || 0 == size) {
        return 0;
    }

    /* size - 1 means to ignore '\n' in the end */
    if (-1 == send(slog_get_output_remote_socket(), log, size - 1, 0)) {
        if (errno == EINTR) {
            return 0;
        }
        char host_address[SLOG_REMOTE_HOST_MAX_LEN] = { 0 };
        slog_get_output_remote_host(host_address);
        slog_error_inner("send message to remote: %s error: %s", host_address, strerror(errno));
        return -1;
    }

    return 0;
}

void slog_remote_deinit(void)
{
    if (-1 != slog_get_output_remote_socket()) {
        close(slog_get_output_remote_socket());
        slog_set_output_remote_socket(-1);
    }
}


/* ============== EOF ======================================================= */