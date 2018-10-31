
#include <time.h>
#include "common.h"
#include <SDL2/SDL.h>

double hires_time_in_seconds() {
    double ticks_per_sec = (double)SDL_GetPerformanceFrequency();
    double ticks = (double)SDL_GetPerformanceCounter();
    return ticks / ticks_per_sec;
}
