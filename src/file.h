#ifndef POLYADVEMT_FILE_H
#define POLYADVEMT_FILE_H

#include <time.h>

time_t file_mtime(const char *filename);
char *file_contents(const char *filename, size_t *length);

#endif /* POLYADVEMT_FILE_H */
