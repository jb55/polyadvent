

#include "gl.h"
#include "mat4.h"
#include "window.h"
#include "game.h"
#include "update.h"


void
handle_resize(struct game *game, int width, int height) {
  /* printf("resizing %d %d\n", width, height); */
  glViewport( 0, 0, width, height );
  mat4_perspective(75 /* fov */, (float)width / height, 0.1, 10000.0,
                   game->test_resources.proj_persp);

  resize_fbos(get_entity(&game->test_resources.player_id),
              &game->test_resources.shadow_buffer,
              game->test_resources.proj_ortho,
              width, height);

  /* glMatrixMode( GL_PROJECTION ); //Switch to setting the camera perspective */
  /* Set the camera perspective */
  /* glLoadIdentity(); //reset the camera */
}
