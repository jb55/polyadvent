
#ifndef POLYADVENT_DEBUG_H
#define POLYADVENT_DEBUG_H

#include "gl.h"

void show_info_log(GLuint shader);

// rtassert exists in release builds, use sparingly
#define rtassert(cond, msg) if (!cond) { \
        fprintf(stderr, "FATAL %s:%d %s\n", __FILE__, __LINE__, msg); \
        exit(1); \
    }

#define unusual(...) fprintf(stderr, "UNUSUAL: " __VA_ARGS__)

#ifdef DEBUG
#define debug(...) fprintf(stderr, __VA_ARGS__)
#else
#define debug(...)
#endif


#endif /* POLYADVENT_DEBUG_H */
