typedef struct cpu {
    unsigned char ram[4096];
    unsigned char registers[16];
    unsigned short i_register;
    unsigned char timer;
    unsigned char sound;
    unsigned short PC;
    unsigned short SP;
    unsigned short stack[16];
    unsigned char screen[64*32];
    unsigned char key[16];
    unsigned char d_flag; 
 } cpu;
void init(cpu *cur_cpu, unsigned char font[]);
void readOpcode(cpu *cur_cpu);
int read_rom(cpu *cur_cpu, const char *rom);