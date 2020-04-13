#include "SDL2/SDL.h"
#include "cpu.h"
#include <iostream>

CPU cpu;
const int window_widght = 640;
const int window_height = 320;
SDL_Window* win;
SDL_Renderer* ren;

auto initializeGraphics() -> int {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  win = SDL_CreateWindow("Hello World!", 100, 100, window_widght, window_height,
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

  SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
  SDL_RenderClear(ren);
  SDL_RenderPresent(ren);

  return 0;
}

auto main(int argc, char* argv[]) -> int {
  initializeGraphics();

  std::cout << "Renderer pointer is " << ren << std::endl;

  if (cpu.loadProgram(argv[1]) != 0) {
    SDL_Quit();
    return 1;
  }

  for (;;) {
    cpu.cycle();
  }

  SDL_Delay(5000);
  SDL_Quit();
  return 0;
}