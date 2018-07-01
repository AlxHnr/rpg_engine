#include <SDL.h>
#include <SDL_ttf.h>

#include <menu.h>
#include <user_interface.h>
#include <settings.h>
#include <joystick.h>
#include <game.h>

#ifdef __WIN32__
  int WinMain(int argc, char *argv[])
#else
  int main(int argc, char *argv[])
#endif
{
  /* game variables */
  SDL_Surface *screen = NULL;
  Uint8 program_run = MAIN_MENU;
  
  char *player_path[PLAYER_MAX];
  int team_list[PLAYER_MAX] = {1, 2, 3, 4};
  char *map_path;
  
  /* init SDL start */
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    exit(EXIT_FAILURE);
  atexit(SDL_Quit);
  
  load_settings();
  if(get_fullscreen_toggle())
    screen = SDL_SetVideoMode(get_resolution_x(), get_resolution_y(), 32, SDL_HWSURFACE | SDL_FULLSCREEN);
  else
    screen = SDL_SetVideoMode(get_resolution_x(), get_resolution_y(), 32, SDL_HWSURFACE);
  
  if(screen == NULL)
    exit(EXIT_FAILURE);
  
  /* SDL ttf */
  if(TTF_Init() < 0)
    exit(EXIT_FAILURE);
  atexit(TTF_Quit);
  
  /* SDL mixer */
  if(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 256) < 0)
    exit(EXIT_FAILURE);
  
  atexit(Mix_CloseAudio);
  Mix_VolumeMusic(get_music_volume());
  Mix_Volume(-1, get_sfx_volume());
  
  /* others */
  load_joystick();
  atexit(free_joystick);
  
  if(load_menu() == -1)
    exit(EXIT_FAILURE);
  atexit(free_menu);
  
  if(load_damage() == -1)
    exit(EXIT_FAILURE);
  atexit(free_damage);
  
  if(load_user_interface() == -1)
    exit(EXIT_FAILURE);
  atexit(free_user_interface);
  
  SDL_ShowCursor(SDL_DISABLE);
  /* init SDL end */
  
  /* main loop start */
  while(program_run != EXIT_MENU)
  {
    if(program_run == MAIN_MENU)
      program_run = call_main_menu(screen);
    else if(program_run == PLAYER_2_MENU)
    {
      if((map_path = call_map_select(screen)) == NULL)
      {
        program_run = MAIN_MENU;
        continue;
      }
      
      reset_all_buttons();
      if((player_path[0] = call_character_select(screen, 0, team_list)) == NULL)
      {
        program_run = MAIN_MENU;
        continue;
      }
      
      reset_all_buttons();
      if((player_path[1] = call_character_select(screen, 1, team_list)) == NULL)
      {
        program_run = MAIN_MENU;
        continue;
      }
      
      player_path[2] = NULL;
      player_path[3] = NULL;
      
      blit_loading_screen(screen);
      if(load_map(map_path) == -1)
      {
        program_run = MAIN_MENU;
        continue;
      }
      program_run = start_game((const char **)player_path, team_list, screen);
    }
    else if(program_run == PLAYER_3_MENU)
    {
      if((map_path = call_map_select(screen)) == NULL)
      {
        program_run = MAIN_MENU;
        continue;
      }
      
      reset_all_buttons();
      if((player_path[0] = call_character_select(screen, 0, team_list)) == NULL)
      {
        program_run = MAIN_MENU;
        continue;
      }
      
      reset_all_buttons();
      if((player_path[1] = call_character_select(screen, 1, team_list)) == NULL)
      {
        program_run = MAIN_MENU;
        continue;
      }
      
      reset_all_buttons();
      if((player_path[2] = call_character_select(screen, 2, team_list)) == NULL)
      {
        program_run = MAIN_MENU;
        continue;
      }
      
      player_path[3] = NULL;
      
      blit_loading_screen(screen);
      if(load_map(map_path) == -1)
      {
        program_run = MAIN_MENU;
        continue;
      }
      program_run = start_game((const char **)player_path, team_list, screen);
    }
    else if(program_run == PLAYER_4_MENU)
    {
      if((map_path = call_map_select(screen)) == NULL)
      {
        program_run = MAIN_MENU;
        continue;
      }
      
      reset_all_buttons();
      if((player_path[0] = call_character_select(screen, 0, team_list)) == NULL)
      {
        program_run = MAIN_MENU;
        continue;
      }
      
      reset_all_buttons();
      if((player_path[1] = call_character_select(screen, 1, team_list)) == NULL)
      {
        program_run = MAIN_MENU;
        continue;
      }
      
      reset_all_buttons();
      if((player_path[2] = call_character_select(screen, 2, team_list)) == NULL)
      {
        program_run = MAIN_MENU;
        continue;
      }
      
      reset_all_buttons();
      if((player_path[3] = call_character_select(screen, 3, team_list)) == NULL)
      {
        program_run = MAIN_MENU;
        continue;
      }
      
      blit_loading_screen(screen);
      if(load_map(map_path) == -1)
      {
        program_run = MAIN_MENU;
        continue;
      }
      program_run = start_game((const char **)player_path, team_list, screen);
    }
    else if(program_run == RESTART_BATTLE_MENU)
    {
      blit_loading_screen(screen);
      load_map(map_path);
      program_run = start_game((const char **)player_path, team_list, screen);
    }
    else if(program_run == SELECT_CHARACTER_MENU)
    {
      if(player_path[3] != NULL)
        program_run = PLAYER_4_MENU;
      else if(player_path[2] != NULL)
        program_run = PLAYER_3_MENU;
      else if(player_path[1] != NULL)
        program_run = PLAYER_2_MENU;
      else
        program_run = MAIN_MENU;
    }
    else if(program_run == SAVE_MENU)
    {
      if(get_fullscreen_toggle())
        screen = SDL_SetVideoMode(get_resolution_x(), get_resolution_y(), 32, SDL_HWSURFACE | SDL_FULLSCREEN);
      else
        screen = SDL_SetVideoMode(get_resolution_x(), get_resolution_y(), 32, SDL_HWSURFACE);
      
      Mix_VolumeMusic(get_music_volume());
      Mix_Volume(-1, get_sfx_volume());
      
      program_run = MAIN_MENU;
    }
    else if(program_run == SETTING_MENU)
      program_run = call_setting_menu(screen);
    
  }
  /* main loop end */
  
  return EXIT_SUCCESS;
}