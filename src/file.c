
#include <stdio.h>
#include <stdlib.h>
#include "file.h"

#include <sys/stat.h>

time_t file_mtime(const char *filename) {
	// TODO: windows file_mtime
	struct stat stats;
	stat(filename, &stats);

	return stats.st_mtime;
}

void *file_contents(const char *filename, size_t *length) {
  FILE *f = fopen(filename, "r");
  void *buffer;

  if (!f) {
      fprintf(stderr, "Unable to open %s for reading\n", filename);
      return NULL;
  }

  fseek(f, 0, SEEK_END);
  *length = (size_t)ftell(f);
  fseek(f, 0, SEEK_SET);

  buffer = malloc(*length+1);
  *length = fread(buffer, 1, *length, f);
  fclose(f);

  return buffer;
}
