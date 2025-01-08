#include "game.h"
#include "init_sdl.h"

void game_events(struct Game *g);
void game_draw(struct Game *g);

bool game_new(struct Game **game) {
    *game = calloc(1, sizeof(struct Game));
    if (*game == NULL) {
        fprintf(stderr, "Error in calloc of new game.\n");
        return false;
    }
    struct Game *g = *game;

    g->is_running = true;

    if (!game_init_sdl(g)) {
        return false;
    }

    if (!clock_new(&g->clock, g->window, g->renderer)) {
        return false;
    }

    return true;
}

void game_free(struct Game **game) {
    if (*game) {

        clock_free(&(*game)->clock);

        SDL_DestroyRenderer((*game)->renderer);
        (*game)->renderer = NULL;

        SDL_DestroyWindow((*game)->window);
        (*game)->window = NULL;

        TTF_Quit();
        IMG_Quit();
        SDL_Quit();

        free(*game);
        *game = NULL;

        printf("all clean!\n");
    }
}

void game_events(struct Game *g) {
    while (SDL_PollEvent(&g->event)) {
        switch (g->event.type) {
        case SDL_QUIT:
            g->is_running = false;
            break;
        case SDL_KEYDOWN:
            switch (g->event.key.keysym.scancode) {
            case SDL_SCANCODE_ESCAPE:
                g->is_running = false;
                break;
            case SDL_SCANCODE_B:
                clock_toggle_size(g->clock);
                break;
            case SDL_SCANCODE_C:
                clock_toggle_color(g->clock);
                break;
            case SDL_SCANCODE_T:
                clock_toggle_text(g->clock);
                break;
            default:
                break;
            }
        default:
            break;
        }
    }
}

void game_draw(struct Game *g) {
    SDL_RenderClear(g->renderer);

    clock_draw(g->clock);

    SDL_RenderPresent(g->renderer);
}

void game_run(struct Game *g) {
    while (g->is_running) {

        game_events(g);

        clock_update(g->clock);

        game_draw(g);

        SDL_Delay(16);
    }
}
