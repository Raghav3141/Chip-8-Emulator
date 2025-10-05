#include "chip8_header.h"

#define START_ADDRESS 0x200
#define FONTSIZE 80
#define FONTADDRESS 0x050

Chip8::Chip8()
    : randGen(std::chrono::system_clock::now().time_since_epoch().count())//random number seed that uses time
    {
        pc = START_ADDRESS;
        for (int i = 0; i < FONTSIZE; i++){
            mmem[FONTADDRESS + i] = fonts[i];
        }

        randByte = std::uniform_int_distribution<uint8_t>(0, 255U); //random value that uses random number seed with uniform probability distribution
        /*below  is a table of function pointers that direct the user
        to the respective instruction based on the opcode received*/
        table[0x0] = &Chip8::Table0;
        table[0x1] = &Chip8::OP1nnn;
        table[0x2] = &Chip8::OP2nnn;
        table[0x3] = &Chip8::OP3xkk;
        table[0x4] = &Chip8::OP4xkk;
        table[0x5] = &Chip8::OP5xy0;
        table[0x6] = &Chip8::OP6xkk;
        table[0x7] = &Chip8::OP7xkk;
        table[0x8] = &Chip8::Table8;
        table[0x9] = &Chip8::OP9xy0;
        table[0xA] = &Chip8::OPAnnn;
        table[0xB] = &Chip8::OPBnnn;
        table[0xC] = &Chip8::OPCxkk;
        table[0xD] = &Chip8::OPDxyn;
        table[0xE] = &Chip8::TableE;
        table[0xF] = &Chip8::TableF;

        for (uint8_t i = 0; i <= 0xE; i++){
            //Use OPNULL to catch any stray instructions instead of getting null pointers
            table0[i] = &Chip8::OPNULL;
            table8[i] = &Chip8::OPNULL;
            tableE[i] = &Chip8::OPNULL;
        }

        table0[0x0] = &Chip8::OP_00E0;
        table0[0xE] = &Chip8::OP_00EE;

        table8[0x0] = &Chip8::OP8xy0;
        table8[0x1] = &Chip8::OP8xy1;
        table8[0x2] = &Chip8::OP8xy2;
        table8[0x3] = &Chip8::OP8xy3;
        table8[0x4] = &Chip8::OP8xy4;
        table8[0x5] = &Chip8::OP8xy5;
        table8[0x6] = &Chip8::OP8xy6;
        table8[0x7] = &Chip8::OP8xy7;
        table8[0xE] = &Chip8::OP8xyE;

        tableE[0x1] = &Chip8::OPExA1;
        tableE[0xE] = &Chip8::OPEx9E;

        for (uint8_t i = 0; i <= 0x65; i++){
            tableF[i] = &Chip8::OPNULL;
        }

        tableF[0x7] = &Chip8::OPFx07;
        tableF[0xA] = &Chip8::OPFx0A;
        tableF[0x15] = &Chip8::OPFx15;
        tableF[0x18] = &Chip8::OPFx18;
        tableF[0x1E] = &Chip8::OPFx1E;
        tableF[0x29] = &Chip8::OPFx29;
        tableF[0x33] = &Chip8::OPFx33;
        tableF[0x55] = &Chip8::OPFx55;
        tableF[0x65] = &Chip8::OPFx65;

    }

    /*the functions below dereferences the Chip8 pointer, then accesses the 
        function pointer from the respective table, then dereferences that
        to call the function*/
    void Chip8::Table0(){
           ((*this).*(table0[opcode & 0x000F]))();
    }

    void Chip8::Table8(){
           ((*this).*(table8[opcode & 0x000F]))();
    }

    void Chip8::TableE(){
           ((*this).*(tableE[opcode & 0x000F]))();
    }

    void Chip8::TableF(){
           ((*this).*(tableF[opcode & 0x00FF]))();
    }
    
    void Chip8::OPNULL(){}

    void Chip8::OP_00E0(void){ // Clears the display
        memset(display, 0 ,sizeof(display));
    }

    void Chip8::OP_00EE(){ // Call the stack when returning a value
        --sp;
        pc = stack[sp]; 
    }

    void Chip8::OP1nnn(){ //Jump to address nnn in memory
        uint16_t address = opcode & 0x0FFFu;
        pc = address;
    }

    void Chip8::OP2nnn(){ //Call subroutine at nnn
        stack[sp] = pc;
        sp++;
        
        pc = opcode & 0x0FFFu;
    }

    void Chip8::OP3xkk(){ //if Vx = kk, skip instruction
        uint8_t vx = (opcode >> 8) & 0x0F;
        if (procregs[vx] == (opcode & 0x00FF)){
            pc += 2;
        }
    }

    void Chip8::OP4xkk(){ //if Vx != kk, skip instruction
        uint8_t vx = (opcode >> 8) & 0x0F;
        if (procregs[vx] != (opcode & 0x00FF)){
            pc += 2;
        }
    }

    void Chip8::OP5xy0(){ //if Vx = Vy, skip instruction
        uint8_t vx = (opcode >> 8) & 0x0F;
        uint8_t vy = (opcode & 0x00F0) >> 4;
        if (procregs[vx] == procregs[vy]){
            pc += 2;
        }
    }

    void Chip8::OP6xkk(){ //Vx = kk
        uint8_t vx = (opcode & 0x0F00) >> 8;
        uint8_t byte = opcode & 0x00FF;
        procregs[vx] = byte;
    }

    void Chip8::OP7xkk(){ //Vx = Vx + kk
        uint8_t vx = (opcode & 0x0F00) >> 8;
        uint8_t byte = opcode & 0x00FF;
        procregs[vx] += byte;
    }

    void Chip8::OP8xy0(){ //Vx = Vy
        uint8_t vx = (opcode & 0x0F00)  >> 8;
        uint8_t vy = (opcode & 0x00F0) >> 4;
        procregs[vx] = procregs[vy];
    }

    void Chip8::OP8xy1(){ //Vx = Vx OR Vy
        uint8_t vx = (opcode & 0x0F00)  >> 8;
        uint8_t vy = (opcode & 0x00F0) >> 4;
        procregs[vx] |= procregs[vy];
    }

    void Chip8::OP8xy2(){ //Vx = Vx AND Vy
        uint8_t vx = (opcode & 0x0F00)  >> 8;
        uint8_t vy = (opcode & 0x00F0) >> 4;
        procregs[vx] &= procregs[vy];
    }

    void Chip8::OP8xy3(){ // Vx = Vx XOR Vy
        uint8_t vx = (opcode & 0x0F00)  >> 8;
        uint8_t vy = (opcode & 0x00F0) >> 4;
        procregs[vx] ^= procregs[vy];
    }

    void Chip8::OP8xy4(){ // Vx = Vx + Vy, with VF representing carry 
        uint8_t vx = (opcode & 0x0F00)  >> 8;
        uint8_t vy = (opcode & 0x00F0) >> 4;
        uint16_t sum = procregs[vx] + procregs[vy];
        if (sum > 255){
            procregs[0xF] = 1; 
        }
        else {
            procregs[0xF] = 0;
        }
        procregs[vx] = sum & 0xFF;
    }

    void Chip8::OP8xy5(){ //Vx = Vx - Vy, with VF representing NOT borrow
        uint8_t vx = (opcode & 0x0F00)  >> 8;
        uint8_t vy = (opcode & 0x00F0) >> 4;
        if (procregs[vx] > procregs[vy]){
            procregs[0xF] = 1; 
        }
        else {
            procregs[0xF] = 0;
        }
        procregs[vx] -= procregs[vy];
    }

    void Chip8::OP8xy6(){ /*if last bit of Vx is 1,
        set VF to 1, else VF is 0, then divide Vx by 2*/
        uint8_t vx = (opcode & 0x0F00)  >> 8;
        procregs[0xF] = procregs[vx] & 0x1;
        procregs[vx] >>= 1;
    }

    void Chip8::OP8xy7(){ //Vx = Vy - Vx, with VF representing NOT borrow
        uint8_t vx = (opcode & 0x0F00)  >> 8;
        uint8_t vy = (opcode & 0x00F0) >> 4;
        if (procregs[vy] > procregs[vx]){
            procregs[0xF] = 1; 
        }
        else {
            procregs[0xF] = 0;
        }
        procregs[vx] = procregs[vy] - procregs[vx];
    }

    void Chip8::OP8xyE(){ /*if first  bit of Vx is 1,
        set VF to 1, else VF is 0, then multiply Vx by 2*/
        uint8_t vx = (opcode & 0x0F00)  >> 8;
        procregs[0xF] = (procregs[vx] & 0x80) >> 7;
        procregs[vx] <<= 1;
    }

    void Chip8::OP9xy0(){ // Skip next instruction if Vx != Vy
        uint8_t vx = (opcode & 0x0F00)  >> 8;
        uint8_t vy = (opcode & 0x00F0) >> 4;
        if (procregs[vx] != procregs[vy]){
            pc += 2;
        }
    }

    void Chip8::OPAnnn(){ // loads the index register with memory address nnn
        indexreg = opcode & 0x0FFF;
    }

    void Chip8::OPBnnn(){ /*jumps to address nnn with offset of
        V0*/
        uint16_t address = opcode & 0x0FFF;
        pc = address + procregs[0];
    }

    void Chip8::OPCxkk(){
        uint8_t vx = (opcode & 0x0F00) >> 8;
        uint8_t value = (opcode & 0x00FF);
        procregs[vx] = randByte(randGen) & value;
    }

    void Chip8::OPDxyn(){ //Displays a sprite at (Vx, Vy)
        uint8_t vx = (opcode & 0x0F00) >> 8;
        uint8_t vy = (opcode & 0x00F0) >> 4;
        uint8_t numbytes = (opcode & 0x000F);

        uint8_t xpos = procregs[vx] % VIDEO_WIDTH;
        uint8_t ypos = procregs[vy] % VIDEO_HEIGHT;
        
        procregs[0xF] = 0;
        for (int i = 0; i < numbytes; i++){
            uint8_t spriterow = mmem[indexreg +  i];
            for (int j = 0; j < 8; j++){
                uint8_t spritecol = spriterow & (0x80 >> j);
                uint32_t* screenpix = &display[(ypos + i) * VIDEO_WIDTH + (xpos + j)];

                if(spritecol){
                    if(*screenpix == 0xFFFFFFFF){
                    procregs[0xF] = 1;
                    }
                    *screenpix ^= 0xFFFFFFFF;
                }
            }
        }
    }

    void Chip8::OPEx9E(){ //Skips instruction if key with value Vx is pressed
        uint8_t vx = (opcode & 0x0F00) >> 8;
        uint8_t key = procregs[vx];
        if (keys[key]){
            pc += 2;
        }
    }

    void Chip8::OPExA1(){ //Skips instruction if key with value Vx is not pressed
        uint8_t vx = (opcode & 0x0F00) >> 8;
        uint8_t key = procregs[vx];
        if (!keys[key]){
            pc += 2;
        }
    }

    void Chip8::OPFx07(){ //Sets register Vx to delay  timer value
        uint8_t vx = (opcode & 0x0F00) >> 8;
        procregs[vx] = deltimer;
    }

    void Chip8::OPFx0A(){ //Waits for a key press and then assigns key press to Vx
        uint8_t vx = (opcode & 0x0F00) >> 8;
        for (int i = 0; i < 16; i++){
            if (keys[i]){
                procregs[vx] = i;
                return;
            }
        }
        pc -= 2;
    }
    
    void Chip8::OPFx15(){ //Set delay timer = Vx
        uint8_t vx = (opcode & 0x0F00) >> 8;
        deltimer = procregs[vx];
    }

    void Chip8::OPFx18(){ //Set sound timer = Vx
        uint8_t vx = (opcode & 0x0F00) >> 8;
        sound = procregs[vx];
    }

    void Chip8::OPFx1E(){ //Increments the index register by Vx
        uint8_t vx = (opcode & 0x0F00) >> 8;
        indexreg += procregs[vx];
    }

    void Chip8::OPFx29(){ //Sets the index register to the start of the selected font sprite
        uint8_t vx = (opcode & 0x0F00) >> 8;
        indexreg = FONTADDRESS + (5*procregs[vx]);
    }

    void Chip8::OPFx33(){ /*stores hundreds value
        in indexreg, tens value in indexreg + 1, and 
        ones value in indexreg + 2*/
        uint8_t vx = (opcode & 0x0F00) >> 8;
        mmem[indexreg] = procregs[vx]/100;
        mmem[indexreg + 1] = (procregs[vx] % 100) / 10;
        mmem[indexreg + 2] = procregs[vx] % 10;
    }

    void Chip8::OPFx55(){/*Store values from processor registers
        into main memory*/
        uint8_t vx = (opcode & 0x0F00) >> 8;
        for (int i = 0; i <= vx; i++){
            mmem[indexreg + i] = procregs[i];
        }
    }

    void Chip8::OPFx65(){ /*Store values from main memory
        into processor registers*/
        uint8_t vx = (opcode & 0x0F00) >> 8;
        for (int i = 0; i <= vx; i++){
            procregs[i] = mmem[indexreg + i];
        }        
    }

    /*Code for the Chip8 Fetch-Execute cycle*/
    void Chip8::Cycle(){
        //Fetch
        opcode = (mmem[pc] << 8) | (mmem[pc + 1]);
        pc += 2;

        //Decode
        uint8_t first_byte = (opcode & 0xF000) >> 12;

        //Execute
        ((*this).*(table[first_byte]))();

        if (deltimer > 0){
            deltimer--;
        }
        if(sound > 0){
            sound--;
        }
    }

    void Chip8::LoadROM(char const* filename){

    /*ifstream reads the file, then the data is converted to 
    binary and the starting pointer is at the end of the file*/
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (file.is_open()){
        std::streampos size = file.tellg(); //this tells me the file size in bytes
        char* buffer = new char[size];

        //go back to the beginning of the file
        file.seekg(0, std::ios::beg);
        
        file.read(buffer, size);
        file.close();

        for (long i = 0; i < size; i++){
            /*starts reading at address 0x200 and adds 
            contents of the buffer to main memory*/
            mmem[START_ADDRESS + i] = buffer[i];
        }
        delete[] buffer;
        }
    }

uint16_t fonts[FONTSIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x10, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80 //F
};
