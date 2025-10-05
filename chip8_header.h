#include <stdio.h>
#include <cstdint>
#include <cstring>
#include <chrono>
#include <random>
#include <fstream>
#define START_ADDRESS 0x200
#define FONTSIZE 80
#define FONTADDRESS 0x050
#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32

#ifndef CHIP8_HEADER_H
#define CHIP8_HEADER_H

extern uint16_t fonts[FONTSIZE];

class Chip8
{
public:
    Chip8();
    void LoadROM(char const*filename);
    uint8_t keys[16]{};
    uint32_t display[VIDEO_WIDTH * VIDEO_HEIGHT]{};
    void Cycle();

private:
    uint8_t procregs[16]{};
    uint8_t mmem[4096]{};
    uint16_t indexreg{};
    uint16_t pc{};
    uint16_t opcode;
    uint16_t stack[16]{};
    uint8_t sp{};
    uint8_t deltimer{};
    uint8_t sound{};

    std::default_random_engine randGen;
    std::uniform_int_distribution<uint8_t> randByte;

    void OP_00E0();
    void OP_00EE();
    void OP1nnn();
    void OP2nnn();
    void OP3xkk();
    void OP4xkk();
    void OP5xy0();
    void OP6xkk();
    void OP7xkk();
    void OP8xy0();
    void OP8xy1();
    void OP8xy2();
    void OP8xy3();
    void OP8xy4();
    void OP8xy5();
    void OP8xy6();
    void OP8xy7();
    void OP8xyE();
    void OP9xy0();
    void OPAnnn();
    void OPBnnn();
    void OPCxkk();
    void OPDxyn();
    void OPEx9E();
    void OPExA1();
    void OPFx07();
    void OPFx0A();
    void OPFx15();
    void OPFx18();
    void OPFx1E();
    void OPFx29();
    void OPFx33();
    void OPFx55();
    void OPFx65();
    void Table0();
    void Table8();
    void TableE();
    void TableF();
    void OPNULL();

    typedef void (Chip8::*Chip8Func)();
    Chip8Func table[0xF + 1];
    Chip8Func table0[0xE + 1];
    Chip8Func table8[0xE + 1];
    Chip8Func tableE[0xE + 1];
    Chip8Func tableF[0x65 + 1];
};
#endif
