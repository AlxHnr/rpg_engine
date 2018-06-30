#include <menu.h>

SDL_Surface *background = NULL;
SDL_Surface *menu_box = NULL;
SDL_Surface *team_box = NULL;
SDL_Surface *select_field = NULL;

SDL_Surface *volume_bar = NULL;
SDL_Surface *volume_controller = NULL;

SDL_Surface *toggle_off = NULL;
SDL_Surface *toggle_on = NULL;

SDL_Surface *loading_screen = NULL;
SDL_Surface *loading_screen_text = NULL;
static const SDL_Color loading_screen_color = {200, 200, 200};

Mix_Chunk *toggle_sound = NULL;
Mix_Chunk *select_sound = NULL;
Mix_Chunk *abort_sound = NULL;

TTF_Font *menu_font = NULL;
static const SDL_Color menu_color = {45, 35, 35};

/* menu text surfaces */
static new_menu_list main_menu;
static new_menu_list game_menu;
static new_menu_list setting_menu;

static new_menu_list map_menu;
static new_menu_list character_menu;

static int count_directory(const char *path)
{
  int counter = 0;
  
  DIR *program_dir = NULL;
  struct dirent *dir_pointer = NULL;
  
  if((program_dir = opendir(path)) != NULL)
  {
    while((dir_pointer = readdir(program_dir)) != NULL)
    {
      if(dir_pointer->d_name[0] != '.')
        counter++;
    }
    
    closedir(program_dir);
  }
  
  return counter;
}
static void set_buttons(SDL_Surface *screen, const int player)
{
  SDL_Rect background_src = {(int)(background->w/2 - screen->w/2), (int)(background->h/2 - screen->h/2), screen->w, screen->h};
  SDL_Rect menu_box_dest = {(int)(screen->w/2 - menu_box->w/2), (int)(screen->h - menu_box->h * 1.2), 0, 0};
  
  SDL_Surface *cache_text = NULL;
  SDL_Rect cache_text_dest = {(int)(screen->w/2 - menu_box->w/2), (int)(screen->h - menu_box->h * 1.2 + select_field->h * 2), 0, 0};
  
  SDL_Event cache_event;
  
  int counter = 0;
  while(counter < BUTTON_MAX)
  {
    SDL_FreeSurface(cache_text);
    
    /* set menu text */
    if(counter == BUTTON_LEFT)
      cache_text = TTF_RenderText_Blended(menu_font, "Press Button Left...", menu_color);
    else if(counter == BUTTON_RIGHT)
      cache_text = TTF_RenderText_Blended(menu_font, "Press Button Right...", menu_color);
    else if(counter == BUTTON_UP)
      cache_text = TTF_RenderText_Blended(menu_font, "Press Button Up...", menu_color);
    else if(counter == BUTTON_DOWN)
      cache_text = TTF_RenderText_Blended(menu_font, "Press Button Down...", menu_color);
    else if(counter == BUTTON_1)
      cache_text = TTF_RenderText_Blended(menu_font, "Press Button 1...", menu_color);
    else if(counter == BUTTON_2)
      cache_text = TTF_RenderText_Blended(menu_font, "Press Button 2...", menu_color);
    else if(counter == BUTTON_3)
      cache_text = TTF_RenderText_Blended(menu_font, "Press Button 3...", menu_color);
    else if(counter == BUTTON_4)
      cache_text = TTF_RenderText_Blended(menu_font, "Press Button 4...", menu_color);
    else if(counter == BUTTON_5)
      cache_text = TTF_RenderText_Blended(menu_font, "Press Button 5...", menu_color);
    else if(counter == BUTTON_6)
      cache_text = TTF_RenderText_Blended(menu_font, "Press Button 6...", menu_color);
    else if(counter == BUTTON_7)
      cache_text = TTF_RenderText_Blended(menu_font, "Press Button 7...", menu_color);
    else if(counter == BUTTON_8)
      cache_text = TTF_RenderText_Blended(menu_font, "Press Button 8...", menu_color);
    else if(counter == BUTTON_PAUSE)
      cache_text = TTF_RenderText_Blended(menu_font, "Press Pause Button...", menu_color);
    else if(counter == BUTTON_OK)
      cache_text = TTF_RenderText_Blended(menu_font, "Press OK Button...", menu_color);
    else
      cache_text = TTF_RenderText_Blended(menu_font, "Press Abort Button...", menu_color);
    
    /* blit everything */
    SDL_BlitSurface(background, &background_src, screen, NULL);
    SDL_BlitSurface(menu_box, NULL, screen, &menu_box_dest);
    
    cache_text_dest.x = (int)(screen->w/2 - cache_text->w/2);
    SDL_BlitSurface(cache_text, NULL, screen, &cache_text_dest);
    SDL_Flip(screen);
    
    /* avoid to read in axis button up */
    if(cache_event.type == SDL_JOYAXISMOTION)
      SDL_WaitEvent(&cache_event);
    
    SDL_WaitEvent(&cache_event);
    
    if(cache_event.type != SDL_KEYDOWN && cache_event.type != SDL_JOYBUTTONDOWN && cache_event.type != SDL_JOYAXISMOTION)
      continue;
      
    Mix_PlayChannel(-1, toggle_sound, 0);
    hard_set_button_state(player, counter, cache_event);
    
    counter++;
  }
  
  SDL_FreeSurface(cache_text);
}

