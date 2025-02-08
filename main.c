
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

// System flags
bool SDL_InitComplete;
bool lFlagQuit;

// animation frame names
static struct dirent **namelist = NULL;            /* dirent structure to hold listing */
// file names filtering
int sdfilt (const struct dirent *de);

// string concatenation
char* concat(const char* str1, const char* str2);

// userspace flags
static bool lBootAnimRendered;

// init defines
int SDL_Init_step(void);

// system routines
void load_render_anim_direct(const char *boot_patch, SDL_Event *e, const int frame_delay);
bool checkquitevent(SDL_Event *e);

// main menu routines
char* get_animation_patch(SDL_Event *e);
void render_current_screen(SDL_Event *e, const int frame_delay);

// magic defines
#define DEF_SCREEN_WIDTH 800
#define DEF_SCREEN_HEIGHT 480

#ifndef MAGIC_DEMO_MODE
    #define MAGIC_DEMO_MODE true
#endif

#ifndef MAGIC_DEMO_DELAY
    #define MAGIC_DEMO_DELAY 3000
#endif


#define DEF_ONE_SHOT_FRAMES 12   // more frames - more memory use!
#define DEF_FRAME_DELAY 12       // avg delay (16) animation speed
#define DEF_TICK_INTERVAL 30

#define IMG_BOOT_PATH   "jpg/boot/"
#define IMG_STAT_PATH   "jpg/root/00-status/"
#define IMG_INV_PATH    "jpg/root/01-inv/"
#define IMG_DATA_PATH   "jpg/root/02-data/"
#define IMG_MAP_PATH    "jpg/root/03-map/"
#define IMG_RADIO_PATH  "jpg/root/04-radio/"
#define IMG_MAIL_PATH   "jpg/root/05-email/"
#define IMG_SETUP_PATH  "jpg/root/06-settings/"

// main menu variables
enum OS_MainMenu {
    MM_STATUS = 0,
    MM_INV,
    MM_DATA,
    MM_MAP,
    MM_RADIO,
};

typedef enum OS_MainMenu *POS_MainMenu, **LPOS_MainMenu;
static enum OS_MainMenu curren_mainmenu_itm = MM_STATUS;

/// @brief inline definitions
/// @param e SDL_Event
bool checkquitevent(SDL_Event *e){
    if (SDL_PollEvent(e))
    {
        if (e->type == SDL_QUIT)
        {
            lFlagQuit = true;
            return true;
        }
    }
    return false;
}

// calculate left time interval for next event
uint32_t timeLeft(void){
    static uint32_t next_time = 0;
    uint32_t now_time;
    now_time = SDL_GetTicks();
    if(next_time <= now_time){
        next_time = now_time + DEF_TICK_INTERVAL;
        return 0;
    }
    return next_time - now_time;
}


