
#ifndef __SLOG_PORT_H
#define __SLOG_PORT_H

/* -------------------------------------------------------------------------- */
/* -------------- DEPENDANCIES ---------------------------------------------- */

#include <stddef.h>


/* -------------------------------------------------------------------------- */
/* -------------- PUBLIC FUNCTIONS PROTOTYPES ------------------------------- */

int slog_port_init(void);

void slog_port_deinit(void);

void slog_port_output(uint8_t level, const char *log, size_t size);


#endif  /* __SLOG_PORT_H */
/* ============== EOF ======================================================= */