
#include "texture.h"
#include "common.h"
#include "gl.h"
#include "stb_image.h"
#include "util.h"


#include "texture.h"
#include "common.h"

u32 create_cubemap(const char *faces[6]) {
    u32 tid;
    int width, height, n_channels;
    glGenTextures(1, &tid);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tid);

    for (u32 i = 0; i < 6; i++) {
        const char *texture = faces[i];
        u8 *data = stbi_load(texture, &width, &height, &n_channels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width,
                         height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            printf("cubemap texture failed to load: %s\n", texture);
            assert(0);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return tid;
}


