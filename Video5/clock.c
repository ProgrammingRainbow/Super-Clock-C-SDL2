#include "clock.h"

bool clock_generate_image(struct Clock *c, int image_index,
                          SDL_Color front_color, SDL_Color back_color,
                          bool color, bool size);
bool clock_generate_images(struct Clock *c);

bool clock_new(struct Clock **clock, SDL_Window *window,
               SDL_Renderer *renderer) {
    *clock = calloc(1, sizeof(struct Clock));
    if (!*clock) {
        fprintf(stderr, "Error in calloc of clock!\n");
        return false;
    }
    struct Clock *c = *clock;

    c->window = window;
    c->renderer = renderer;
    c->enable_color = true;

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
            for (int j = 0; j < 8; j++) {
                for (int k = 0; k < 2; k++) {
                    for (int l = 0; l < 2; l++) {
                        SDL_DestroyTexture((*clock)->images[i][j][k][l]);
                        (*clock)->images[i][j][k][l] = NULL;
                    }
                }
            }
        }

        (*clock)->renderer = NULL;
        (*clock)->window = NULL;

        free(*clock);
        *clock = NULL;
    }
}

void clock_update_layout(struct Clock *c) {
    int multiplier = c->enable_large ? 2 : 1;

    int i = 0;
    int offset = DIGIT_OFFSET;
    int width = 0;
    int height = 0;

    width = DIGIT_SIZE * 6 + DIGIT_OFFSET * 13;
    height = DIGIT_SIZE * 4 + DIGIT_OFFSET * 5;
    for (int column = 0; column < 6; column++) {
        if (column > 3) {
            offset = DIGIT_OFFSET * 7;
        } else if (column > 1) {
            offset = DIGIT_OFFSET * 4;
        }
        for (int row = 0; row < 4; row++) {
            if (i < DIGITS_LENGTH) {
                c->digits[i].rect.x =
                    (column * (DIGIT_SIZE + DIGIT_OFFSET) + offset) *
                    multiplier;
                c->digits[i].rect.y =
                    (row * (DIGIT_SIZE + DIGIT_OFFSET) + DIGIT_OFFSET) *
                    multiplier;
                c->digits[i].rect.w = DIGIT_SIZE * multiplier;
                c->digits[i].rect.h = DIGIT_SIZE * multiplier;
                i++;
            }
        }
    }

    SDL_SetWindowSize(c->window, width * multiplier, height * multiplier);
    SDL_SetWindowPosition(c->window, SDL_WINDOWPOS_CENTERED,
                          SDL_WINDOWPOS_CENTERED);
}

bool clock_generate_image(struct Clock *c, int image_index,
                          SDL_Color front_color, SDL_Color back_color,
                          bool color, bool size) {

    int multiplier = size ? 2 : 1;

    if (c->target_surf) {
        SDL_FreeSurface(c->target_surf);
        c->target_surf = NULL;
    }

    c->target_surf = SDL_CreateRGBSurface(
        0, DIGIT_SIZE * multiplier, DIGIT_SIZE * multiplier, 32, 0, 0, 0, 255);
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

    c->images[0][image_index][color][size] =
        SDL_CreateTextureFromSurface(c->renderer, c->target_surf);
    if (!c->images[0][image_index][color][size]) {
        fprintf(stderr, "Error creating a texture: %s\n", SDL_GetError());
        return false;
    }

    if (SDL_FillRect(c->target_surf, NULL,
                     SDL_MapRGB(c->target_surf->format, front_color.r,
                                front_color.g, front_color.b)) != 0) {
        fprintf(stderr, "Error creating filled rect: %s\n", SDL_GetError());
        return false;
    }

    c->images[1][image_index][color][size] =
        SDL_CreateTextureFromSurface(c->renderer, c->target_surf);
    if (!c->images[1][image_index][color][size]) {
        fprintf(stderr, "Error creating a texture: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

bool clock_generate_images(struct Clock *c) {

    SDL_Color colors[] = {
        {245, 224, 220, 255}, {243, 139, 168, 255}, {250, 179, 135, 255},
        {249, 226, 175, 255}, {166, 227, 161, 255}, {148, 226, 213, 255},
        {137, 180, 250, 255}, {203, 166, 247, 255}, {30, 30, 46, 255},
        {223, 223, 223, 255}, {31, 31, 31, 255},
    };

    for (int i = 0; i < 8; i++) {
        if (!clock_generate_image(c, i, colors[9], colors[10], false, false)) {
            return false;
        }

        if (!clock_generate_image(c, i, colors[9], colors[10], false, true)) {
            return false;
        }

        if (!clock_generate_image(c, i, colors[i % 8], colors[8], true,
                                  false)) {
            return false;
        }

        if (!clock_generate_image(c, i, colors[i % 8], colors[8], true, true)) {
            return false;
        }
    }

    return true;
}

void clock_toggle_color(struct Clock *c) { c->enable_color = !c->enable_color; }

void clock_toggle_size(struct Clock *c) {
    c->enable_large = !c->enable_large;
    clock_update_layout(c);
}

void clock_update(struct Clock *c) {
    time_t raw_time;
    time(&raw_time);
    struct tm *current_time = localtime(&raw_time);

    int n = current_time->tm_hour;
    for (int i = 7; i >= 0; i--) {
        c->digits[i].digit = n % 2;
        n /= 2;
    }

    n = current_time->tm_min;
    for (int i = 15; i >= 8; i--) {
        c->digits[i].digit = n % 2;
        n /= 2;
    }

    n = current_time->tm_sec;
    for (int i = 23; i >= 16; i--) {
        c->digits[i].digit = n % 2;
        n /= 2;
    }
}

void clock_draw(struct Clock *c) {

    for (int i = 0; i < DIGITS_LENGTH; i++) {
        SDL_RenderCopy(c->renderer,
                       c->images[c->digits[i].digit][i % 8][c->enable_color]
                                [c->enable_large],
                       NULL, &c->digits[i].rect);
    }
}