#include "SDL2/SDL.h"
#include "cpu.h"
#include <iostream>

CPU cpu;
const int window_widght = 640;
const int window_height = 320;
const float window_scale = 10.0;
SDL_Window* win;
SDL_Renderer* ren;

auto initializeGraphics() -> int {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  win = SDL_CreateWindow("Chip8", 100, 100, window_widght, window_height,
                         SDL_WINDOW_SHOWN);
  if (win == nullptr) {
    std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }

  ren = SDL_CreateRenderer(
      win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (ren == nullptr) {
    SDL_DestroyWindow(win);
    std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }

  SDL_RenderSetScale(ren, window_scale, window_scale);
  SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
  SDL_RenderClear(ren);
  SDL_RenderPresent(ren);

  return 0;
}

auto renderFrame() -> void {
  SDL_RenderClear(ren);
  SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
  for (int x = 0; x < 64; x++) {
    for (int y = 0; y < 32; y++) {
      if (cpu.gfx[(y * 64) + x] == 1) {
        SDL_RenderDrawPoint(ren, x, y);
      }
    }
  }
  SDL_RenderPresent(ren);
  SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0xFF);
}

auto main(int argc, char* argv[]) -> int {
  initializeGraphics();

  std::cout << "Renderer pointer is " << ren << std::endl;

  if (cpu.loadProgram(argv[1]) != 0) {
    SDL_Quit();
    return 1;
  }

  for (;;) {
    for (int cycles = 0; cycles < 9; cycles ++) // run for 9 cycles before rendering a frame
      cpu.cycle();                              // at 60 frames per second this will result
    renderFrame();                              // in a clock rate of 9*60 = 540hz 
  }

  SDL_Quit();
  return 0;
}