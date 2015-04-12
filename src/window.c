

#include "gl.h"
#include "window.h"

void
handle_resize(int width, int height) {
  glViewport( 0, 0, width, height );

  //glMatrixMode( GL_PROJECTION ); //Switch to setting the camera perspective
  //Set the camera perspective
  //glLoadIdentity(); //reset the camera
}
