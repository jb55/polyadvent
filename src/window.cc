

#include "gl.h"

void
handle_resize(GLFWwindow * window, int width, int height) {
  glViewport( 0, 0, width, height );

  glMatrixMode( GL_PROJECTION ); //Switch to setting the camera perspective
  //Set the camera perspective
  glLoadIdentity(); //reset the camera
}
