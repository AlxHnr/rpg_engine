#include <game.h>

int start_game(const char *player_path[PLAYER_MAX], int *team_list, SDL_Surface *screen)
{
  int return_value = GAME_RUNNING;
  int current_player;
  int cache_current_player;
  int cache_counter;
  new_skill *cache_skill_list = NULL;
  
  new_game_unit *player[PLAYER_MAX];
  new_camera player_camera[PLAYER_MAX];
  
  Uint32 game_menu_time_stop;
  
  /* load game_units */
  if((player[0] = load_game_unit(player_path[0])) == NULL)
    return MAIN_MENU;
  if((player[1] = load_game_unit(player_path[1])) == NULL)
    return MAIN_MENU;
  
  if(player_path[2] != NULL && (player[2] = load_game_unit(player_path[2])) == NULL)
    return MAIN_MENU;
  else if(player_path[2] == NULL)
    player[2] = NULL;
  
  if(player_path[3] != NULL && (player[3] = load_game_unit(player_path[3])) == NULL)
    return MAIN_MENU;
  else if(player_path[3] == NULL)
    player[3] = NULL;
  
  /* set player_camera, init game_units and spawn */
  if(player[3] != NULL)
  {
    /* 4 player settings */
    player_camera[0].x_movement = 0;
    player_camera[0].y_movement = 0;
    player_camera[0].w = (int)(screen->w/2);
    player_camera[0].h = (int)(screen->h/2);
    set_spawn(player[0], rand() % SPAWN_POINTS + 1);
    set_camera(&player_camera[0], player[0]);
    
    player_camera[1].x_movement = (int)(screen->w/2);
    player_camera[1].y_movement = 0;
    player_camera[1].w = (int)(screen->w/2);
    player_camera[1].h = (int)(screen->h/2);
    set_spawn(player[1], rand() % SPAWN_POINTS + 1);
    set_camera(&player_camera[1], player[1]);
    
    player_camera[2].x_movement = 0;
    player_camera[2].y_movement = (int)(screen->h/2);
    player_camera[2].w = (int)(screen->w/2);
    player_camera[2].h = (int)(screen->h/2);
    set_spawn(player[2], rand() % SPAWN_POINTS + 1);
    set_camera(&player_camera[2], player[2]);
    
    player_camera[3].x_movement = (int)(screen->w/2);
    player_camera[3].y_movement = (int)(screen->h/2);
    player_camera[3].w = (int)(screen->w/2);
    player_camera[3].h = (int)(screen->h/2);
    set_spawn(player[3], rand() % SPAWN_POINTS + 1);
    set_camera(&player_camera[3], player[3]);
  }
  else if(player[2] != NULL)
  {
    /* 3 player settings */
    player_camera[0].x_movement = 0;
    player_camera[0].y_movement = 0;
    player_camera[0].w = (int)(screen->w/2);
    player_camera[0].h = (int)(screen->h/2);
    set_spawn(player[0], rand() % SPAWN_POINTS + 1);
    set_camera(&player_camera[0], player[0]);
    
    player_camera[1].x_movement = (int)(screen->w/2);
    player_camera[1].y_movement = 0;
    player_camera[1].w = (int)(screen->w/2);
    player_camera[1].h = (int)(screen->h/2);
    set_spawn(player[1], rand() % SPAWN_POINTS + 1);
    set_camera(&player_camera[1], player[1]);
    
    player_camera[2].x_movement = (int)(screen->w/4);
    player_camera[2].y_movement = (int)(screen->h/2);
    player_camera[2].w = (int)(screen->w/2);
    player_camera[2].h = (int)(screen->h/2);
    set_spawn(player[2], rand() % SPAWN_POINTS + 1);
    set_camera(&player_camera[2], player[2]);
  }
  else if(player[1] != NULL)
  {
    /* 2 player settings */
    player_camera[0].x_movement = 0;
    player_camera[0].y_movement = 0;
    player_camera[0].w = (int)(screen->w/2);
    player_camera[0].h = screen->h;
    set_spawn(player[0], rand() % SPAWN_POINTS + 1);
    set_camera(&player_camera[0], player[0]);
    
    player_camera[1].x_movement = (int)(screen->w/2);
    player_camera[1].y_movement = 0;
    player_camera[1].w = (int)(screen->w/2);
    player_camera[1].h = screen->h;
    set_spawn(player[1], rand() % SPAWN_POINTS + 1);
    set_camera(&player_camera[1], player[1]);
  }
  else
    return MAIN_MENU;
  
  while(return_value == GAME_RUNNING)
  {
    update_button_state();
    
    /* handle special buttons */
    if(get_all_button_state(BUTTON_PAUSE))
    {
      game_menu_time_stop = SDL_GetTicks();
      return_value = call_game_menu(screen);
      
      /* correct game menu time */
      current_player = 0;
      while(current_player < PLAYER_MAX && player[current_player] != NULL)
      {
        extend_game_unit_time(player[current_player], SDL_GetTicks() - game_menu_time_stop);
        
        current_player++;
      }
      extend_damage_time(SDL_GetTicks() - game_menu_time_stop);
    }
    
    /* handle each player */
    current_player = 0;
    while(current_player < PLAYER_MAX && player[current_player] != NULL)
    {
      /* handle user input */
      /* handle directions */
      player[current_player]->direction = dir_none;
      
      if(get_button_state(current_player, BUTTON_LEFT))
        player[current_player]->direction = dir_left;
      else if(get_button_state(current_player, BUTTON_RIGHT))
        player[current_player]->direction = dir_right;
      
      if(get_button_state(current_player, BUTTON_UP))
        player[current_player]->direction += dir_up;
      else if(get_button_state(current_player, BUTTON_DOWN))
        player[current_player]->direction += dir_down;
      
      /* handle skills */
      cache_counter = 0;
      cache_skill_list = player[current_player]->skill_list;
      while(cache_counter < 8 && cache_skill_list != NULL)
      {
        if(get_button_state(current_player, BUTTON_1 + cache_counter))
          use_skill(player[current_player], cache_skill_list);
        
        cache_skill_list = cache_skill_list->next;
        cache_counter++;
      }
      
      /* handle player */
      update_game_unit_stats(player[current_player]);
      
      handle_collision(player[current_player], &player_camera[current_player]);
      move_game_unit(player[current_player]);
      set_camera(&player_camera[current_player], player[current_player]);
      
      animate_game_unit(player[current_player]);
      handle_game_unit(player[current_player]);
      
      /* handle damage on other players */
      cache_current_player = 0;
      while(cache_current_player < PLAYER_MAX && player[cache_current_player] != NULL)
      {
        if(cache_current_player != current_player && team_list[cache_current_player] != team_list[current_player])
          handle_game_unit_damage(player[current_player], player[cache_current_player]);
        
        cache_current_player++;
      }
      
      /* handle respawn */
      if(player[current_player]->hp <= 0 && SDL_GetTicks() - player[current_player]->time_stop > RESPAWN_DELAY)
      {
        player[current_player]->hp = player[current_player]->stats.max_hp;
        player[current_player]->mp = player[current_player]->stats.max_mp;
        set_spawn(player[current_player], rand() % SPAWN_POINTS + 1);
        cancel_all_enchantments(player[current_player]);
      }
      
      current_player++;
    }
    
    /* clear screen */
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    
    /* blit lower layer */
    current_player = 0;
    while(current_player < PLAYER_MAX && player[current_player] != NULL)
    {
      blit_layer(0, &player_camera[current_player], screen);
      blit_layer(1, &player_camera[current_player], screen);
      blit_layer(2, &player_camera[current_player], screen);
      
      current_player++;
    }
    
    /* blit character layer */
    current_player = 0;
    while(current_player < PLAYER_MAX && player[current_player] != NULL)
    {
      cache_current_player = 0;
      while(cache_current_player < PLAYER_MAX && player[cache_current_player] != NULL)
      {
        blit_game_unit(&player_camera[cache_current_player], player[current_player], screen);
        
        cache_current_player++;
      }
      
      current_player++;
    }
    
    /* blit upper layer, damage and interface */
    current_player = 0;
    while(current_player < PLAYER_MAX && player[current_player] != NULL)
    {
      blit_layer(3, &player_camera[current_player], screen);
      blit_layer(4, &player_camera[current_player], screen);
      blit_layer(5, &player_camera[current_player], screen);
      
      blit_damage(&player_camera[current_player], screen);
      blit_user_interface(player[current_player], &player_camera[current_player], screen);
      
      current_player++;
    }
    
    /* flip screen */
    if(return_value == GAME_RUNNING)
      SDL_Flip(screen);
    controll_framerate();
  }
  
  current_player = 0;
  while(current_player < PLAYER_MAX)
  {
    free_game_unit(player[current_player]);
    
    current_player++;
  }
  
  return return_value;
}