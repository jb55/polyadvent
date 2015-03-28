
#ifndef POLYADVENT_DEBUG_H
#define POLYADVENT_DEBUG_H

#include "gl.h"

void
show_info_log(
  GLuint object,
  PFNGLGETSHADERIVPROC glGet__iv,
  PFNGLGETSHADERINFOLOGPROC glGet__InfoLog
);


#endif /* POLYADVENT_DEBUG_H */