// Just type [F5] to start debugging!
int main(int argc, char *argv[]) {       
    static uint32_t next_time = 0;
    lBootAnimRendered = false;
    lFlagQuit = false;

    SDL_InitComplete = (0 == SDL_Init_step());

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
        
        if(!lBootAnimRendered){
            load_render_anim_direct(IMG_BOOT_PATH, &e, DEF_FRAME_DELAY);
            lBootAnimRendered = true;
            next_time = SDL_GetTicks() + MAGIC_DEMO_DELAY;
        }
        if(lBootAnimRendered){
            render_current_screen(&e, DEF_FRAME_DELAY);            
        }
        if(MAGIC_DEMO_MODE){
            uint32_t current_time = SDL_GetTicks();
            if( current_time <= next_time)
                continue;
            curren_mainmenu_itm++;

            if(curren_mainmenu_itm > MM_RADIO)
                curren_mainmenu_itm = MM_STATUS;
                next_time = SDL_GetTicks() + MAGIC_DEMO_DELAY;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

int read_anim_files(const char *dname){
    int ndir = scandir (dname, &namelist, sdfilt, alphasort);
    if (ndir < 0)
    {
        perror("scandir return failed");  /* throw error & return on failure  */
        return -1;
    }
    return ndir;
}

void render_current_screen(SDL_Event *e, const int frame_delay){
    char *current_path = get_animation_patch(e);
    load_render_anim_direct(current_path, e, DEF_FRAME_DELAY);
}

char* get_animation_patch(SDL_Event *e){    

    checkquitevent(e);
    switch (curren_mainmenu_itm){
        case MM_STATUS:{
            return IMG_STAT_PATH;
        }
        case MM_INV:{
            return IMG_INV_PATH;
        }
        case MM_DATA:{
            return IMG_DATA_PATH;
        }
        case MM_MAP:{
            return IMG_MAP_PATH;
        }
        case MM_RADIO:{
            return IMG_RADIO_PATH;
        }
        default:{
            return IMG_STAT_PATH;
        }
    }
    return IMG_STAT_PATH;
}

void load_render_anim_direct(const char *boot_patch, SDL_Event *e, const int frame_delay){    
    SDL_Texture* texture_array[DEF_ONE_SHOT_FRAMES];
    int ndir = read_anim_files(boot_patch);
    
    if(ndir<=0)
        return;

    for(int i  = 0; i < ndir; i++){
        int textures_loaded = 0;
        for(int j = 0; j < DEF_ONE_SHOT_FRAMES && i < ndir; j++){        
            if(checkquitevent(e)){
                return;
            };

            char *sname = namelist[i]->d_name; 
            SDL_Surface* surface = IMG_Load(concat(boot_patch, sname));
            if(NULL == surface){
                printf("Error loading image: %s", IMG_GetError());
                continue;
            }

            free(namelist[i]);

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
            SDL_Delay(frame_delay);
        }
    }
    free(namelist);
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

    window = SDL_CreateWindow("First program", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DEF_SCREEN_WIDTH, DEF_SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
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



// Example of animation from disk
// void render_disk_anim(SDL_Event *e){
//     for(int i  = 0; i < ndir; i++){
//         int textures_loaded = 0;
//         for(int j = 0; j < ONE_SHOT_FRAMES && i < ndir; j++){        
//             if (SDL_PollEvent(e))
//             {
//                 if (e->type == SDL_QUIT)
//                 {
//                     lFlagQuit = true;
//                     return;
//                 }
//             }            
            
//             char *sname = namelist[i]->d_name; 
//             SDL_Surface* surface = IMG_Load(concat("jpg/", sname));
//             if(NULL == surface){
//                 printf("Error loading image: %s", IMG_GetError());
//                 continue;
//             }

//             texture_array[j] = SDL_CreateTextureFromSurface(renderer, surface);
//             if(NULL == texture_array[j]){
//                 printf( "Error creating texture" );
//                 continue;
//             }
            
//             SDL_FreeSurface(surface);
//             textures_loaded++;
//             i++;
//         }

//         for(int animframe = 0; animframe < textures_loaded; animframe++){
//             SDL_RenderClear(renderer);
//             SDL_RenderCopy(renderer, texture_array[animframe], NULL, NULL);
//             SDL_RenderPresent(renderer);
//             SDL_DestroyTexture(texture_array[animframe]);
//             SDL_Delay(21);
//         }
//     }
// }

// Example load files
// int filesfromdir(void){
//     const char *dname = "jpg/";

//     /* call scandir to fill pointer to array of dirent entries  */
//     if ((ndir = scandir (dname, &namelist, sdfilt, alphasort)) < 0)
//     {
//         perror("scandir");  /* throw error & return on failure  */
//         return 1;
//     }

//     // size_t it = 0;                              /* simple iterator for dir list     */
//     // /* print each of the entries in alphasort order  */
//     // printf ("\nscandir example (alphasort):\n\n");
//     // for (it = 0; it < ndir; it++)
//     //     printf("  nl[%2zu] %s\n", it, namelist[it]->d_name);

//     // /* print each entry in reverse sort order & free */
//     // printf ("\nreverse:\n\n");
//     // it = ndir;
//     // SDL_UNUSED char *sname;
//     // while (it--) {
//     //     sname = namelist[it]->d_name;
//     //     printf("  nl[%2zu] %s\n", it, sname);
//     //     if (namelist[it]->d_name)
//     //         free (namelist[it]);
//     // }
//     // free(namelist);

//     // printf ("\n");

//     return 0;
// }



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