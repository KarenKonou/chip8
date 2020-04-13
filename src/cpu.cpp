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
  draw_flag = false;
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
  std::cout << "Loading program " << path << std::endl;
  FILE* file = fopen(path, "rb");

  if (file == nullptr) {
    std::cout << "Couldn't load program." << std::endl;
    return 1;
  }

  fseek(file, 0, SEEK_END);
  long bufferSize = ftell(file);
  rewind(file);

  char* buffer = (char*)malloc(sizeof(char) * bufferSize);

  if (buffer == NULL) {
    std::cout << "Oh fuck this isn't supposed to happen shit" << std::endl;
    return 1;
  }

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

  switch (opcode & 0xF000) {
  case 0x0000:
    switch (opcode & 0x000F) {
    case 0x0000: // 0x00E0: Clears the screen
      for (int i = 0; i < 64 * 32; i++) {
        gfx[i] = 0;
      }
      draw_flag = true;
      pc += 2;
      break;

    case 0x000E: // 0x00EE: Returns from subroutine
      break;
    }
  case 0x1000: // 0x1NNN: Jumps to address NNN
    pc = opcode & 0x0FFF;
    break;

  case 0x2000: // 0x2NNN: Calls subroutine at NNN
    break;

  case 0x3000: // 0x3XNN: Skips next instruction if VX equals NN
    if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
      pc += 4;
    } else {
      pc += 2;
    }
    break;

  case 0x4000: // 0x4XNN: Skips next instruction if VX doesn't equal NN
    if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
      pc += 4;
    } else {
      pc += 2;
    }
    break;

  case 0x5000: // 0x5XY0: Skips next instruction if VX equals VY
    if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
      pc += 4;
    } else {
      pc += 2;
    }
    break;

  case 0x6000: // 0x6XNN: Sets VX to NN
    V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
    pc += 2;
    break;

  case 0x7000: // 0x7XNN: Adds NN to VX
    V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
    pc += 2;
    break;

  case 0x8000: // lotta shit here
    switch (opcode & 0x000F) {
    case 0x0000: // 0x8XY0: Sets VX to the value of VY
      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
      pc += 2;
      break;

    case 0x0001: // 0x8XY1: Sets VX to bitwise OR of VX and VY
      V[(opcode & 0x0F00) >> 8] =
          (V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4]);
      pc += 2;
      break;

    case 0x0002: // 0x8XY2: Sets VX to bitwise AND of VX and VY
      V[(opcode & 0x0F00) >> 8] =
          (V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4]);
      pc += 2;
      break;

    case 0x0003: // 0x8XY3: Sets VX to bitwise XOR of VX and VY
      V[(opcode & 0x0F00) >> 8] =
          (V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4]);
      pc += 2;
      break;

    case 0x0004: // 0x8XY4: Adds VY to VX (sets carry flag)
      if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
        V[0xF] = 1; // carry
      else
        V[0xF] = 0;
      V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
      pc += 2;
      break;

    case 0x0005: // 0x8XY5: Substract VY from FX (VF zero'd when borrowing)
      if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
        V[0xF] = 0; // borrow
      else
        V[0xF] = 1;
      V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
      pc += 2;
      break;
    }
  case 0xA000: // 0xANNN: Set I to N
    I = opcode & 0x0FFF;
    pc += 2;
    break;

  default:
    std::cout << "Unrecognised opcode 0x" << std::hex << opcode << std::endl;
    pc += 2;
  }
}