
#include "common.h"
#include "util.h"

struct point *
uniform_samples(const int count, double size) {
  struct point *samples = malloc(sizeof(*samples) * count);

  for (int i = 0; i < count; ++i) {
    samples[i].x = rand_0to1() * size;
    samples[i].y = rand_0to1() * size;
  }

  return samples;
}


