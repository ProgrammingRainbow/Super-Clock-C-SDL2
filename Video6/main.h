#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SDL_FLAGS (SDL_INIT_VIDEO | SDL_INIT_AUDIO)
#define IMG_FLAGS IMG_INIT_PNG

#define WINDOW_TITLE "Super Clock"
#define WINDOW_WIDTH 294
#define WINDOW_HEIGHT 174

#define DIGITS_LENGTH 24
#define DIGIT_SIZE 36
#define DIGIT_OFFSET 6

#define FONT_SIZE 24

#endif
