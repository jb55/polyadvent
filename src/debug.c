
#include "debug.h"
#include <stdlib.h>
#include <stdio.h>

void show_info_log(GLuint shader) {
	GLint msgLen = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &msgLen);

	// The maxLength includes the NULL character
	char *buffer = malloc(msgLen);
	glGetShaderInfoLog(shader, msgLen, &msgLen, buffer);

  printf("shader error: (%d) %.*s\n", msgLen, msgLen, buffer);

  free(buffer);
}


