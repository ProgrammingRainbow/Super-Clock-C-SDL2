#ifndef CLOCK_H
#define CLOCK_H

#include "main.h"

struct Digit {
        SDL_Rect rect;
};

struct Clock {
        SDL_Renderer *renderer;
        SDL_Surface *target_surf;
        SDL_Texture *images[2];
        struct Digit digits[DIGITS_LENGTH];
};

bool clock_new(struct Clock **clock, SDL_Renderer *renderer);
void clock_free(struct Clock **clock);
void clock_update_layout(struct Clock *c);
void clock_draw(const struct Clock *c);

#endif
