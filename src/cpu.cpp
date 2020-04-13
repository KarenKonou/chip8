#include "cpu.h"
#include <iostream>

unsigned char chip8_fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

CPU::CPU() {
  std::cout << "Initializing cpu... ";
  pc = 0x200;
  opcode = 0;
  I = 0;
  sp = 0;
  for (int i = 0; i < 4096; i++) {
    memory[i] = 0;
  }
  for (int i = 0; i < 16; i++) {
    V[i] = 0;
  }
  for (int i = 0; i < 16; i++) {
    stack[i] = 0;
  }
  for (int i = 0; i < 16; i++) {
    key[i] = 0;
  }

  for (int i = 0; i < 80; i++) {
    memory[i + 80] = chip8_fontset[i];
  }

  delay_timer = 60;
  sound_timer = 60;

  std::cout << "Done!" << std::endl;
}

auto CPU::loadProgram(const char* path) -> int {
  FILE* file = fopen(path, "rb");
  if (file == nullptr) {
    std::cout << "Couldn't load program." << std::endl;
    return 1;
  }

  fseek(file, 0, SEEK_END);
  long bufferSize = ftell(file);
  rewind(file);

  char* buffer = (char*)malloc(sizeof(char) * bufferSize);
  fread(buffer, 1, bufferSize, file);
  for (int i = 0; i < bufferSize; ++i) {
    memory[i + 512] = buffer[i];
  }
  fclose(file);
  free(buffer);
  return 0;
}

auto CPU::cycle() -> void {
  opcode = memory[pc] << 8 | memory[pc + 1];

  switch (opcode & 0xF000) {}
}