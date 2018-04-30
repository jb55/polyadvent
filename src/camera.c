
#include "camera.h"
#include "util.h"
#include "mat4.h"
#include "vec3.h"

// should probably be a scene node
struct camera *camera_init(struct camera *cam) {
  return 0;
}


void
camera_follow(vec3 *cam_pos, vec3 *target_prev, vec3 *target, mat4 *cam) {
  cam_pos[0] = target[0];
  cam_pos[1] = target[1] - 20;
  cam_pos[2] = target[2] + 20;
  /* printf("cam %f %f %f looking at player %f %f %f\n", */
  /*        cam_pos[0], cam_pos[1], cam_pos[2], */
  /*        target[0], target[1], target[2]); */

  /* look_at(cam_pos, target, V3(0,1,0), cam); */
  /* cam_pos[0] = -target[0]; */
  /* cam_pos[1] = -target[1]; */
  /* cam_pos[2] = target[2]; */
}
