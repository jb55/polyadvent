
#include "gl.h"
#include "game.h"
#include "slab.h"
#include "file.h"
#include "update.h"
#include "window.h"
#include "slab_geom.h"
#include "slab.h"
#include "geometry.h"
#include "event.h"
#include "render.h"
#include "terrain.h"
#include "util.h"
#include <assert.h>
#include <time.h>
#include "poisson.h"
#include "uniform.h"
#include "ply.h"
#include "fbo.h"
#include "hires.h"


int main(void)
{
    int nsamples;

    int seed = time(NULL);
    srand(seed);

    struct game game;
    game.seed = seed;

    /* SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES); */
    int width = 640;
    int height = 480;

    SDL_Window *window = SDL_CreateWindow(
        "SDL2/OpenGL Demo", 0, 0, width, height,
        SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);

    SDL_GL_CreateContext(window);
    game_init(&game, width, height);

    check_gl();
    double last = hires_time_in_seconds();
    static float depth_mvp[MAT4_ELEMS];
    mat4_id(depth_mvp);

    struct render_config fbo_render_config = {
      .draw_ui = 0,
      .is_depth_pass = 1,
      .camera = game.test_resources.sun_camera.mat,
      .projection = game.test_resources.proj_ortho,
      .depth_mvp = depth_mvp
    };

    struct render_config default_config = {
      .draw_ui = 0,
      .is_depth_pass = 0,
      .camera = game.test_resources.camera.mat,
      .projection = game.test_resources.proj_persp,
      .depth_mvp = depth_mvp
    };

    while (1) {
        process_events(&game, game.test_resources.proj_persp);
        double new_time = hires_time_in_seconds();
        double frame_time = new_time - last;
        game.dt = frame_time;
        update(&game);

        /* while (accumulator >= dt) { */
        /*     t += dt; */
        /*     accumulator -= dt; */
        /* } */

        last = new_time;

        GLuint texture = game.test_resources.shadow_buffer.attachments[0];
        struct fbo *fbo = &game.test_resources.shadow_buffer;
        check_fbo(fbo);
        bind_fbo(fbo);
        /* glDrawBuffer(GL_NONE); */
        render(&game, &fbo_render_config);
        unbind_fbo(&game.test_resources.shadow_buffer);
        render(&game, &default_config);

        /* Swap front and back buffers */
        SDL_GL_SwapWindow(window);
    }

    /* free(slab_buffer); */
    /* free_slab_geom(&geom, NULL) */

    //SDL_GL_DeleteContext(gl);
    //return 0;
}

