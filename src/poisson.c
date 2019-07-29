
#include <stdlib.h>
#include <math.h>
#include "util.h"
#include "common.h"
#include "poisson.h"


struct grid_info {
  double size;
  int *grid;
  int *active;
  int cellw;
  int cells;
  float cell_size;
  struct point *samples;
  int n_samples;
  int n_active;
};

void
draw_samples(struct point *samples, double point_dist, const int nsamples, const int size) {
  const int dimx = size, dimy = size;
  int x, y;
  int close = 0;
  FILE *fp = fopen("poisson.ppm", "wb"); /* b - binary mode */
  fprintf(fp, "P6\n%d %d\n255\n", dimx, dimy);
  for (y = 0; y < dimy; ++y)
  for (x = 0; x < dimx; ++x) {
    static unsigned char color[3];
    double dx, dy, dist;
    for (int i = 0; i < nsamples; ++i) {
      dx = samples[i].x - (double)x;
      dy = samples[i].y - (double)y;
      dist = dx * dx + dy * dy;
      if (dist <= 2) {
        close = 1;
        break;
      }
    }
    /* if (fmod(x, point_dist/sqrt(2)) <= 1 || */
    /*     fmod(y, point_dist/sqrt(2)) <= 1) { */
    /*   color[0] = 180; */
    /*   color[1] = 180; */
    /*   color[2] = 180; */
    /* } */
    if (close) {
      int c = 255-(int)(dist/(point_dist/sqrt(2)) * 255.0);
      color[0] = c;
      color[1] = c;
      color[2] = c;
    }
    else {
      color[0] = 0;
      color[1] = 0;
      color[2] = 0;
    }
    fwrite(color, 1, 3, fp);
    close = 0;
  }
  fclose(fp);
}

int
grid_lookup(struct grid_info *info, int x, int y) {
  if (x > info->cellw || y > info->cellw || x < 0 || y < 0)
    return -2;

  int grid_ix = x * info->cellw + y;
  assert(grid_ix < info->cells && "blah");
  int ix = info->grid[grid_ix];

  return ix;
}

void
save_samples(struct point *samples, int seed, int n_samples) {
  FILE *fp = fopen("samples.bin", "wb"); /* b - binary mode */
  fwrite(&seed, sizeof(seed), 1, fp);
  fwrite(&n_samples, sizeof(n_samples), 1, fp);
  fwrite(samples, sizeof(*samples), n_samples, fp);
  fclose(fp);
}

struct point *
load_samples(int *seed, int *n_samples) {
  FILE *fp = fopen("data/samples-200x200.bin", "rb");
  int res;
  (void)res;
  int localseed;
  seed = seed ? seed : &localseed;
  res = fread(seed, sizeof(*seed), 1, fp);
  assert(res);
  res = fread(n_samples, sizeof(*n_samples), 1, fp);
  assert(res);
  struct point *samples = malloc(*n_samples * sizeof(*samples));
  res = fread(samples, sizeof(*samples), *n_samples, fp);
  assert(res);
  fclose(fp);
  return samples;
}

static int
add_sample(struct grid_info *info, struct point *candidate) {
  int grid_x = floor(candidate->x / info->cell_size);
  int grid_y = floor(candidate->y / info->cell_size);

  int i = info->n_samples++;
  int grid_ix = grid_ix = grid_x * info->cellw + grid_y;
  int ai = info->n_active++;

  assert(info->n_samples < info->cells);
  assert(grid_ix < info->cells);
  assert(ai < info->cells);

  info->active[ai] = i;
  info->samples[i].x = candidate->x;
  info->samples[i].y = candidate->y;
  assert(info->grid[grid_ix] == -1);
  info->grid[grid_ix] = i;
  return i;
}

static void
remove_active(struct grid_info *info, int i) {
  if (info->n_active == 0)
    return;

  if (info->n_active == 1 && i == 0) {
    info->n_active = 0;
    return;
  }

  if (info->n_active == 1 && i != 0)
    assert(!"trying to remove and index that doesn't exist");

  for (int j = i; j < info->n_active-1; ++j) {
    assert(j >= 0);
    assert(j+1 < info->cells);
    info->active[j] = info->active[j+1];
  }

  info->n_active--;
}

struct point *
poisson_disk_samples(const double point_dist, double size,
                     const int reject_limit, int *n_samples) {
  struct point candidate;
  struct grid_info info;

  double cell_size = point_dist / sqrt(2);

  int cellw = ceil(size/cell_size)+1;
  int cells = cellw * cellw;

  int *grid = malloc(cells * sizeof(*grid));
  int *active = malloc(cells * sizeof(*active));
  struct point *samples = malloc(cells * sizeof(*samples));

  info.n_samples = 0;
  info.grid = grid;
  info.samples = samples;
  info.active = active;
  info.n_active = 0;
  info.cellw = cellw;
  info.cells = cells;
  info.size = size;
  info.cell_size = cell_size;

  for (int i = 0; i < cells; ++i)
    grid[i] = -1;

  candidate.x = rand_0to1() * size;
  candidate.y = rand_0to1() * size;

  add_sample(&info, &candidate);

  assert(info.n_active == 1);
  assert(info.n_samples == 1);

  int nexti = -1;
  while (info.n_active > 0) {
    int rand_i = nexti != -1 ? nexti : rand() % info.n_active;
    struct point *xi = &samples[rand_i];

    int all_rejected = 1;

    for (int i = 0, tries = 0; i < reject_limit + tries; ++i) {
      int sx = rand() % 2 ? 1 : -1;
      int sy = rand() % 2 ? 1 : -1;

      double rx = ((rand_0to1() * point_dist) + point_dist) * sx;
      double ry = ((rand_0to1() * point_dist) + point_dist) * sy;

      /* printf("xi %f xy %f rx %f ry %f\n", xi->x, xi->y, rx, ry); */

      candidate.x = xi->x + rx;
      candidate.y = xi->y + ry;

      int cx = floor(candidate.x / info.cell_size);
      int cy = floor(candidate.y / info.cell_size);

      if (candidate.x > size || candidate.x < 0 || candidate.y > size || candidate.y < 0) {
        tries++;
        continue;
      }

      int nearby[] = {
        grid_lookup(&info, cx - 1, cy - 1),
        grid_lookup(&info, cx + 0, cy - 1),
        grid_lookup(&info, cx + 1, cy - 1),

        grid_lookup(&info, cx - 1, cy + 0),
        grid_lookup(&info, cx + 0, cy + 0),
        grid_lookup(&info, cx + 1, cy + 0),

        grid_lookup(&info, cx - 1, cy + 1),
        grid_lookup(&info, cx + 0, cy + 1),
        grid_lookup(&info, cx + 1, cy + 1),
      };


      int found = 1;

      if (grid_lookup(&info, cx + 0, cy + 0) != -1) {
        found = 0;
      }
      else {
        for (size_t j = 0; j < ARRAY_SIZE(nearby); ++j) {
          int neari = nearby[j];

          if (neari < 0)
            continue;
          struct point *near = &samples[neari];

          assert(near);

          double dx = near->x - candidate.x;
          double dy = near->y - candidate.y;
          double dist = sqrt(dx * dx + dy * dy);
          if (dist < point_dist) {
            found = 0;
            break;
          }
        }
      }

      if (found) {
        all_rejected = 0;
        nexti = add_sample(&info, &candidate);
      }
    } // gen up to reject_limit samples

    if (all_rejected) {
      nexti = -1;
      remove_active(&info, rand_i);
    }
  }

  *n_samples = info.n_samples;

  free(grid);
  free(active);
  return samples;
}
