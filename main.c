
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

/*
#ifndef _BSD_SOURCE     // for scandir sort routines
#define _BSD_SOURCE
#endif
*/

#include <stdbool.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>  /* opendir  */
#include <dirent.h>     /* opendir, readdir, scandir  */
#include <errno.h>

#include "SDL.h"
#include "SDL_image.h"

#ifndef __weak
#define __weak __attribute__((weak))
#endif

SDL_Window *window;
SDL_Renderer *renderer;

bool SDL_InitComplete;
bool lFlagQuit;

int ndir = 0;                               /* num dirs scandir returns, -1 err */
struct dirent **namelist = NULL;            /* dirent structure to hold listing */

int sdfilt (const struct dirent *de);
int filesfromdir(void); 
char* concat(const char* str1, const char* str2);

int SDL_Init_step(void);
void render_disk_anim(SDL_Event *e);


#define ONE_SHOT_FRAMES 24
SDL_Texture* texture_array[ONE_SHOT_FRAMES];

// Just type [F5] to start debugging!
int main(int argc, char *argv[]) {   

    lFlagQuit = false;

    SDL_InitComplete = (0 == SDL_Init_step());
    filesfromdir(); 

    if(!SDL_InitComplete)
        return 100;


    while (true)
    {
        SDL_Event e;
        if (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT || lFlagQuit)
            {
                break;
            }
        }

        render_disk_anim(&e);
    }

    // SDL_DestroyTexture(lettuce_tex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

void render_disk_anim(SDL_Event *e){
    for(int i  = 0; i < ndir; i++){
        int textures_loaded = 0;
        for(int j = 0; j < ONE_SHOT_FRAMES && i < ndir; j++){        
            if (SDL_PollEvent(e))
            {
                if (e->type == SDL_QUIT)
                {
                    lFlagQuit = true;
                    return;
                }
            }            
            
            char *sname = namelist[i]->d_name; 
            SDL_Surface* surface = IMG_Load(concat("jpg/", sname));
            if(NULL == surface){
                printf("Error loading image: %s", IMG_GetError());
                continue;
            }

            texture_array[j] = SDL_CreateTextureFromSurface(renderer, surface);
            if(NULL == texture_array[j]){
                printf( "Error creating texture" );
                continue;
            }
            
            SDL_FreeSurface(surface);
            textures_loaded++;
            i++;
        }

        for(int animframe = 0; animframe < textures_loaded; animframe++){
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture_array[animframe], NULL, NULL);
            SDL_RenderPresent(renderer);
            SDL_DestroyTexture(texture_array[animframe]);
            SDL_Delay(21);
        }
    }
}

int SDL_Init_step(void){

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        printf("Error SDL2 Initialization : %s",SDL_GetError());
        return 1;
    }

    if (IMG_Init(IMG_INIT_JPG) == 0)
    {
        printf("Error SDL2_image Initialization");
        return 2;
    }

    window = SDL_CreateWindow("First program", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 480, SDL_WINDOW_OPENGL);
    if (window == NULL)
    {
        printf("Error window creation");
        return 3;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        printf("Error renderer creation");
        return 4;
    }

    return 0;
}

int filesfromdir(void){
    const char *dname = "jpg/";

    /* call scandir to fill pointer to array of dirent entries  */
    if ((ndir = scandir (dname, &namelist, sdfilt, alphasort)) < 0)
    {
        perror("scandir");  /* throw error & return on failure  */
        return 1;
    }

    // size_t it = 0;                              /* simple iterator for dir list     */
    // /* print each of the entries in alphasort order  */
    // printf ("\nscandir example (alphasort):\n\n");
    // for (it = 0; it < ndir; it++)
    //     printf("  nl[%2zu] %s\n", it, namelist[it]->d_name);

    // /* print each entry in reverse sort order & free */
    // printf ("\nreverse:\n\n");
    // it = ndir;
    // SDL_UNUSED char *sname;
    // while (it--) {
    //     sname = namelist[it]->d_name;
    //     printf("  nl[%2zu] %s\n", it, sname);
    //     if (namelist[it]->d_name)
    //         free (namelist[it]);
    // }
    // free(namelist);

    // printf ("\n");

    return 0;
}



/* simple scandir filter that omit strings and
dot files '.' and '..' from dirent entries */
int sdfilt (const struct dirent *de)
{
    if (strcmp (de->d_name, ".") == 0 || strcmp (de->d_name, "..") == 0)
        return 0;
    else
        return 1;
}

char* concat(const char* str1, const char* str2)
{
    char* result;
    asprintf(&result, "%s%s", str1, str2);
    return result;
}

void pipboyboxinit(void){

}