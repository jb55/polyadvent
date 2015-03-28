
#include "debug.h"
#include <stdlib.h>
#include <stdio.h>

void
show_info_log(
  GLuint object,
  PFNGLGETSHADERIVPROC glGet__iv,
  PFNGLGETSHADERINFOLOGPROC glGet__InfoLog
) {
  GLint log_length;
  char *log;

  glGet__iv(object, GL_INFO_LOG_LENGTH, &log_length);
  // TODO (jb55): remove mallocs?
  log = (char*)malloc(log_length);
  glGet__InfoLog(object, log_length, NULL, log);
  fprintf(stderr, "%s", log);
  free(log);
}
