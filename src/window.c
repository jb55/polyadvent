

#include "gl.h"
#include "mat4/mat4.h"
#include "window.h"


void
handle_resize(float *camera, int width, int height) {
  printf("resizing %d %d\n", width, height);
  glViewport( 0, 0, width, height );
  mat4_perspective(100 /* fov */, (float)width / height, 1, 1000, camera);

  /* glMatrixMode( GL_PROJECTION ); //Switch to setting the camera perspective */
  /* Set the camera perspective */
  /* glLoadIdentity(); //reset the camera */
}
