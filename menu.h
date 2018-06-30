#ifndef MENU_H
#define MENU_H

#include <dirent.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_gfxPrimitives.h>
#include <graphic.h>
#include <settings.h>
#include <joystick.h>
#include <framerate.h>

#define MAIN_MENU 0
#define PLAYER_2_MENU 1
#define PLAYER_3_MENU 2
#define PLAYER_4_MENU 3
#define SETTING_MENU 4
#define EXIT_MENU 5

#define GAME_MENU 6
#define RESTART_BATTLE_MENU 7
#define SELECT_CHARACTER_MENU 8
#define SAVE_MENU 9

#define GAME_RUNNING 10

#define MENU_ITEM_SPACE 5

#define MAIN_MENU_ITEMS 5
#define GAME_MENU_ITEMS 5
#define SETTING_MENU_ITEMS (6 + PLAYER_MAX)

#define MENU_BOX_FRAME_SIZE 8

struct menu_item{
  char path[1024];
  
  SDL_Surface *text_surface;
};

typedef struct new_menu_list{
  int list_items;
  struct menu_item *list;
}new_menu_list;

int load_menu();
void free_menu();

void blit_loading_screen(SDL_Surface *screen);
int call_main_menu(SDL_Surface *screen);
int call_setting_menu(SDL_Surface *screen);
int call_game_menu(SDL_Surface *screen);
char *call_map_select(SDL_Surface *screen);
char *call_character_select(SDL_Surface *screen, const int player, int *team_list);

#endif