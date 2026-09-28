#include "sim.h"
#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;
static uint32_t pixels[SIM_Y_SIZE][SIM_X_SIZE];
static uint32_t frame_tick;
static int pending_click;
static int click_position;

static void check_sdl(int failed) {
  if (failed) {
    fprintf(stderr, "SDL error: %s\n", SDL_GetError());
    exit(1);
  }
}

void simInit(void) {
  check_sdl(SDL_Init(SDL_INIT_VIDEO) != 0);
  window = SDL_CreateWindow("Colony of six-colour turmites",
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            SIM_X_SIZE, SIM_Y_SIZE, 0);
  check_sdl(window == NULL);
  renderer = SDL_CreateRenderer(window, -1, 0);
  check_sdl(renderer == NULL);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                              SDL_TEXTUREACCESS_STREAMING, SIM_X_SIZE,
                              SIM_Y_SIZE);
  check_sdl(texture == NULL);
  check_sdl(SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE) != 0);
  frame_tick = SDL_GetTicks();
}

void simExit(void) {
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

static void poll_events(void) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      simExit();
      exit(0);
    }
    if (event.type == SDL_MOUSEBUTTONDOWN) {
      pending_click = 1;
      click_position = (event.button.x << 16) | (event.button.y & 0xFFFF);
    }
  }
}

void simFlush(void) {
  poll_events();
  check_sdl(SDL_UpdateTexture(texture, NULL, pixels,
                              SIM_X_SIZE * (int)sizeof(uint32_t)) != 0);
  check_sdl(SDL_RenderClear(renderer) != 0);
  check_sdl(SDL_RenderCopy(renderer, texture, NULL, NULL) != 0);
  SDL_RenderPresent(renderer);
  uint32_t elapsed = SDL_GetTicks() - frame_tick;
  if (elapsed < 33) {
    SDL_Delay(33 - elapsed);
  }
  frame_tick = SDL_GetTicks();
}

void simPutPixel(int x, int y, int argb) {
  if (x < 0 || x >= SIM_X_SIZE || y < 0 || y >= SIM_Y_SIZE) {
    fputs("simPutPixel: coordinates outside the canvas\n", stderr);
    exit(1);
  }
  pixels[y][x] = (uint32_t)argb;
}

int simRand(void) { return rand(); }

int simHasClick(void) {
  poll_events();
  return pending_click;
}

int simGetClick(void) {
  poll_events();
  if (pending_click) {
    pending_click = 0;
    return click_position;
  }
  return 0;
}