int load_menu()
{
  int counter;
  
  char cache_string[1024];
  DIR *program_dir = NULL;
  struct dirent *dir_pointer = NULL;
  
  /* load graphics */
  if((background = load_image("gui/background.png")) == NULL)
    return -1;
  if((menu_box = load_image("gui/menu_box.png")) == NULL)
    return -1;
  if((team_box = load_image("gui/team_box.png")) == NULL)
    return -1;
  if((select_field = load_image("gui/select_field.png")) == NULL)
    return -1;
  
  if((volume_bar = load_image("gui/volume_bar.png")) == NULL)
    return -1;
  if((volume_controller = load_image("gui/volume_controller.png")) == NULL)
    return -1;
  
  if((toggle_off = load_image("gui/toggle_off.png")) == NULL)
    return -1;
  if((toggle_on = load_image("gui/toggle_on.png")) == NULL)
    return -1;
  
  if((loading_screen = load_image("gui/loading_screen.png")) == NULL)
    return -1;
  
  /* load sounds */
  if((toggle_sound = Mix_LoadWAV("gui/toggle.wav")) == NULL)
    return -1;
  if((select_sound = Mix_LoadWAV("gui/select.wav")) == NULL)
    return -1;
  if((abort_sound = Mix_LoadWAV("gui/abort.wav")) == NULL)
    return -1;
  
  /* load font */
  if((menu_font = TTF_OpenFont("amiri-regular.ttf", 30)) == NULL)
    return -1;
  loading_screen_text = TTF_RenderText_Blended(menu_font, "Loading...", loading_screen_color);
  
  /* main menu */
  main_menu.list_items = MAIN_MENU_ITEMS;
  if((main_menu.list = malloc(sizeof(struct menu_item) * main_menu.list_items)) == NULL)
    return -1;
  
  main_menu.list[0].text_surface = TTF_RenderText_Blended(menu_font, "2 Player Game", menu_color);
  main_menu.list[1].text_surface = TTF_RenderText_Blended(menu_font, "3 Player Game", menu_color);
  main_menu.list[2].text_surface = TTF_RenderText_Blended(menu_font, "4 Player Game", menu_color);
  main_menu.list[3].text_surface = TTF_RenderText_Blended(menu_font, "Options", menu_color);
  main_menu.list[4].text_surface = TTF_RenderText_Blended(menu_font, "Exit Game", menu_color);
  
  /* in game menu */
  game_menu.list_items = GAME_MENU_ITEMS;
  if((game_menu.list = malloc(sizeof(struct menu_item) * game_menu.list_items)) == NULL)
    return -1;
  
  game_menu.list[0].text_surface = TTF_RenderText_Blended(menu_font, "Continue", menu_color);
  game_menu.list[1].text_surface = TTF_RenderText_Blended(menu_font, "Restart Battle", menu_color);
  game_menu.list[2].text_surface = TTF_RenderText_Blended(menu_font, "Select Character", menu_color);
  game_menu.list[3].text_surface = TTF_RenderText_Blended(menu_font, "Main Menu", menu_color);
  game_menu.list[4].text_surface = TTF_RenderText_Blended(menu_font, "Exit Game", menu_color);
  
  /* setting menu */
  setting_menu.list_items = SETTING_MENU_ITEMS;
  if((setting_menu.list = malloc(sizeof(struct menu_item) * setting_menu.list_items)) == NULL)
    return -1;
  
  /* resolution text surface = NULL */
  setting_menu.list[0].text_surface = NULL;
  setting_menu.list[1].text_surface = TTF_RenderText_Blended(menu_font, "Fullscreen", menu_color);
  setting_menu.list[2].text_surface = TTF_RenderText_Blended(menu_font, "Music Volume", menu_color);
  setting_menu.list[3].text_surface = TTF_RenderText_Blended(menu_font, "Sound Volume", menu_color);
  
  counter = 0;
  while(counter < PLAYER_MAX)
  {
    sprintf(cache_string, "Set Player %i Buttons", counter + 1);
    setting_menu.list[counter + 4].text_surface = TTF_RenderText_Blended(menu_font, cache_string, menu_color);
    
    counter++;
  }
  
  setting_menu.list[4 + PLAYER_MAX].text_surface = TTF_RenderText_Blended(menu_font, "Save and Exit Settings", menu_color);
  setting_menu.list[5 + PLAYER_MAX].text_surface = TTF_RenderText_Blended(menu_font, "Exit Settings", menu_color);
  
  /* map menu */
  /* +1 for the last "back" entry in map list */
  map_menu.list_items = count_directory("maps") + 1;
  
  if((map_menu.list = malloc(sizeof(struct menu_item) * map_menu.list_items)) == NULL)
    return -1;
  
  if((program_dir = opendir("maps")) != NULL)
  {
    counter = 0;
    while((dir_pointer = readdir(program_dir)) != NULL && counter < map_menu.list_items - 1)
    {
      if(dir_pointer->d_name[0] != '.')
      {
        sprintf(map_menu.list[counter].path, "maps/%s", dir_pointer->d_name);
        map_menu.list[counter].text_surface = TTF_RenderText_Blended(menu_font, dir_pointer->d_name, menu_color);
        
        counter++;
      }
    }
    
    closedir(program_dir);
  }
  /* the last item */
  map_menu.list[map_menu.list_items - 1].text_surface = TTF_RenderText_Blended(menu_font, "Exit", menu_color);
  
  /* character menu */
  character_menu.list_items = count_directory("character") + 1;
  if((character_menu.list = malloc(sizeof(struct menu_item) * character_menu.list_items)) == NULL)
    return -1;
  
  if((program_dir = opendir("character")) != NULL)
  {
    counter = 0;
    while((dir_pointer = readdir(program_dir)) != NULL && counter < character_menu.list_items - 1)
    {
      if(dir_pointer->d_name[0] != '.')
      {
        sprintf(character_menu.list[counter].path, "%s", dir_pointer->d_name);
        character_menu.list[counter].text_surface = TTF_RenderText_Blended(menu_font, dir_pointer->d_name, menu_color);
        
        counter++;
      }
    }
    
    closedir(program_dir);
  }
  /* the last item */
  character_menu.list[character_menu.list_items - 1].text_surface = TTF_RenderText_Blended(menu_font, "Exit", menu_color);
  
  return 0;
}
void free_menu()
{
  int counter;
  
  /* free graphics */
  SDL_FreeSurface(background);
  SDL_FreeSurface(menu_box);
  SDL_FreeSurface(team_box);
  SDL_FreeSurface(select_field);
  
  SDL_FreeSurface(volume_bar);
  SDL_FreeSurface(volume_controller);
  
  SDL_FreeSurface(toggle_off);
  SDL_FreeSurface(toggle_on);
  
  SDL_FreeSurface(loading_screen);
  SDL_FreeSurface(loading_screen_text);
  
  /* free sounds */
  Mix_HaltChannel(-1);
  Mix_FreeChunk(toggle_sound);
  Mix_FreeChunk(select_sound);
  Mix_FreeChunk(abort_sound);
  
  /* free font */
  TTF_CloseFont(menu_font);
  
  /* free main menu list */
  counter = 0;
  while(counter < main_menu.list_items)
  {
    SDL_FreeSurface(main_menu.list[counter].text_surface);
    
    counter++;
  }
  free(main_menu.list);
  
  /* free game menu list */
  counter = 0;
  while(counter < game_menu.list_items)
  {
    SDL_FreeSurface(game_menu.list[counter].text_surface);
    
    counter++;
  }
  free(game_menu.list);
  
  /* free setting menu list */
  /* counter = 1, because setting_menu.list->resolution is ignored */
  counter = 1;
  while(counter < setting_menu.list_items)
  {
    SDL_FreeSurface(setting_menu.list[counter].text_surface);
    
    counter++;
  }
  free(setting_menu.list);
  
  /* free map menu and the last item */
  counter = 0;
  while(counter < map_menu.list_items)
  {
    SDL_FreeSurface(map_menu.list[counter].text_surface);
    
    counter++;
  }
  
  free(map_menu.list);
  
  /* free character menu and the last item */
  counter = 0;
  while(counter < character_menu.list_items)
  {
    SDL_FreeSurface(character_menu.list[counter].text_surface);
    
    counter++;
  }
  
  free(character_menu.list);
}

