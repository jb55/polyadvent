
#ifndef POLYADVENT_POISSON_H
#define POLYADVENT_POISSON_H

struct point;

void draw_samples(struct point *samples, double point_dist,
                  const int nsamples, const int size);

struct point *
poisson_disk_samples(const double point_dist, double size,
                     const int reject_limit, int *n_samples);

struct point *
load_samples(int *seed, int *num_samples);

void
save_samples(struct point *samples, int seed, int n_samples);

#endif /* POLYADVENT_POISSON_H */
