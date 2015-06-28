
#include "util.h"

void check_gl() {
  unsigned int e = glGetError();
  if (e != GL_NO_ERROR) {
    printf("Got error from OpenGL: %d\n", e);
    assert(0);
  }
}
