

#ifndef PLYPARSER_H
#define PLYPARSER_H

typedef void (vertex_cb)(void *context, int nverts, float *vert, float *norm, unsigned char *color);
typedef void (index_cb)(void *context, int ninds, int *indices);

int parse_ply(const char *filename, void *context, vertex_cb *cb);

#endif /* PLYPARSER_H */
