
#include "animation.h"
#include "event.h"
#include "fbo.h"
#include "file.h"
#include "game.h"
#include "debug.h"
#include "geometry.h"
#include "gl.h"
#include "hires.h"
#include "ply.h"
#include "poisson.h"
#include "quickhull.h"
#include "render.h"
#include "scene.h"
#include "slab_geom.h"
#include "slab.h"
#include "terrain.h"
#include "uniform.h"
#include "update.h"
#include "util.h"
#include "window.h"



#include <assert.h>
#include <time.h>

int main(void)
{
    int nsamples;

    int seed = time(NULL);
    debug("seed %d\n", seed);
    srand(seed);

    struct game game;
    game.seed = seed;

    /* SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES); */
    int width = 640;
    int height = 480;
    game_init(&game, width, height);
    /* reset_scene(&game); */
    entity_test_scene(&game);

    check_gl();
    double last = hires_time_in_seconds();
    static float depth_vp[MAT4_ELEMS];
    mat4_id(depth_vp);

    struct render_config fbo_render_config = {
      .draw_ui = 1,
      .is_depth_pass = 1,
      .camera = game.test_resources.sun_camera_id,
      .projection = game.test_resources.proj_ortho,
      .depth_vp = depth_vp
    };

    struct render_config default_config = {
      .draw_ui = 1,
      .is_depth_pass = 0,
      .camera = game.test_resources.camera_node_id,
      .projection = game.test_resources.proj_persp,
      .depth_vp = depth_vp
    };


    while (!game.quit) {
        game.frame++;
        process_events(&game.input, game.frame);
        if (game.input.resized_height) {
            /* printf("handling resize %d %d\n", game.input.resized_width, */
            /*        game.input.resized_height); */
            handle_resize(&game, game.input.resized_width, game.input.resized_height);
        }
        //default_config.camera = game.test_resources.camera_node->mat;
        double new_time = hires_time_in_seconds();
        double frame_time = new_time - last;
        game.dt = frame_time;

        update(&game);

        /* while (accumulator >= dt) { */
        /*     t += dt; */
        /*     accumulator -= dt; */
        /* } */

        last = new_time;

        struct fbo *fbo = &game.test_resources.shadow_buffer;
        check_fbo(fbo);
        bind_fbo(fbo);
        /* glDrawBuffer(GL_NONE); */
        render(&game, &fbo_render_config);
        unbind_fbo(&game.test_resources.shadow_buffer);
        render(&game, &default_config);

        /* Swap front and back buffers */
        SDL_GL_SwapWindow(game.window);
    }

    /* free(slab_buffer); */
    /* free_slab_geom(&geom, NULL) */

    //SDL_GL_DeleteContext(gl);
    //return 0;
}

