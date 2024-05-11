#include "cpu.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
int print_once = 0;
void init(cpu *cur_cpu, unsigned char font[])
{
    memset(cur_cpu, 0, sizeof(cpu));
    cur_cpu->PC = 0x200;
    cur_cpu->SP = 0;
    for (int i=0; i<0x50;i++)
    {
        cur_cpu->ram[i] = font[i];
    }
    srand (time(NULL));
}
int read_rom(cpu *cur_cpu, const char *fileName)
{
    FILE *rom = fopen(fileName, "rb");
    if (!rom)
    {
        return -1;
    }
    fseek(rom, 0, SEEK_END);
    int size = ftell(rom);
    fseek(rom, 0, SEEK_SET);
    unsigned char* buff = (unsigned char*)malloc(sizeof(unsigned char) * size);
    if (buff == NULL)
    {
        return -1;
    }
    if (!fread(buff, sizeof(unsigned char), size, rom))
    {
        return -1;
    }
    for (int i=0; i<size;i++)
    {
        cur_cpu->ram[i+0x200] = buff[i];
    }
    fclose(rom);
    free(buff);
    return 0;
}
int readOpcode(cpu *cur_cpu)
{
    unsigned char reg_no;
    unsigned char reg_no_2;
    unsigned char val;
    unsigned short opcode = cur_cpu->ram[cur_cpu->PC] << 8 | cur_cpu->ram[cur_cpu->PC+1];
    cur_cpu->PC+=2;
    int pressed;
    switch(opcode & 0xF000)
    {
        case 0x0000:
            switch(opcode & 0x00FF)
            {
                case 0x00E0:
                    memset(cur_cpu->screen, 0, sizeof(unsigned char)*(64*32));
                    cur_cpu->d_flag = 1;
                break;

                case 0x00EE:
                    cur_cpu->SP--;
                    cur_cpu->PC = cur_cpu->stack[cur_cpu->SP];
                    state_of_cpu(cur_cpu);
                break;
                default: 
                return-1;
                break;
            }
        break;

        case 0x1000:
            cur_cpu->PC = opcode & 0x0FFF;
        break;
        case 0x2000:
            if (cur_cpu->SP > sizeof(cur_cpu->stack) / sizeof(unsigned short)) {
                printf("Fatal error: Stack overflow\n");
                printf("SP is %X", cur_cpu->SP);
                return -1;
            }
            if(print_once==0)
            {
                printf("%X\n",cur_cpu->PC);
            }
            cur_cpu->stack[cur_cpu->SP] = cur_cpu->PC;
            cur_cpu->SP++;
            cur_cpu->PC = opcode & 0x0FFF;
            state_of_cpu(cur_cpu);
        break;

        case 0x3000:
           reg_no = (opcode & 0x0F00) >> 8;
           val = opcode & 0x00FF;
           if (cur_cpu->registers[reg_no] == val)
           {
                cur_cpu->PC +=2;
           }
        break;

        case 0x4000:
            reg_no = (opcode & 0x0F00) >> 8;
            val = opcode & 0x00FF;
           if (cur_cpu->registers[reg_no] != val)
           {
                cur_cpu->PC +=2;
           }
        break;
          case 0x5000:
            reg_no = (opcode & 0x0F00) >> 8;
            reg_no_2 = (opcode & 0x00F0) >> 4;
           if (cur_cpu->registers[reg_no] == cur_cpu->registers[reg_no_2])
           {
                cur_cpu->PC +=4;
           }
        break;
        case 0x6000:
            reg_no = (opcode & 0x0F00) >> 8;
            val = (opcode & 0x00FF);
            cur_cpu->registers[reg_no] = val;
        break;
        case 0x7000:
            reg_no = (opcode & 0x0F00) >> 8;
            val = (opcode & 0x00FF);
            cur_cpu->registers[reg_no] += val;
        break;
        case 0x8000:
            switch(opcode & 0x000F)
            {
                case 0x0000:
                    reg_no = (opcode & 0x0F00) >> 8;
                    reg_no_2 = (opcode & 0x00F0) >> 4;
                    cur_cpu->registers[reg_no] = cur_cpu->registers[reg_no_2];
                break;
                case 0x0001:
                    reg_no = (opcode & 0x0F00) >> 8;
                    reg_no_2 = (opcode & 0x00F0) >> 4;
                   cur_cpu->registers[reg_no] |=  cur_cpu->registers[reg_no_2];
                break;
                case 0x0002:
                    reg_no = (opcode & 0x0F00) >> 8;
                    reg_no_2 = (opcode & 0x00F0) >> 4;
                   cur_cpu->registers[reg_no] &=  cur_cpu->registers[reg_no_2];
                break;
                case 0x0003:
                    reg_no = (opcode & 0x0F00) >> 8;
                    reg_no_2 = (opcode & 0x00F0) >> 4;
                   cur_cpu->registers[reg_no] ^=  cur_cpu->registers[reg_no_2];
                break;
                case 0x0004:
                    reg_no = (opcode & 0x0F00) >> 8;
                    reg_no_2 = (opcode & 0x00F0) >> 4;
                    int test = (int)(cur_cpu->registers[reg_no] +cur_cpu->registers[reg_no_2]);
                    cur_cpu->registers[reg_no] += cur_cpu->registers[reg_no_2];
                    if(test>255)
                    {
                        cur_cpu->registers[0xF] =1;
                    }
                    else
                    {
                        cur_cpu->registers[0xF] =0;
                    }
                break;
                case 0x0005:
                    reg_no = (opcode & 0x0F00) >> 8;
                    reg_no_2 = (opcode & 0x00F0) >> 4;
                    int  carry = (int)cur_cpu->registers[reg_no] -cur_cpu->registers[reg_no_2];
                    cur_cpu->registers[reg_no] -= cur_cpu->registers[reg_no_2];
                      if (carry<0)
                    {
                         cur_cpu->registers[0xF] =0;
                    }
                    else{
                        cur_cpu->registers[0xF] =1;
                    }

                break;
                case 0x0006:
                    reg_no = (opcode & 0x0F00) >> 8;
                    val =  cur_cpu->registers[reg_no];
                    cur_cpu->registers[reg_no] >>= 1;
                     cur_cpu->registers[0xF] = val&1;
                break;
                case 0x0007:
                    reg_no = (opcode & 0x0F00) >> 8;
                    reg_no_2 = (opcode & 0x00F0) >> 4;
                    cur_cpu->registers[reg_no] = (cur_cpu->registers[reg_no_2] - cur_cpu->registers[reg_no]);
                    if (cur_cpu->registers[reg_no_2] > cur_cpu->registers[reg_no])
                    {
                         cur_cpu->registers[0xF] =1;
                    }
                    else{
                        cur_cpu->registers[0xF] =0;;
                    }
                break;
                case 0x000E:
                    reg_no = (opcode & 0x0F00) >> 8;
                    val =  cur_cpu->registers[reg_no];
                    cur_cpu->registers[reg_no] <<= 1;
                    cur_cpu->registers[0xF] = val >> 7;
                break;

            }
        break;
        case 0x9000:
            reg_no = (opcode & 0x0F00) >> 8;
            reg_no_2 = (opcode & 0x00F0) >> 4;
            if(cur_cpu->registers[reg_no_2] != cur_cpu->registers[reg_no])
            {
                cur_cpu->PC +=2;
            }
        break;
        case 0xA000:
            cur_cpu->i_register = opcode & 0xFFF;
        break;
        case 0xB000:
            val = opcode & 0x0FFF;
            cur_cpu->PC = val+cur_cpu->registers[0];
        break;
        case 0xC000:
            reg_no = (opcode & 0x0F00) >> 8;
            cur_cpu->registers[reg_no] = (rand()%0xFF) & (opcode & 0x00FF);
        break;
        case 0xD000:
            reg_no = (opcode & 0x0F00) >> 8;
            reg_no_2 = (opcode & 0x00F0) >> 4;
            unsigned short pixel;
            unsigned short x =  cur_cpu->registers[reg_no]%64;
            unsigned short y =  cur_cpu->registers[reg_no_2]%32;
            unsigned short n =  (opcode & 0x000F);
            cur_cpu->registers[0xF] = 0;
            for (int yline = 0; yline < n; yline++)
            {
                if (y + yline > 32)
                {
                    break;
                }
                pixel = cur_cpu->ram[cur_cpu->i_register+yline];
                for(int xline = 0; xline<8;xline++)
                {
                    if (x + xline > 64)
                    {
                        break;
                    }
                    if((pixel &(0x080 >> xline)) !=0)
                    {
                        if(cur_cpu->screen[(x + xline + ((y+yline)*RENDER_W))] ==1)
                        {
                            cur_cpu->registers[0xF] = 1;
                        }
                        cur_cpu->screen[(x + xline + ((y+yline)*RENDER_W))] ^=1;
                    }
                }
                
            }
            cur_cpu->d_flag = 1;
        break;
        case 0xE000:
            switch(opcode & 0x00FF)
            {
                case 0x009E:
                    reg_no = (opcode & 0x0F00) >> 8;
                    if (cur_cpu->key[cur_cpu->registers[reg_no]] !=0 )
                    {
                        cur_cpu->PC +=2;
                    }
                break;
                 case 0x00A1:
                    reg_no = (opcode & 0x0F00) >> 8;
                    if (cur_cpu->key[cur_cpu->registers[reg_no]] ==0 )
                    {
                        cur_cpu->PC +=2;
                    }
                break;

            }
        break;
        case 0xF000:
            switch(opcode & 0x00FF)
            {
                case 0x0007:
                    reg_no = (opcode & 0x0F00) >> 8;
                    cur_cpu->registers[reg_no] =  cur_cpu->timer;
                break;
                case 0x000A:
                     pressed = 0;
                     reg_no = (opcode & 0x0F00) >> 8;
                     for(int i=0; i<16; i++)
                     {
                        if(cur_cpu->key[i] != 0)
                        {
                            cur_cpu->registers[reg_no] = i;
                            pressed = 1;
                        }
                     }
                     if (pressed != 1)
                     {
                        cur_cpu->PC -=2;
                        return 0;
                     }
                    
                break;
                case 0x0015:
                    reg_no = (opcode & 0x0F00) >> 8;
                    cur_cpu->timer = cur_cpu->registers[reg_no];
                break;
                case 0x0018:
                    reg_no = (opcode & 0x0F00) >> 8;
                    cur_cpu->sound = cur_cpu->registers[reg_no];
                break;
                case 0x001E:
                    reg_no = (opcode & 0x0F00) >> 8;
                    cur_cpu->i_register = (cur_cpu->i_register + cur_cpu->registers[reg_no]);
                break;
                case 0x0029:
                    reg_no = (opcode & 0x0F00) >> 8;
                    cur_cpu->i_register = (cur_cpu->registers[reg_no]*5);
                break;
                case 0x0033:
                    reg_no = (opcode & 0x0F00) >> 8;
                    cur_cpu->ram[cur_cpu->i_register] = cur_cpu->registers[reg_no]/100;
                    cur_cpu->ram[cur_cpu->i_register+1] = (cur_cpu->registers[reg_no]/10)%10;
                    cur_cpu->ram[cur_cpu->i_register+2] = (cur_cpu->registers[reg_no]%100)%10; 
                break;
                case 0x0055:
                     reg_no = (opcode & 0x0F00) >> 8;
                     for(unsigned char i=0; i<=reg_no; i++)
                     {
                        cur_cpu->ram[cur_cpu->i_register+i] = cur_cpu->registers[i];
                     }
                break;
                case 0x0065:
                     reg_no = (opcode & 0x0F00) >> 8;
                     for(unsigned char i=0; i<=reg_no; i++)
                     {
                        cur_cpu->registers[i] = cur_cpu->ram[cur_cpu->i_register+i];
                     }
                    break;
            }
        break;

        default:
            printf("Have not implemented opt-code 0x%X\n", opcode);
        break;

    }
    if (cur_cpu->timer > 0)
    {
        --cur_cpu->timer;
    }
    if (cur_cpu->sound > 0)
    {
        --cur_cpu->sound;
    }
    return 0;
}
void state_of_cpu(cpu* cur_cpu)
{
    return;
    printf("SP:%X\n", cur_cpu->SP);
    printf("PC:%X\n", cur_cpu->PC);
    printf("I:%X\n", cur_cpu->i_register);
    for (int i = 0; i < 16; i++) {
        printf("Register[%d]:%X\n", i, cur_cpu->registers[i]);
    }
    for (int i = 0; i < 16; i++) {
        printf("stack[%d]:%X\n", i, cur_cpu->stack[i]);
    }
}