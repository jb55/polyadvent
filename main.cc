
#include "gl.h"
#include "window.h"
#include "render.h"

int main(void)
{
  GLFWwindow* window;

  /* Initialize the library */
  if (!glfwInit())
    return -1;

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);
  glfwSetWindowSizeCallback(window, handle_resize);

  init_gl();

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f ); //clear background screen to black
    glClear( GL_COLOR_BUFFER_BIT );

    glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
    glLoadIdentity(); //Reset the drawing perspective

    render();

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

