#include "clock.h"

bool clock_generate_image(struct Clock *c, int image_index,
                          SDL_Color front_color, SDL_Color back_color,
                          bool color);
bool clock_generate_images(struct Clock *c);

bool clock_new(struct Clock **clock, SDL_Renderer *renderer) {
    *clock = calloc(1, sizeof(struct Clock));
    if (!*clock) {
        fprintf(stderr, "Error in calloc of clock!\n");
        return false;
    }
    struct Clock *c = *clock;

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
                    SDL_DestroyTexture((*clock)->images[i][j][k]);
                    (*clock)->images[i][j][k] = NULL;
                }
            }
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

bool clock_generate_image(struct Clock *c, int image_index,
                          SDL_Color front_color, SDL_Color back_color,
                          bool color) {

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

    c->images[0][image_index][color] =
        SDL_CreateTextureFromSurface(c->renderer, c->target_surf);
    if (!c->images[0][image_index][color]) {
        fprintf(stderr, "Error creating a texture: %s\n", SDL_GetError());
        return false;
    }

    if (SDL_FillRect(c->target_surf, NULL,
                     SDL_MapRGB(c->target_surf->format, front_color.r,
                                front_color.g, front_color.b)) != 0) {
        fprintf(stderr, "Error creating filled rect: %s\n", SDL_GetError());
        return false;
    }

    c->images[1][image_index][color] =
        SDL_CreateTextureFromSurface(c->renderer, c->target_surf);
    if (!c->images[1][image_index][color]) {
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
        if (!clock_generate_image(c, i, colors[9], colors[10], false)) {
            return false;
        }

        if (!clock_generate_image(c, i, colors[i % 8], colors[8], true)) {
            return false;
        }
    }

    return true;
}

void clock_toggle_color(struct Clock *c) { c->enable_color = !c->enable_color; }

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

void clock_draw(const struct Clock *c) {
    for (int i = 0; i < DIGITS_LENGTH; i++) {
        SDL_RenderCopy(c->renderer,
                       c->images[c->digits[i].digit][i % 8][c->enable_color],
                       NULL, &c->digits[i].rect);
    }
}
