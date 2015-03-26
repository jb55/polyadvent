
#include "gl.h"


void
init_gl() {
  glEnable(GL_DEPTH_TEST);
}


void
render () {
  glBegin(GL_TRIANGLES); //Begin triangle coordinates

  //Triangle
  glVertex3f(-0.5f, 0.5f, -5.0f);
  glVertex3f(-1.0f, 1.5f, -5.0f);
  glVertex3f(-1.5f, 0.5f, -5.0f);

  glEnd(); //End triangle coordinates
}
