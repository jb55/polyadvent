
#ifndef POLYADVENT_DEBUG_H
#define POLYADVENT_DEBUG_H

#include "gl.h"

void show_info_log(GLuint shader);

#define unusual(...) fprintf(stderr, "UNUSUAL: " __VA_ARGS__)
#define debug(...) fprintf(stderr, "debug: " __VA_ARGS__)


#endif /* POLYADVENT_DEBUG_H */
