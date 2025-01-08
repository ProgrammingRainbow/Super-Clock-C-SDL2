#ifndef GAME_H
#define GAME_H

#include "main.h"
#include "clock.h"

struct Game {
        SDL_Event event;
        SDL_Window *window;
        SDL_Renderer *renderer;
        struct Clock *clock;
        bool is_running;
};

bool game_new(struct Game **game);
void game_free(struct Game **game);
void game_run(struct Game *g);

#endif
