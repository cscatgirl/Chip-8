#include <stdio.h>
#include <stdlib.h>
#include<SDL.h>
#include "cpu.h"
unsigned char chip8_fontset[80] =
{ 
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
//The window we'll be rendering to
SDL_Window* gWindow = NULL;
    
//The surface contained by the window
SDL_Renderer* gRender = NULL;
SDL_Texture* gTexture = NULL;
int pause = 0;
int screen_init()
{
     if(SDL_Init(SDL_INIT_EVERYTHING)!=0)
    {
        printf("Error");
        return -1;
    }
    else
    {
    gWindow = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, (RENDER_W*SCALE), (RENDER_H*SCALE), SDL_WINDOW_SHOWN);
    if (gWindow == NULL)
    {
        printf("Error");
        return -1;
    }
    else
    {
        gRender = SDL_CreateRenderer(gWindow,-1,SDL_RENDERER_ACCELERATED);
        if (gRender == NULL)
    {
        printf("Error");
        return -1;
    }
        
        SDL_RenderSetLogicalSize(gRender, 64, 32);
        SDL_SetRenderDrawColor(gRender,0,0,0,0);
        SDL_RenderClear(gRender);
        SDL_RenderPresent(gRender);
        gTexture = SDL_CreateTexture(gRender, SDL_PIXELFORMAT_RGBA8888, 
            SDL_TEXTUREACCESS_STATIC,RENDER_W,RENDER_H);
        if (gTexture == NULL)
        {
             printf("Error");
             return -1;
        }
    

    }
    }
    return 0;
}


void terminate()
{
    SDL_DestroyTexture(gTexture);
    gTexture = NULL;
    SDL_DestroyRenderer( gRender );
    gRender = NULL;

   
    //Destroy window
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;

     //Destroy window
   

    //Quit SDL subsystems
    SDL_Quit();
}
void draw(cpu *cur_cpu)
{
    int buffer[RENDER_W*RENDER_H] = {0};
    for(int px=0;px<RENDER_W*RENDER_H;px++)
    {
        if(cur_cpu->screen[px]==1)
        {
           buffer[px]=UINT32_MAX;
        }
    }
   
    SDL_UpdateTexture(gTexture, NULL, buffer, RENDER_W*sizeof(int));
    SDL_RenderClear(gRender);
    SDL_RenderCopy(gRender, gTexture, NULL, NULL);
    SDL_RenderPresent(gRender);
}
int main(int argv, char **argc)
{  
    if(screen_init() == -1)
    {
        exit(1);
    }
    cpu *cur_cpu = (cpu*)malloc(sizeof(cpu));
    init(cur_cpu, chip8_fontset);
    int rc = read_rom(cur_cpu, argc[1]);
    if(rc == -1)
    {
        printf("Error reading file\n");
        free(cur_cpu);
        terminate();
        exit(1);
    }
    SDL_Event e;
    int quit = 0;
    int cycle = 0;
    int tick;
    int speed;
    while(quit==0)
    {   
        tick = SDL_GetTicks();
        if (pause == 0)
        {
            if (readOpcode(cur_cpu) == -1)
            {
                state_of_cpu(cur_cpu);
                terminate();
                free(cur_cpu);
            }
        cycle++;
        }
        if(cur_cpu->d_flag==1)
        {
            draw(cur_cpu);
            cur_cpu->d_flag=0;
        }
        speed = SDL_GetTicks() -tick;
        SDL_RenderPresent(gRender);
        if(speed<1000/500)
        {
            SDL_Delay(1000/500);
        }
        
        while(SDL_PollEvent(&e))
        {
          switch(e.type)
          {
            case SDL_QUIT:
                quit =1;
            break;
            case SDL_KEYDOWN:
                switch(e.key.keysym.sym)
                {
                    case  SDLK_1:
                        cur_cpu->key[0x1] =1;
                    break;
                    case  SDLK_2:
                        cur_cpu->key[0x2] =1;
                    break;
                    case  SDLK_3:
                        cur_cpu->key[0x3] =1;
                    break;
                    case  SDLK_4:
                        cur_cpu->key[0xc] =1;
                    break;
                    case  SDLK_q:
                        cur_cpu->key[0x4] =1;
                    break;
                    case  SDLK_w:
                        cur_cpu->key[0x5] =1;
                    break;
                    case  SDLK_e:
                        cur_cpu->key[0x6] =1;
                    break;
                    case  SDLK_r:
                        cur_cpu->key[0xD] =1;
                    case  SDLK_a:
                        cur_cpu->key[0x7] =1;
                    break;
                    case  SDLK_s:
                        cur_cpu->key[0x8] =1;
                    break;
                    case  SDLK_d:
                        cur_cpu->key[0x9] =1;
                    break;
                    case  SDLK_f:
                        cur_cpu->key[0xE] =1;
                    case  SDLK_z:
                        cur_cpu->key[0xA] =1;
                    break;
                    case  SDLK_x:
                        cur_cpu->key[0x0] =1;
                    break;
                    case  SDLK_c:
                        cur_cpu->key[0xB] =1;
                    break;
                    case  SDLK_v:
                        cur_cpu->key[0xF] =1;
                    break;
                    case  SDLK_k:
                       for(int i=0;i<RENDER_W*RENDER_H;i++)
                        {
                            printf("%d", cur_cpu->screen[i]);
                        }   
                        printf("\n");
                    break;
                    case  SDLK_l:
                        state_of_cpu(cur_cpu);
                    break;
                    case  SDLK_p:
                        pause = 1;
                    break;
                    case  SDLK_m:
                        pause = 0;
                    break;
                }
            break;
            case SDL_KEYUP:
                switch(e.key.keysym.sym)
                {
                   case  SDLK_1:
                        cur_cpu->key[0x1] =0;
                    break;
                    case  SDLK_2:
                        cur_cpu->key[0x2] =0;
                    break;
                    case  SDLK_3:
                        cur_cpu->key[0x3] =0;
                    break;
                    case  SDLK_4:
                        cur_cpu->key[0xc] =0;
                    break;
                    case  SDLK_q:
                        cur_cpu->key[0x4] =0;
                    break;
                    case  SDLK_w:
                        cur_cpu->key[0x5] =0;
                    break;
                    case  SDLK_e:
                        cur_cpu->key[0x6] =0;
                    break;
                    case  SDLK_r:
                        cur_cpu->key[0xD] =0;
                    case  SDLK_a:
                        cur_cpu->key[0x7] =0;
                    break;
                    case  SDLK_s:
                        cur_cpu->key[0x8] =0;
                    break;
                    case  SDLK_d:
                        cur_cpu->key[0x9] =0;
                    break;
                    case  SDLK_f:
                        cur_cpu->key[0xE] =0;
                    case  SDLK_z:
                        cur_cpu->key[0xA] =0;
                    break;
                    case  SDLK_x:
                        cur_cpu->key[0x0] =0;
                    break;
                    case  SDLK_c:
                        cur_cpu->key[0xB] =0;
                    break;
                    case  SDLK_v:
                        cur_cpu->key[0xF] =0;
                    break;
                }
            break;
          }
        }
    }
    free(cur_cpu);
    terminate();
    return 0;
    
}