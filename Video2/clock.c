#include "clock.h"

bool clock_generate_image(struct Clock *c, SDL_Color front_color,
                          SDL_Color back_color);
bool clock_generate_images(struct Clock *c);

bool clock_new(struct Clock **clock, SDL_Renderer *renderer) {
    *clock = calloc(1, sizeof(struct Clock));
    if (!*clock) {
        fprintf(stderr, "Error in calloc of clock!\n");
        return false;
    }
    struct Clock *c = *clock;

    c->renderer = renderer;

    for (int i = 0; i < DIGITS_LENGTH; i++) {
        c->digits[i].rect.w = DIGIT_SIZE;
        c->digits[i].rect.h = DIGIT_SIZE;
    }

    clock_update_layout(c);

    if (!clock_generate_images(c)) {
        return false;
    }

    return true;
}

void clock_free(struct Clock **clock) {
    if (*clock) {
        SDL_FreeSurface((*clock)->target_surf);
        (*clock)->target_surf = NULL;

        for (int i = 0; i < 2; i++) {
            SDL_DestroyTexture((*clock)->images[i]);
            (*clock)->images[i] = NULL;
        }

        (*clock)->renderer = NULL;

        free(*clock);
        *clock = NULL;
    }
}

void clock_update_layout(struct Clock *c) {

    Uint32 i = 0;
    int offset = DIGIT_OFFSET;

    for (int column = 0; column < 6; column++) {
        if (column > 3) {
            offset = 42;
        } else if (column > 1) {
            offset = 24;
        }
        for (int row = 0; row < 4; row++) {
            if (i < DIGITS_LENGTH) {
                c->digits[i].rect.x =
                    (column * (DIGIT_SIZE + DIGIT_OFFSET) + offset);
                c->digits[i].rect.y =
                    (row * (DIGIT_SIZE + DIGIT_OFFSET) + DIGIT_OFFSET);
                c->digits[i].rect.w = DIGIT_SIZE;
                c->digits[i].rect.h = DIGIT_SIZE;
                i++;
            }
        }
    }
}

bool clock_generate_image(struct Clock *c, SDL_Color front_color,
                          SDL_Color back_color) {

    if (c->target_surf) {
        SDL_FreeSurface(c->target_surf);
        c->target_surf = NULL;
    }

    c->target_surf =
        SDL_CreateRGBSurface(0, DIGIT_SIZE, DIGIT_SIZE, 32, 0, 0, 0, 255);
    if (!c->target_surf) {
        fprintf(stderr, "Error creating a target text surface: %s\n",
                SDL_GetError());
        return false;
    }

    if (SDL_FillRect(c->target_surf, NULL,
                     SDL_MapRGB(c->target_surf->format, back_color.r,
                                back_color.g, back_color.b)) != 0) {
        fprintf(stderr, "Error creating filled rect: %s\n", SDL_GetError());
        return false;
    }

    c->images[0] = SDL_CreateTextureFromSurface(c->renderer, c->target_surf);
    if (!c->images[0]) {
        fprintf(stderr, "Error creating a texture: %s\n", SDL_GetError());
        return false;
    }

    if (SDL_FillRect(c->target_surf, NULL,
                     SDL_MapRGB(c->target_surf->format, front_color.r,
                                front_color.g, front_color.b)) != 0) {
        fprintf(stderr, "Error creating filled rect: %s\n", SDL_GetError());
        return false;
    }

    c->images[1] = SDL_CreateTextureFromSurface(c->renderer, c->target_surf);
    if (!c->images[1]) {
        fprintf(stderr, "Error creating a texture: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

bool clock_generate_images(struct Clock *c) {

    SDL_Color colors[] = {
        {223, 223, 223, 255},
        {31, 31, 31, 255},
    };

    if (!clock_generate_image(c, colors[0], colors[1])) {
        return false;
    }

    return true;
}

void clock_draw(struct Clock *c) {

    for (int i = 0; i < DIGITS_LENGTH; i++) {
        SDL_RenderCopy(c->renderer, c->images[1], NULL, &c->digits[i].rect);
    }
}
