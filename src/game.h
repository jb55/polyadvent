
#ifndef PA_GAME_H
#define PA_GAME_H

/*
 * Global data used by our render callback:
 */
typedef struct TestResources {
    GLuint vertex_buffer, element_buffer;
    GLuint vertex_shader, fragment_shader, program;

    struct {
      GLint fade_factor;
    } uniforms;

    struct {
      GLint position;
    } attributes;

    GLfloat fade_factor;
} TestResources;


typedef struct GameState {
  int counter;
  TestResources test_resources;
} GameState;


#endif /* PA_GAME_H */
