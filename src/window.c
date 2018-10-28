

#include "gl.h"
#include "mat4.h"
#include "window.h"
#include "game.h"
#include "update.h"


void
handle_resize(struct game *game, int width, int height) {
  /* printf("resizing %d %d\n", width, height); */
  glViewport( 0, 0, width, height );
  mat4_perspective(75 /* fov */, (float)width / height, 1, 20000,
                   game->test_resources.proj_persp);

  resize_fbos(game, width, height);

  /* glMatrixMode( GL_PROJECTION ); //Switch to setting the camera perspective */
  /* Set the camera perspective */
  /* glLoadIdentity(); //reset the camera */
}