void blit_loading_screen(SDL_Surface *screen)
{
  SDL_Rect loading_screen_src = {(int)(loading_screen->w/2 - screen->w/2), (int)(loading_screen->h/2 - screen->h/2), screen->w, screen->h};
  SDL_Rect loading_screen_text_dest = {(int)(screen->w/2 - loading_screen_text->w/2), screen->h - loading_screen_text->h, 0, 0};
  
  SDL_BlitSurface(loading_screen, &loading_screen_src, screen, NULL);
  SDL_BlitSurface(loading_screen_text, NULL, screen, &loading_screen_text_dest);
  SDL_Flip(screen);
}
int call_main_menu(SDL_Surface *screen)
{
  int return_value = MAIN_MENU;
  
  SDL_Rect cache_text_dest;
  int cache_menu_item;
  
  SDL_Rect background_src = {(int)(background->w/2 - screen->w/2), (int)(background->h/2 - screen->h/2), screen->w, screen->h};
  SDL_Rect menu_box_dest = {(int)(screen->w/2 - menu_box->w/2), (int)(screen->h - menu_box->h * 1.2), 0, 0};
  SDL_Rect select_field_dest = {(int)(screen->w/2 - menu_box->w/2) + MENU_BOX_FRAME_SIZE, menu_box_dest.y + MENU_BOX_FRAME_SIZE, 0, 0};
  
  /* static variables to keep select_field at place */
  static int current_menu_item = 0;
  
  cache_menu_item = 0;
  while(cache_menu_item < current_menu_item)
  {
    select_field_dest.y += select_field->h;
    
    cache_menu_item++;
  }
  
  /* main menu loop */
  while(return_value == MAIN_MENU)
  {
    update_button_state();
    
    /* handle user_input */
    if(get_all_button_state(BUTTON_DOWN))
    {
      set_all_button_state(BUTTON_DOWN, 0);
      Mix_PlayChannel(-1, toggle_sound, 0);
      
      select_field_dest.y += select_field->h;
      current_menu_item++;
    }
    else if(get_all_button_state(BUTTON_UP))
    {
      set_all_button_state(BUTTON_UP, 0);
      Mix_PlayChannel(-1, toggle_sound, 0);
      
      select_field_dest.y -= select_field->h;
      current_menu_item--;
    }
    else if(get_all_button_state(BUTTON_OK))
    {
      set_all_button_state(BUTTON_OK, 0);
      Mix_PlayChannel(-1, select_sound, 0);
      
      switch(current_menu_item)
      {
        case 0:
          return_value = PLAYER_2_MENU;
          break;
        case 1:
          return_value = PLAYER_3_MENU;
          break;
        case 2:
          return_value = PLAYER_4_MENU;
          break;
        case 3:
          return_value = SETTING_MENU;
          break;
        case 4:
          return_value = EXIT_MENU;
          break;
      }
    }
    
    /* correct current_menu_item and select field dest */
    if(select_field_dest.y < menu_box_dest.y + MENU_BOX_FRAME_SIZE)
      select_field_dest.y = menu_box_dest.y + MENU_BOX_FRAME_SIZE;
    else if(select_field_dest.y > menu_box_dest.y + MENU_BOX_FRAME_SIZE + (MENU_ITEM_SPACE - 1) * select_field->h)
      select_field_dest.y = menu_box_dest.y + MENU_BOX_FRAME_SIZE + (MENU_ITEM_SPACE - 1) * select_field->h;
    
    if(current_menu_item < 0)
    {
      select_field_dest.y = menu_box_dest.y + MENU_BOX_FRAME_SIZE + (MENU_ITEM_SPACE - 1) * select_field->h;
      current_menu_item = main_menu.list_items - 1;
    }
    else if(current_menu_item >= main_menu.list_items)
    {
      select_field_dest.y = menu_box_dest.y + MENU_BOX_FRAME_SIZE;
      current_menu_item = 0;
    }
    
    /* blit background */
    SDL_BlitSurface(background, &background_src, screen, NULL);
    SDL_BlitSurface(menu_box, NULL, screen, &menu_box_dest);
    
    /* blit menu text */
    cache_text_dest.y = menu_box_dest.y - 4;
    
    cache_menu_item = current_menu_item - (int)((select_field_dest.y - menu_box_dest.y + MENU_BOX_FRAME_SIZE)/select_field->h);
    while(cache_menu_item <= current_menu_item + (int)(((menu_box_dest.y + menu_box->h) - (select_field_dest.y + select_field->h))/select_field->h))
    {
      cache_text_dest.x = (int)(screen->w/2 - main_menu.list[cache_menu_item].text_surface->w/2);
      
      SDL_BlitSurface(main_menu.list[cache_menu_item].text_surface, NULL, screen, &cache_text_dest);
      cache_text_dest.y += select_field->h;
      
      cache_menu_item++;
    }
    
    /* blit select field */
    SDL_BlitSurface(select_field, NULL, screen, &select_field_dest);
    SDL_Flip(screen);
    
    controll_framerate();
  }
  
  return return_value;
}
int call_setting_menu(SDL_Surface *screen)
{
  int return_value = SETTING_MENU;
  
  SDL_Rect background_src = {(int)(background->w/2 - screen->w/2), (int)(background->h/2 - screen->h/2), screen->w, screen->h};
  SDL_Rect menu_box_dest = {(int)(screen->w/2 - menu_box->w/2), (int)(screen->h - menu_box->h * 1.2), 0, 0};
  SDL_Rect select_field_dest = {(int)(screen->w/2 - menu_box->w/2) + MENU_BOX_FRAME_SIZE, menu_box_dest.y + MENU_BOX_FRAME_SIZE, 0, 0};
  
  SDL_Rect cache_text_dest;
  
  int current_menu_item = 0;
  int cache_menu_item;
  
  /* cache setting variables */
  int resolution_x = get_resolution_x();
  int resolution_y = get_resolution_y();
  
  char cache_string[1024];
  sprintf(cache_string, "Resolution: %ix%i", resolution_x, resolution_y);
  setting_menu.list[0].text_surface = TTF_RenderText_Blended(menu_font, cache_string, menu_color);
  
  Sint8 fullscreen_toggle = get_fullscreen_toggle();
  
  int music_volume = get_music_volume();
  int sfx_volume = get_sfx_volume();
  
  /* setting menu loop */
  while(return_value == SETTING_MENU)
  {
    update_button_state();
    
    /* handle user_input */
    if(get_all_button_state(BUTTON_DOWN))
    {
      set_all_button_state(BUTTON_DOWN, 0);
      Mix_PlayChannel(-1, toggle_sound, 0);
      
      select_field_dest.y += select_field->h;
      current_menu_item++;
    }
    else if(get_all_button_state(BUTTON_UP))
    {
      set_all_button_state(BUTTON_UP, 0);
      Mix_PlayChannel(-1, toggle_sound, 0);
      
      select_field_dest.y -= select_field->h;
      current_menu_item--;
    }
    else if(get_all_button_state(BUTTON_LEFT))
    {
      if(current_menu_item == 0)
      {
        set_all_button_state(BUTTON_LEFT, 0);
        Mix_PlayChannel(-1, toggle_sound, 0);
        
        /* resolution */
        if(resolution_x == 800 && resolution_y == 600)
        {
          resolution_x = 1280;
          resolution_y = 800;
        }
        else if(resolution_x == 1152 && resolution_y == 768)
        {
          resolution_x = 800;
          resolution_y = 600;
        }
        else if(resolution_x == 1280 && resolution_y == 720)
        {
          resolution_x = 1152;
          resolution_y = 768;
        }
        else if(resolution_x == 1280 && resolution_y == 768)
        {
          resolution_x = 1280;
          resolution_y = 720;
        }
        else if(resolution_x == 1280 && resolution_y == 800)
        {
          resolution_x = 1280;
          resolution_y = 768;
        }
        
        SDL_FreeSurface(setting_menu.list[0].text_surface);
        sprintf(cache_string, "Resolution: %ix%i", resolution_x, resolution_y);
        setting_menu.list[0].text_surface = TTF_RenderText_Blended(menu_font, cache_string, menu_color);
      }
      else if(current_menu_item == 2)
      {
        /* music volume */
        music_volume -= 2;
        
        if(music_volume < 0)
          music_volume = 0;
      }
      else if(current_menu_item == 3)
      {
        /* sfx volume */
        sfx_volume -= 2;
        
        if(sfx_volume < 0)
          sfx_volume = 0;
      }
    }
    else if(get_all_button_state(BUTTON_RIGHT))
    {
      if(current_menu_item == 0)
      {
        set_all_button_state(BUTTON_RIGHT, 0);
        Mix_PlayChannel(-1, toggle_sound, 0);
        
        /* resolution */
        if(resolution_x == 800 && resolution_y == 600)
        {
          resolution_x = 1152;
          resolution_y = 768;
        }
        else if(resolution_x == 1152 && resolution_y == 768)
        {
          resolution_x = 1280;
          resolution_y = 720;
        }
        else if(resolution_x == 1280 && resolution_y == 720)
        {
          resolution_x = 1280;
          resolution_y = 768;
        }
        else if(resolution_x == 1280 && resolution_y == 768)
        {
          resolution_x = 1280;
          resolution_y = 800;
        }
        else if(resolution_x == 1280 && resolution_y == 800)
        {
          resolution_x = 800;
          resolution_y = 600;
        }
        
        SDL_FreeSurface(setting_menu.list[0].text_surface);
        sprintf(cache_string, "Resolution: %ix%i", resolution_x, resolution_y);
        setting_menu.list[0].text_surface = TTF_RenderText_Blended(menu_font, cache_string, menu_color);
      }
      else if(current_menu_item == 2)
      {
        /* music volume */
        music_volume += 2;
        
        if(music_volume > MIX_MAX_VOLUME)
          music_volume = MIX_MAX_VOLUME;
      }
      else if(current_menu_item == 3)
      {
        /* sfx volume */
        sfx_volume += 2;
        
        if(sfx_volume > MIX_MAX_VOLUME)
          sfx_volume = MIX_MAX_VOLUME;
      }
    }
    else if(get_all_button_state(BUTTON_OK))
    {
      set_all_button_state(BUTTON_OK, 0);
      
      if(current_menu_item == 1)
      {
        Mix_PlayChannel(-1, toggle_sound, 0);
        if(fullscreen_toggle)
          fullscreen_toggle = 0;
        else
          fullscreen_toggle = 1;
      }
      else if(current_menu_item > 3 && current_menu_item <= 3 + PLAYER_MAX)
      {
        Mix_PlayChannel(-1, select_sound, 0);
        set_buttons(screen, current_menu_item - 4);
      }
      else if(current_menu_item == 8)
      {
        Mix_PlayChannel(-1, select_sound, 0);
        return_value = SAVE_MENU;
        
        set_resolution_x(resolution_x);
        set_resolution_y(resolution_y);
        set_fullscreen_toggle(fullscreen_toggle);
        
        set_music_volume(music_volume);
        set_sfx_volume(sfx_volume);
        
        save_settings();
        save_joystick();
      }
      else if(current_menu_item == 9)
      {
        Mix_PlayChannel(-1, select_sound, 0);
        return_value = MAIN_MENU;
      }
    }
    else if(get_all_button_state(BUTTON_ABORT))
    {
      set_all_button_state(BUTTON_ABORT, 0);
      Mix_PlayChannel(-1, abort_sound, 0);
      
      return_value = MAIN_MENU;
    }
    
    /* correct current_menu_item and select field dest */
    if(select_field_dest.y < menu_box_dest.y + MENU_BOX_FRAME_SIZE)
      select_field_dest.y = menu_box_dest.y + MENU_BOX_FRAME_SIZE;
    else if(select_field_dest.y > menu_box_dest.y + MENU_BOX_FRAME_SIZE + (MENU_ITEM_SPACE - 1) * select_field->h)
      select_field_dest.y = menu_box_dest.y + MENU_BOX_FRAME_SIZE + (MENU_ITEM_SPACE - 1) * select_field->h;
    
    if(current_menu_item < 0)
    {
      select_field_dest.y = menu_box_dest.y + MENU_BOX_FRAME_SIZE + (MENU_ITEM_SPACE - 1) * select_field->h;
      current_menu_item = setting_menu.list_items - 1;
    }
    else if(current_menu_item >= setting_menu.list_items)
    {
      select_field_dest.y = menu_box_dest.y + MENU_BOX_FRAME_SIZE;
      current_menu_item = 0;
    }
    
    /* blit background */
    SDL_BlitSurface(background, &background_src, screen, NULL);
    SDL_BlitSurface(menu_box, NULL, screen, &menu_box_dest);
    
    /* blit menu text */
    cache_text_dest.y = menu_box_dest.y - 4;
    
    cache_menu_item = current_menu_item - (int)((select_field_dest.y - menu_box_dest.y + MENU_BOX_FRAME_SIZE)/select_field->h);
    while(cache_menu_item <= current_menu_item + (int)(((menu_box_dest.y + menu_box->h) - (select_field_dest.y + select_field->h))/select_field->h))
    {
      if(cache_menu_item == 1)
      {
        /* fullscreen toggle menu item */
        cache_text_dest.x = (int)(screen->w/2 - (setting_menu.list[cache_menu_item].text_surface->w + toggle_off->w)/2);
        SDL_BlitSurface(setting_menu.list[cache_menu_item].text_surface, NULL, screen, &cache_text_dest);
        
        cache_text_dest.x += setting_menu.list[cache_menu_item].text_surface->w;
        cache_text_dest.y += (int)(toggle_off->h/1.4);
        
        if(fullscreen_toggle)
          SDL_BlitSurface(toggle_on, NULL, screen, &cache_text_dest);
        else
          SDL_BlitSurface(toggle_off, NULL, screen, &cache_text_dest);
        
        cache_text_dest.y -= (int)(toggle_off->h/1.4);
      }
      else if(cache_menu_item == 2)
      {
        /* music volume */
        cache_text_dest.x = (int)(screen->w/2 - (setting_menu.list[cache_menu_item].text_surface->w + volume_bar->w)/2);
        SDL_BlitSurface(setting_menu.list[cache_menu_item].text_surface, NULL, screen, &cache_text_dest);
        
        cache_text_dest.x += setting_menu.list[cache_menu_item].text_surface->w + 10;
        cache_text_dest.y += (int)(volume_controller->h * 1.6);
        
        SDL_BlitSurface(volume_bar, NULL, screen, &cache_text_dest);
        
        cache_text_dest.x += (int)((volume_bar->w - volume_controller->w) * ((float)music_volume/(float)MIX_MAX_VOLUME));
        cache_text_dest.y -= (int)(volume_controller->h * 0.35);
        SDL_BlitSurface(volume_controller, NULL, screen, &cache_text_dest);
        
        cache_text_dest.y -= (int)(volume_controller->h * 1.25);
      }
      else if(cache_menu_item == 3)
      {
        /* sfx volume */
        cache_text_dest.x = (int)(screen->w/2 - (setting_menu.list[cache_menu_item].text_surface->w + volume_bar->w)/2);
        SDL_BlitSurface(setting_menu.list[cache_menu_item].text_surface, NULL, screen, &cache_text_dest);
        
        cache_text_dest.x += setting_menu.list[cache_menu_item].text_surface->w + 10;
        cache_text_dest.y += (int)(volume_controller->h * 1.6);
        
        SDL_BlitSurface(volume_bar, NULL, screen, &cache_text_dest);
        
        cache_text_dest.x += (int)((volume_bar->w - volume_controller->w) * ((float)sfx_volume/(float)MIX_MAX_VOLUME));
        cache_text_dest.y -= (int)(volume_controller->h * 0.35);
        SDL_BlitSurface(volume_controller, NULL, screen, &cache_text_dest);
        
        cache_text_dest.y -= (int)(volume_controller->h * 1.25);
      }
      else
      {
        /* other menu items */
        cache_text_dest.x = (int)(screen->w/2 - setting_menu.list[cache_menu_item].text_surface->w/2);
        
        SDL_BlitSurface(setting_menu.list[cache_menu_item].text_surface, NULL, screen, &cache_text_dest);
      }
      
      cache_text_dest.y += select_field->h;
      cache_menu_item++;
    }
    
    /* blit select field */
    SDL_BlitSurface(select_field, NULL, screen, &select_field_dest);
    SDL_Flip(screen);
    
    controll_framerate();
  }
  
  SDL_FreeSurface(setting_menu.list[0].text_surface);
  return return_value;
}
int call_game_menu(SDL_Surface *screen)
{
  int return_value = GAME_MENU;
  
  SDL_Rect cache_text_dest;
  int current_menu_item = 0;
  int cache_menu_item;
  
  SDL_Rect menu_box_dest = {(int)(screen->w/2 - menu_box->w/2), (int)(screen->h/2 - menu_box->h/2), 0, 0};
  SDL_Rect select_field_dest = {(int)(screen->w/2 - menu_box->w/2) + MENU_BOX_FRAME_SIZE, menu_box_dest.y + MENU_BOX_FRAME_SIZE, 0, 0};
  
  boxRGBA(screen, 0, 0, screen->w - 1, screen->h - 1, 0, 0, 0, 127);
  SDL_Surface *screen_shot = SDL_ConvertSurface(screen, screen->format, screen->flags);
  
  /* pause previous sounds and reset all buttons */
  reset_all_buttons();
  Mix_Pause(-1);
  
  /* game menu loop */
  Mix_PlayChannel(-1, abort_sound, 0);
  while(return_value == GAME_MENU)
  {
    update_button_state();
    
    /* handle user_input */
    if(get_all_button_state(BUTTON_DOWN))
    {
      set_all_button_state(BUTTON_DOWN, 0);
      Mix_PlayChannel(-1, toggle_sound, 0);
      
      select_field_dest.y += select_field->h;
      current_menu_item++;
    }
    else if(get_all_button_state(BUTTON_UP))
    {
      set_all_button_state(BUTTON_UP, 0);
      Mix_PlayChannel(-1, toggle_sound, 0);
      
      select_field_dest.y -= select_field->h;
      current_menu_item--;
    }
    else if(get_all_button_state(BUTTON_OK))
    {
      set_all_button_state(BUTTON_OK, 0);
      Mix_PlayChannel(-1, select_sound, 0);
      
      switch(current_menu_item)
      {
        case 0:
          return_value = GAME_RUNNING;
          break;
        case 1:
          return_value = RESTART_BATTLE_MENU;
          break;
        case 2:
          return_value = SELECT_CHARACTER_MENU;
          break;
        case 3:
          return_value = MAIN_MENU;
          break;
        case 4:
          return_value = EXIT_MENU;
          break;
      }
    }
    else if((get_all_button_state(BUTTON_ABORT)) || (get_all_button_state(BUTTON_PAUSE)))
    {
      set_all_button_state(BUTTON_ABORT, 0);
      set_all_button_state(BUTTON_PAUSE, 0);
      Mix_PlayChannel(-1, abort_sound, 0);
      
      return_value = GAME_RUNNING;
    }
    
    /* correct current_menu_item and select field dest */
    if(select_field_dest.y < menu_box_dest.y + MENU_BOX_FRAME_SIZE)
      select_field_dest.y = menu_box_dest.y + MENU_BOX_FRAME_SIZE;
    else if(select_field_dest.y > menu_box_dest.y + MENU_BOX_FRAME_SIZE + (MENU_ITEM_SPACE - 1) * select_field->h)
      select_field_dest.y = menu_box_dest.y + MENU_BOX_FRAME_SIZE + (MENU_ITEM_SPACE - 1) * select_field->h;
    
    if(current_menu_item < 0)
    {
      select_field_dest.y = menu_box_dest.y + MENU_BOX_FRAME_SIZE + (MENU_ITEM_SPACE - 1) * select_field->h;
      current_menu_item = game_menu.list_items - 1;
    }
    else if(current_menu_item >= game_menu.list_items)
    {
      select_field_dest.y = menu_box_dest.y + MENU_BOX_FRAME_SIZE;
      current_menu_item = 0;
    }
    
    /* blit screen_shot */
    SDL_BlitSurface(screen_shot, NULL, screen, NULL);
    SDL_BlitSurface(menu_box, NULL, screen, &menu_box_dest);
    
    /* blit menu text */
    cache_text_dest.y = menu_box_dest.y - 4;
    
    cache_menu_item = current_menu_item - (int)((select_field_dest.y - menu_box_dest.y + MENU_BOX_FRAME_SIZE)/select_field->h);
    while(cache_menu_item <= current_menu_item + (int)(((menu_box_dest.y + menu_box->h) - (select_field_dest.y + select_field->h))/select_field->h))
    {
      cache_text_dest.x = (int)(screen->w/2 - game_menu.list[cache_menu_item].text_surface->w/2);
      
      SDL_BlitSurface(game_menu.list[cache_menu_item].text_surface, NULL, screen, &cache_text_dest);
      cache_text_dest.y += select_field->h;
      
      cache_menu_item++;
    }
    
    /* blit select field */
    SDL_BlitSurface(select_field, NULL, screen, &select_field_dest);
    SDL_Flip(screen);
    
    controll_framerate();
  }
  
  /* resume previous sounds and reset buttons */
  Mix_Resume(-1);
  reset_all_buttons();
  
  SDL_FreeSurface(screen_shot);
  return return_value;
}
char *call_map_select(SDL_Surface *screen)
{
  SDL_Rect cache_text_dest;
  int cache_menu_item;
  
  SDL_Rect background_src = {(int)(background->w/2 - screen->w/2), (int)(background->h/2 - screen->h/2), screen->w, screen->h};
  SDL_Rect menu_box_dest = {(int)(screen->w/2 - menu_box->w/2), (int)(screen->h - menu_box->h * 1.2), 0, 0};
  SDL_Rect select_field_dest = {(int)(screen->w/2 - menu_box->w/2) + MENU_BOX_FRAME_SIZE, menu_box_dest.y + MENU_BOX_FRAME_SIZE, 0, 0};
  
  /* static variables to keep select_field at place */
  static int current_menu_item = 0;
  
  cache_menu_item = 0;
  while(cache_menu_item < current_menu_item)
  {
    select_field_dest.y += select_field->h;
    
    cache_menu_item++;
  }
  
  /* main menu loop */
  reset_all_buttons();
  while(1)
  {
    update_button_state();
    
    /* handle user_input */
    if(get_all_button_state(BUTTON_DOWN))
    {
      set_all_button_state(BUTTON_DOWN, 0);
      Mix_PlayChannel(-1, toggle_sound, 0);
      
      select_field_dest.y += select_field->h;
      current_menu_item++;
    }
    else if(get_all_button_state(BUTTON_UP))
    {
      set_all_button_state(BUTTON_UP, 0);
      Mix_PlayChannel(-1, toggle_sound, 0);
      
      select_field_dest.y -= select_field->h;
      current_menu_item--;
    }
    else if(get_all_button_state(BUTTON_OK))
    {
      set_all_button_state(BUTTON_OK, 0);
      Mix_PlayChannel(-1, select_sound, 0);
      
      if(current_menu_item == map_menu.list_items - 1)
        return NULL;
      else
        return map_menu.list[current_menu_item].path;
    }
    else if(get_all_button_state(BUTTON_ABORT))
    {
      set_all_button_state(BUTTON_ABORT, 0);
      Mix_PlayChannel(-1, abort_sound, 0);
      
      return NULL;
    }
    
    /* correct current_menu_item and select field dest */
    if(select_field_dest.y < menu_box_dest.y + MENU_BOX_FRAME_SIZE)
      select_field_dest.y = menu_box_dest.y + MENU_BOX_FRAME_SIZE;
    else if(select_field_dest.y > menu_box_dest.y + MENU_BOX_FRAME_SIZE + (MENU_ITEM_SPACE - 1) * select_field->h)
      select_field_dest.y = menu_box_dest.y + MENU_BOX_FRAME_SIZE + (MENU_ITEM_SPACE - 1) * select_field->h;
    
    if(current_menu_item < 0)
    {
      if(map_menu.list_items > MENU_ITEM_SPACE)
        select_field_dest.y = menu_box_dest.y + MENU_BOX_FRAME_SIZE + (MENU_ITEM_SPACE - 1) * select_field->h;
      else
        select_field_dest.y = menu_box_dest.y + MENU_BOX_FRAME_SIZE + (map_menu.list_items - 1) * select_field->h;
      
      current_menu_item = map_menu.list_items - 1;
    }
    else if(current_menu_item >= map_menu.list_items)
    {
      select_field_dest.y = menu_box_dest.y + MENU_BOX_FRAME_SIZE;
      current_menu_item = 0;
    }
    
    /* blit background */
    SDL_BlitSurface(background, &background_src, screen, NULL);
    SDL_BlitSurface(menu_box, NULL, screen, &menu_box_dest);
    
    /* blit menu text */
    cache_text_dest.y = menu_box_dest.y - 4;
    
    cache_menu_item = current_menu_item - (int)((select_field_dest.y - menu_box_dest.y + MENU_BOX_FRAME_SIZE)/select_field->h);
    while(cache_menu_item < map_menu.list_items && cache_menu_item <= current_menu_item + (int)(((menu_box_dest.y + menu_box->h) - (select_field_dest.y + select_field->h))/select_field->h))
    {
      cache_text_dest.x = (int)(screen->w/2 - map_menu.list[cache_menu_item].text_surface->w/2);
      SDL_BlitSurface(map_menu.list[cache_menu_item].text_surface, NULL, screen, &cache_text_dest);
      
      cache_text_dest.y += select_field->h;
      
      cache_menu_item++;
    }
    /* blit select field */
    SDL_BlitSurface(select_field, NULL, screen, &select_field_dest);
    SDL_Flip(screen);
    
    controll_framerate();
  }
}
char *call_character_select(SDL_Surface *screen, const int player, int *team_list)
{
  SDL_Rect cache_text_dest;
  int current_menu_item = 0;
  int cache_menu_item;
  
  char cache_string[1024];
  SDL_Surface *team_text = NULL;
  
  SDL_Rect background_src = {(int)(background->w/2 - screen->w/2), (int)(background->h/2 - screen->h/2), screen->w, screen->h};
  SDL_Rect menu_box_dest = {(int)(screen->w/2 - menu_box->w/2), (int)(screen->h - menu_box->h * 1.2), 0, 0};
  SDL_Rect team_box_dest = {menu_box_dest.x, menu_box_dest.y - team_box->h, 0, 0};
  SDL_Rect select_field_dest = {(int)(screen->w/2 - menu_box->w/2) + MENU_BOX_FRAME_SIZE, menu_box_dest.y + MENU_BOX_FRAME_SIZE, 0, 0};
  
  /* main menu loop */
  while(1)
  {
    update_button_state();
    
    /* handle user_input */
    if(get_button_state(player, BUTTON_DOWN))
    {
      set_button_state(player, BUTTON_DOWN, 0);
      Mix_PlayChannel(-1, toggle_sound, 0);
      
      select_field_dest.y += select_field->h;
      current_menu_item++;
    }
    else if(get_button_state(player, BUTTON_UP))
    {
      set_button_state(player, BUTTON_UP, 0);
      Mix_PlayChannel(-1, toggle_sound, 0);
      
      select_field_dest.y -= select_field->h;
      current_menu_item--;
    }
    else if(get_button_state(player, BUTTON_LEFT))
    {
      set_button_state(player, BUTTON_LEFT, 0);
      Mix_PlayChannel(-1, toggle_sound, 0);
      
      team_list[player]--;
    }
    else if(get_button_state(player, BUTTON_RIGHT))
    {
      set_button_state(player, BUTTON_RIGHT, 0);
      Mix_PlayChannel(-1, toggle_sound, 0);
      
      team_list[player]++;
    }
    else if(get_button_state(player, BUTTON_OK))
    {
      set_button_state(player, BUTTON_OK, 0);
      Mix_PlayChannel(-1, select_sound, 0);
      
      if(current_menu_item == character_menu.list_items - 1)
        return NULL;
      else
        return character_menu.list[current_menu_item].path;
    }
    else if(get_all_button_state(BUTTON_ABORT))
    {
      set_all_button_state(BUTTON_ABORT, 0);
      Mix_PlayChannel(-1, abort_sound, 0);
      
      return NULL;
    }
    
    /* correct current_menu_item and select field dest */
    if(select_field_dest.y < menu_box_dest.y + MENU_BOX_FRAME_SIZE)
      select_field_dest.y = menu_box_dest.y + MENU_BOX_FRAME_SIZE;
    else if(select_field_dest.y > menu_box_dest.y + MENU_BOX_FRAME_SIZE + (MENU_ITEM_SPACE - 1) * select_field->h)
      select_field_dest.y = menu_box_dest.y + MENU_BOX_FRAME_SIZE + (MENU_ITEM_SPACE - 1) * select_field->h;
    
    if(current_menu_item < 0)
    {
      if(character_menu.list_items > MENU_ITEM_SPACE)
        select_field_dest.y = menu_box_dest.y + MENU_BOX_FRAME_SIZE + (MENU_ITEM_SPACE - 1) * select_field->h;
      else
        select_field_dest.y = menu_box_dest.y + MENU_BOX_FRAME_SIZE + (character_menu.list_items - 1) * select_field->h;
      
      current_menu_item = character_menu.list_items - 1;
    }
    else if(current_menu_item >= character_menu.list_items)
    {
      select_field_dest.y = menu_box_dest.y + MENU_BOX_FRAME_SIZE;
      current_menu_item = 0;
    }
    
    /* blit background */
    SDL_BlitSurface(background, &background_src, screen, NULL);
    SDL_BlitSurface(menu_box, NULL, screen, &menu_box_dest);
    
    /* blit team_box */
    SDL_BlitSurface(team_box, NULL, screen, &team_box_dest);
    
    /* correct team number */
    if(team_list[player] < 1)
      team_list[player] = PLAYER_MAX;
    else if(team_list[player] > PLAYER_MAX)
      team_list[player] = 1;
    
    sprintf(cache_string, "Team: %i", team_list[player]);
    team_text = TTF_RenderText_Blended(menu_font, cache_string, menu_color);
    
    cache_text_dest = team_box_dest;
    cache_text_dest.x = team_box_dest.x + (int)(team_box->w/2 - team_text->w/2);
    cache_text_dest.y -= 5;
    SDL_BlitSurface(team_text, NULL, screen, &cache_text_dest);
    
    /* blit menu text */
    cache_text_dest.y = menu_box_dest.y - 4;
    
    cache_menu_item = current_menu_item - (int)((select_field_dest.y - menu_box_dest.y + MENU_BOX_FRAME_SIZE)/select_field->h);
    while(cache_menu_item < character_menu.list_items && cache_menu_item <= current_menu_item + (int)(((menu_box_dest.y + menu_box->h) - (select_field_dest.y + select_field->h))/select_field->h))
    {
      cache_text_dest.x = (int)(screen->w/2 - character_menu.list[cache_menu_item].text_surface->w/2);
      SDL_BlitSurface(character_menu.list[cache_menu_item].text_surface, NULL, screen, &cache_text_dest);
      
      cache_text_dest.y += select_field->h;
      
      cache_menu_item++;
    }
    /* blit select field */
    SDL_BlitSurface(select_field, NULL, screen, &select_field_dest);
    SDL_Flip(screen);
    
    controll_framerate();
  }
}