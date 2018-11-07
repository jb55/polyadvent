
#ifndef TEXTURE_H
#define TEXTURE_H

#define TEXTURE(f) "data/textures/" f
#define CUBEMAP(f) TEXTURE("cubemaps/" f)

unsigned int create_cubemap(const char *faces[6]);
// right left top bottom front back

#endif /* TEXTURE_H */
