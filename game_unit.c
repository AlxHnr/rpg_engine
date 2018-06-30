#include <game_unit.h>

void set_spawn(new_game_unit *game_unit, const int spawn_num)
{
  int counter_x = 0;
  int counter_y = 0;
  
  while(counter_y < max_tile_y)
  {
    counter_x = 0;
    while(counter_x < max_tile_x)
    {
      if(get_tile(6, counter_x, counter_y, 0) == 2 + ' ' && get_tile(6, counter_x, counter_y, 1) == spawn_num + ' ')
      {
        game_unit->x = counter_x * tile_w;
        game_unit->y = counter_y * tile_h;
      }
      
      counter_x++;
    }
    
    counter_y++;
  }
}

void set_camera(new_camera *camera, const new_game_unit *game_unit)
{
  /* handle horizontal position */
  if(game_unit->x < camera->w/2)
    camera->x = 0;
  else if(game_unit->x > max_tile_x * tile_w - camera->w/2)
    camera->x = max_tile_x * tile_w - camera->w;
  else
    camera->x = game_unit->x - camera->w/2;
  
  /* handle vertical position */
  if(game_unit->y < camera->h/2)
    camera->y = 0;
  else if(game_unit->y > max_tile_y * tile_h - camera->h/2)
    camera->y = max_tile_y * tile_h - camera->h;
  else
    camera->y = game_unit->y - camera->h/2;
}
void blit_game_unit(const new_camera *camera, const new_game_unit *game_unit, SDL_Surface *screen)
{
  SDL_Rect cache_src = game_unit->sprite_src;
  SDL_Rect cache_dst;
  
  new_enchantment *cache_enchantment = game_unit->enchantment_list;
  new_skill_object *cache_skill_object = game_unit->skill_object_list;
  
  /* check if game_unit is in camera range */
  if(game_unit->x + game_unit->sprite_src.w/2 > camera->x &&
      game_unit->x - game_unit->sprite_src.w/2 < camera->x + camera->w &&
      game_unit->y + game_unit->sprite_src.h/2 > camera->y &&
      game_unit->y - game_unit->sprite_src.h/2 < camera->y + camera->h)
  {
    cache_dst.x = camera->x_movement + (game_unit->x - game_unit->sprite_src.w/2 - camera->x);
    cache_dst.y = camera->y_movement + (game_unit->y - game_unit->sprite_src.h/2 - camera->y);
    
    /* horizontal */
    if(cache_dst.x + game_unit->sprite_src.w >= camera->x_movement &&
        cache_dst.x < camera->x_movement)
    {
      cache_src.x += camera->x_movement - cache_dst.x;
      cache_src.w -= camera->x_movement - cache_dst.x;
      
      cache_dst.x = camera->x_movement;
    }
    else if(cache_dst.x + game_unit->sprite_src.w > camera->x_movement + camera->w &&
        cache_dst.x < camera->x_movement + camera->w)
    {
      cache_src.w -= (cache_dst.x + game_unit->sprite_src.w) - (camera->x_movement + camera->w);
      
      cache_dst.x = camera->x_movement + camera->w - cache_src.w;
    }
    
    /* vertical */
    if(cache_dst.y + game_unit->sprite_src.h >= camera->y_movement &&
        cache_dst.y < camera->y_movement)
    {
      cache_src.y += camera->y_movement - cache_dst.y;
      cache_src.h -= camera->y_movement - cache_dst.y;
      
      cache_dst.y = camera->y_movement;
    }
    else if(cache_dst.y + game_unit->sprite_src.h > camera->y_movement + camera->h &&
        cache_dst.y < camera->y_movement + camera->h)
    {
      cache_src.h -= (cache_dst.y + game_unit->sprite_src.h) - (camera->y_movement + camera->h);
      
      cache_dst.y = camera->y_movement + camera->h - cache_src.h;
    }
    
    SDL_BlitSurface(game_unit->sprite_sheet, &cache_src, screen, &cache_dst);
  }
  
  /* blit enchantment animations */
  while(cache_enchantment != NULL)
  {
    /* check if enchantment animation frame is in camera range and enchantment animation life time is valid */
    if(cache_enchantment->sprite_sheet != NULL &&
      game_unit->x + cache_enchantment->sprite_src.w/2 > camera->x &&
      game_unit->x - cache_enchantment->sprite_src.w/2 < camera->x + camera->w &&
      game_unit->y + cache_enchantment->sprite_src.h/2 > camera->y &&
      game_unit->y - cache_enchantment->sprite_src.h/2 < camera->y + camera->h &&
      SDL_GetTicks() - cache_enchantment->animation_start_time < cache_enchantment->animation_duration)
    {
      cache_src = cache_enchantment->sprite_src;
      
      cache_dst.x = camera->x_movement + (game_unit->x - cache_enchantment->sprite_src.w/2 - camera->x);
      cache_dst.y = camera->y_movement + (game_unit->y - cache_enchantment->sprite_src.h/2 - camera->y);
      
      /* horizontal */
      if(cache_dst.x + cache_enchantment->sprite_src.w >= camera->x_movement &&
          cache_dst.x < camera->x_movement)
      {
        cache_src.x += camera->x_movement - cache_dst.x;
        cache_src.w -= camera->x_movement - cache_dst.x;
        
        cache_dst.x = camera->x_movement;
      }
      else if(cache_dst.x + cache_enchantment->sprite_src.w > camera->x_movement + camera->w &&
          cache_dst.x < camera->x_movement + camera->w)
      {
        cache_src.w -= (cache_dst.x + cache_enchantment->sprite_src.w) - (camera->x_movement + camera->w);
        
        cache_dst.x = camera->x_movement + camera->w - cache_src.w;
      }
      
      /* vertical */
      if(cache_dst.y + cache_enchantment->sprite_src.h >= camera->y_movement &&
          cache_dst.y < camera->y_movement)
      {
        cache_src.y += camera->y_movement - cache_dst.y;
        cache_src.h -= camera->y_movement - cache_dst.y;
        
        cache_dst.y = camera->y_movement;
      }
      else if(cache_dst.y + cache_enchantment->sprite_src.h > camera->y_movement + camera->h &&
          cache_dst.y < camera->y_movement + camera->h)
      {
        cache_src.h -= (cache_dst.y + cache_enchantment->sprite_src.h) - (camera->y_movement + camera->h);
        
        cache_dst.y = camera->y_movement + camera->h - cache_src.h;
      }
      
      SDL_BlitSurface(cache_enchantment->sprite_sheet, &cache_src, screen, &cache_dst);
    }
    
    cache_enchantment = cache_enchantment->next;
  }
  
  /* blit cast animation */
  if(game_unit->current_skill != NULL &&
    game_unit->current_skill->cast_sprite_sheet != NULL &&
    game_unit->x + game_unit->current_skill->cast_sprite_src.w/2 > camera->x &&
    game_unit->x - game_unit->current_skill->cast_sprite_src.w/2 < camera->x + camera->w &&
    game_unit->y + game_unit->current_skill->cast_sprite_src.h/2 > camera->y &&
    game_unit->y - game_unit->current_skill->cast_sprite_src.h/2 < camera->y + camera->h)
  {
    cache_src = game_unit->current_skill->cast_sprite_src;
    
    cache_dst.x = camera->x_movement + (game_unit->x - game_unit->current_skill->cast_sprite_src.w/2 - camera->x);
    cache_dst.y = camera->y_movement + (game_unit->y - game_unit->current_skill->cast_sprite_src.h/2 - camera->y);
    
    /* horizontal */
    if(cache_dst.x + game_unit->current_skill->cast_sprite_src.w >= camera->x_movement &&
        cache_dst.x < camera->x_movement)
    {
      cache_src.x += camera->x_movement - cache_dst.x;
      cache_src.w -= camera->x_movement - cache_dst.x;
      
      cache_dst.x = camera->x_movement;
    }
    else if(cache_dst.x + game_unit->current_skill->cast_sprite_src.w > camera->x_movement + camera->w &&
        cache_dst.x < camera->x_movement + camera->w)
    {
      cache_src.w -= (cache_dst.x + game_unit->current_skill->cast_sprite_src.w) - (camera->x_movement + camera->w);
      
      cache_dst.x = camera->x_movement + camera->w - cache_src.w;
    }
    
    /* vertical */
    if(cache_dst.y + game_unit->current_skill->cast_sprite_src.h >= camera->y_movement &&
        cache_dst.y < camera->y_movement)
    {
      cache_src.y += camera->y_movement - cache_dst.y;
      cache_src.h -= camera->y_movement - cache_dst.y;
      
      cache_dst.y = camera->y_movement;
    }
    else if(cache_dst.y + game_unit->current_skill->cast_sprite_src.h > camera->y_movement + camera->h &&
        cache_dst.y < camera->y_movement + camera->h)
    {
      cache_src.h -= (cache_dst.y + game_unit->current_skill->cast_sprite_src.h) - (camera->y_movement + camera->h);
      
      cache_dst.y = camera->y_movement + camera->h - cache_src.h;
    }
    
    SDL_BlitSurface(game_unit->current_skill->cast_sprite_sheet, &cache_src, screen, &cache_dst);
  }
  
  /* blit cache_skill_object */
  while(cache_skill_object != NULL)
  {
    if(cache_skill_object->sprite_sheet != NULL &&
      cache_skill_object->x + cache_skill_object->sprite_src.w/2 > camera->x &&
      cache_skill_object->x - cache_skill_object->sprite_src.w/2 < camera->x + camera->w &&
      cache_skill_object->y + cache_skill_object->sprite_src.h/2 > camera->y &&
      cache_skill_object->y - cache_skill_object->sprite_src.h/2 < camera->y + camera->h)
    {
      cache_src = cache_skill_object->sprite_src;
      
      cache_dst.x = camera->x_movement + (cache_skill_object->x - cache_skill_object->sprite_src.w/2 - camera->x);
      cache_dst.y = camera->y_movement + (cache_skill_object->y - cache_skill_object->sprite_src.h/2 - camera->y);
      
      /* horizontal */
      if(cache_dst.x + cache_skill_object->sprite_src.w >= camera->x_movement &&
          cache_dst.x < camera->x_movement)
      {
        cache_src.x += camera->x_movement - cache_dst.x;
        cache_src.w -= camera->x_movement - cache_dst.x;
        
        cache_dst.x = camera->x_movement;
      }
      else if(cache_dst.x + cache_skill_object->sprite_src.w > camera->x_movement + camera->w &&
          cache_dst.x < camera->x_movement + camera->w)
      {
        cache_src.w -= (cache_dst.x + cache_skill_object->sprite_src.w) - (camera->x_movement + camera->w);
        
        cache_dst.x = camera->x_movement + camera->w - cache_src.w;
      }
      
      /* vertical */
      if(cache_dst.y + cache_skill_object->sprite_src.h >= camera->y_movement &&
          cache_dst.y < camera->y_movement)
      {
        cache_src.y += camera->y_movement - cache_dst.y;
        cache_src.h -= camera->y_movement - cache_dst.y;
        
        cache_dst.y = camera->y_movement;
      }
      else if(cache_dst.y + cache_skill_object->sprite_src.h > camera->y_movement + camera->h &&
          cache_dst.y < camera->y_movement + camera->h)
      {
        cache_src.h -= (cache_dst.y + cache_skill_object->sprite_src.h) - (camera->y_movement + camera->h);
        
        cache_dst.y = camera->y_movement + camera->h - cache_src.h;
      }
      
      SDL_BlitSurface(cache_skill_object->sprite_sheet, &cache_src, screen, &cache_dst);
    }
    
    cache_skill_object = cache_skill_object->next;
  }
}
char split_dir_x(const char direction)
{
  switch(direction)
  {
    case dir_left:
      return dir_left;
    case dir_right:
      return dir_right;
    case dir_down_left:
      return dir_left;
    case dir_down_right:
      return dir_right;
    case dir_up_left:
      return dir_left;
    case dir_up_right:
      return dir_right;
    default:
      return dir_none;
  }
}
char split_dir_y(const char direction)
{
  switch(direction)
  {
    case dir_down:
      return dir_down;
    case dir_up:
      return dir_up;
    case dir_down_left:
      return dir_down;
    case dir_down_right:
      return dir_down;
    case dir_up_left:
      return dir_up;
    case dir_up_right:
      return dir_up;
     default:
      return dir_none;
  }
}
void handle_collision(new_game_unit *game_unit, const new_camera *camera)
{
  /* new_skill_object variables */
  new_skill_object *cache_skill_object = game_unit->skill_object_list;
  
  new_game_unit cache_game_unit;
  new_camera cache_camera;
  cache_camera.w = camera->w;
  cache_camera.h = camera->w;
  
  /* base variables */
  int unit_direction_x = split_dir_x(game_unit->direction);
  int unit_direction_y = split_dir_y(game_unit->direction);
  
  int direction_x;
  int direction_y;
  
  int counter_x = camera->x/tile_w * tile_w;
  int counter_y = camera->y/tile_h * tile_h;
  
  while(counter_y <= camera->y + camera->h)
  {
    counter_x = camera->x/tile_w * tile_w;
    while(counter_x <= camera->x + camera->w)
    {
      /* check collision */
      if(get_tile(6, counter_x/tile_w, counter_y/tile_h, 0) - ' ' == 1 &&
          game_unit->x + game_unit->collision_w >= counter_x &&
          game_unit->x - game_unit->collision_w <= counter_x + tile_w &&
          game_unit->y + game_unit->collision_h >= counter_y &&
          game_unit->y <= counter_y + tile_h)
      {
        /* handle directions */
        /* get direction from game_unit to tile */
        if(game_unit->x + game_unit->collision_w/2 < counter_x)
          direction_x = dir_right;
        else if(game_unit->x - game_unit->collision_w/2 > counter_x + tile_w)
          direction_x = dir_left;
        else
          direction_x = dir_none;
        
        if(game_unit->y > counter_y + tile_h/2)
          direction_y = dir_up;
        else if(game_unit->y + game_unit->collision_h/2 < counter_y)
          direction_y = dir_down;
        else
          direction_y = dir_none;
        
        /* correct x and position */
        if(direction_y == dir_none && unit_direction_x == direction_x)
          unit_direction_x = dir_none;
        else if(direction_x == dir_none && unit_direction_y == direction_y)
          unit_direction_y = dir_none;
        
        /* handle diagonal collision */
        if(direction_x + direction_y == dir_up_left && get_tile(6, counter_x/tile_w + 1, counter_y/tile_h, 0) != '!' && get_tile(6, counter_x/tile_w, counter_y/tile_h + 1, 0) != '!')
        {
          if(unit_direction_y == dir_up && game_unit->x + game_unit->collision_w/2 < counter_x + tile_w)
          {
            unit_direction_y = dir_none;
          }
          if(unit_direction_x == dir_left && game_unit->y < counter_y + tile_h)
          {
            unit_direction_x = dir_none;
          }
        }
        else if(direction_x + direction_y == dir_down_left && get_tile(6, counter_x/tile_w + 1, counter_y/tile_h, 0) != '!' && get_tile(6, counter_x/tile_w, counter_y/tile_h - 1, 0) != '!')
        {
          if(unit_direction_y == dir_down && game_unit->x - game_unit->collision_w/2 < counter_x + tile_w)
          {
            unit_direction_y = dir_none;
          }
          if(unit_direction_x == dir_left && game_unit->y + game_unit->collision_h > counter_y)
          {
            unit_direction_x = dir_none;
          }
        }
        else if(direction_x + direction_y == dir_up_right && get_tile(6, counter_x/tile_w - 1, counter_y/tile_h, 0) != '!' && get_tile(6, counter_x/tile_w, counter_y/tile_h + 1, 0) != '!')
        {
          if(unit_direction_y == dir_up && game_unit->x + game_unit->collision_w/2 > counter_x)
          {
            unit_direction_y = dir_none;
          }
          if(unit_direction_x == dir_right && game_unit->y < counter_y + tile_h)
          {
            unit_direction_x = dir_none;
          }
        }
        else if(direction_x + direction_y == dir_down_right && get_tile(6, counter_x/tile_w - 1, counter_y/tile_h, 0) != '!' && get_tile(6, counter_x/tile_w, counter_y/tile_h - 1, 0) != '!')
        {
          if(unit_direction_y == dir_down && game_unit->x + game_unit->collision_w/2 > counter_x)
          {
            unit_direction_y = dir_none;
          }
          if(unit_direction_x == dir_right && game_unit->y + game_unit->collision_h > counter_y)
          {
            unit_direction_x = dir_none;
          }
        }
      }
      counter_x += tile_w;
    }
    counter_y += tile_h;
  }
  
  game_unit->direction = unit_direction_x + unit_direction_y;
  
  /* handle skill_object_list */
  while(cache_skill_object != NULL)
  {
    cache_game_unit.x = cache_skill_object->x;
    cache_game_unit.y = cache_skill_object->y;
    set_camera(&cache_camera, &cache_game_unit);
    
    unit_direction_x = split_dir_x(cache_skill_object->direction);
    unit_direction_y = split_dir_y(cache_skill_object->direction);
    
    counter_y = cache_camera.y/tile_h * tile_h;
    while(counter_y <= cache_camera.y + cache_camera.h)
    {
      counter_x = cache_camera.x/tile_w * tile_w;
      while(counter_x <= cache_camera.x + cache_camera.w)
      {
        /* check collision */
        if(cache_skill_object->direction != dir_none &&
            get_tile(6, counter_x/tile_w, counter_y/tile_h, 0) - ' ' == 1 &&
            get_tile(6, counter_x/tile_w, counter_y/tile_h, 1) - ' ' <= 6 &&
            cache_skill_object->x + cache_skill_object->collision_w >= counter_x &&
            cache_skill_object->x - cache_skill_object->collision_w <= counter_x + tile_w &&
            cache_skill_object->y + cache_skill_object->collision_h >= counter_y &&
            cache_skill_object->y - cache_skill_object->collision_h <= counter_y + tile_h)
        {
          /* delete destructable tiles */
          if(get_tile(6, counter_x/tile_w, counter_y/tile_h, 1) - ' ' > 1)
          {
            /* delete tile */
            write_tile(get_tile(6, counter_x/tile_w, counter_y/tile_h, 1) - '!', counter_x/tile_w, counter_y/tile_h, 0, 0);
            
            /* delete collision tile */
            write_tile(6, counter_x/tile_w, counter_y/tile_h, 0, 0);
            
            append_damage(counter_x + tile_w/2, counter_y + tile_h/2, cache_skill_object->m_atk + cache_skill_object->p_atk, damage_normal);
          }
          
          /* set cache_skill_object to hit animation mode */
          cache_skill_object->direction = dir_none;
          cache_skill_object->sprite_sheet = cache_skill_object->hit_sprite_sheet;
          
          if(cache_skill_object->sprite_sheet != NULL)
          {
            cache_skill_object->x = counter_x + tile_w/2;
            cache_skill_object->y = counter_y + tile_h/2;
            
            cache_skill_object->sprite_src.x = 0;
            cache_skill_object->sprite_src.y = 0;
            cache_skill_object->sprite_src.w = cache_skill_object->sprite_sheet->h;
            cache_skill_object->sprite_src.h = cache_skill_object->sprite_sheet->h;
            
            cache_skill_object->time_stop = SDL_GetTicks();
            cache_skill_object->start_time = SDL_GetTicks();
            cache_skill_object->life_time = 100 * (int)(cache_skill_object->sprite_sheet->w/cache_skill_object->sprite_sheet->h);
          }
          
          if(cache_skill_object->hit_sound != NULL)
            Mix_PlayChannel(-1, cache_skill_object->hit_sound, 0);
        }
        counter_x += tile_w;
      }
      counter_y += tile_h;
    }
    
    cache_skill_object = cache_skill_object->next;
  }
}

new_skill *load_skill(const char *name)
{
  /* variables */
  new_skill *cache_skill = NULL;
  
  char cache_path[1024] = {0};
  FILE *reader = NULL;
  
  int cache_num;
  
  /* load skill */
  if((cache_skill = malloc(sizeof(struct new_skill))) == NULL)
    return NULL;
  
  if((cache_skill->self_enchantment = malloc(sizeof(struct new_enchantment))) == NULL)
      return NULL;
  
  if((cache_skill->target_enchantment = malloc(sizeof(struct new_enchantment))) == NULL)
      return NULL;
  
  /* load sound effects */
  sprintf(cache_path, "skills/%s/cast.wav", name);
  cache_skill->cast_sound = Mix_LoadWAV(cache_path);
  
  sprintf(cache_path, "skills/%s/object_start.wav", name);
  cache_skill->object_start_sound = Mix_LoadWAV(cache_path);
  
  sprintf(cache_path, "skills/%s/object_hit.wav", name);
  cache_skill->object_hit_sound = Mix_LoadWAV(cache_path);
  
  sprintf(cache_path, "skills/%s/self_enchantment.wav", name);
  cache_skill->self_enchantment_sound = Mix_LoadWAV(cache_path);
  
  sprintf(cache_path, "skills/%s/target_enchantment.wav", name);
  cache_skill->target_enchantment_sound = Mix_LoadWAV(cache_path);
  
  /* load skill config */
  sprintf(cache_path, "skills/%s/skill.conf", name);
  if((reader = fopen(cache_path, "r")) == NULL)
    return NULL;
  
  sprintf(cache_path, "skills/%s/icon.png", name);
  if((cache_skill->icon = load_image(cache_path)) == NULL)
    return NULL;
  
  sprintf(cache_path, "skills/%s/cast_animation.png", name);
  if((cache_skill->cast_sprite_sheet = load_image(cache_path)) != NULL)
  {
    cache_skill->cast_sprite_src.x = 0;
    cache_skill->cast_sprite_src.y = 0;
    cache_skill->cast_sprite_src.w = cache_skill->cast_sprite_sheet->h;
    cache_skill->cast_sprite_src.h = cache_skill->cast_sprite_sheet->h;
  }
  cache_skill->animation_time_stop = 0;
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  cache_skill->skill_type = atoi(cache_path);
  
  cache_skill->start_time = -100000000;
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  cache_skill->reuse_time = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  cache_skill->hp_cost = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  cache_skill->mp_cost = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  cache_skill->p_atk = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  cache_skill->m_atk = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  cache_skill->base_changse = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  cache_skill->physical_changse = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  cache_skill->magic_changse = atoi(cache_path);
  
  /* read in object_sprite_sheet */
  sprintf(cache_path, "skills/%s/object_sprite_sheet.png", name);
  cache_skill->object_sprite_sheet = load_image(cache_path);
  
  sprintf(cache_path, "skills/%s/object_hit_sprite_sheet.png", name);
  cache_skill->object_hit_sprite_sheet = load_image(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  cache_skill->object_random_direction = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  cache_skill->object_amount = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  cache_skill->collision_w = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  cache_skill->collision_h = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  cache_skill->object_speed = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  cache_skill->object_life_time = atoi(cache_path);
  
  /* read in self_enchantment */
  sprintf(cache_path, "skills/%s/self_enchantment.png", name);
  if((cache_skill->self_enchantment->icon = load_image(cache_path)) != NULL)
  {
    sprintf(cache_path, "skills/%s/self_enchantment_animation.png", name);
    if((cache_skill->self_enchantment->sprite_sheet = load_image(cache_path)) != NULL)
    {
      cache_skill->self_enchantment->sprite_src.x = 0;
      cache_skill->self_enchantment->sprite_src.y = 0;
      cache_skill->self_enchantment->sprite_src.w = cache_skill->self_enchantment->sprite_sheet->h;
      cache_skill->self_enchantment->sprite_src.h = cache_skill->self_enchantment->sprite_sheet->h;
    }
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->self_enchantment->animation_duration = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->self_enchantment->life_time = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->self_enchantment->effect_interval = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->self_enchantment->hp_incrase = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->self_enchantment->mp_incrase = atoi(cache_path);
    
    /* read in stats */
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->self_enchantment->stats.max_hp = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->self_enchantment->stats.hp_reg_rate = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->self_enchantment->stats.max_mp = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->self_enchantment->stats.mp_reg_rate = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->self_enchantment->stats.p_atk = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->self_enchantment->stats.p_def = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->self_enchantment->stats.atk_speed = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->self_enchantment->stats.m_atk = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->self_enchantment->stats.m_def = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->self_enchantment->stats.cast_speed = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->self_enchantment->stats.walk_speed = atoi(cache_path);
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->self_enchantment->stats.physical_changse = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->self_enchantment->stats.physical_resistance = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->self_enchantment->stats.magic_changse = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->self_enchantment->stats.magic_resistance = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->self_enchantment->stats.reflect_changse = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->self_enchantment->stats.block_changse = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->self_enchantment->stats.absorb_hp_rate = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->self_enchantment->stats.absorb_mp_rate = atoi(cache_path);
    
    cache_skill->self_enchantment->next = NULL;
  }
  else
  {
    free(cache_skill->self_enchantment);
    cache_skill->self_enchantment = NULL;
    
    cache_num = 0;
    while(cache_num < 24)
    {
      if(fgets(cache_path, 1024, reader) == NULL)
        return NULL;
      cache_num++;
    }
  }
  
  /* read in target_enchantment */
  sprintf(cache_path, "skills/%s/target_enchantment.png", name);
  if((cache_skill->target_enchantment->icon = load_image(cache_path)) != NULL)
  {
    sprintf(cache_path, "skills/%s/target_enchantment_animation.png", name);
    if((cache_skill->target_enchantment->sprite_sheet = load_image(cache_path)) != NULL)
    {
      cache_skill->target_enchantment->sprite_src.x = 0;
      cache_skill->target_enchantment->sprite_src.y = 0;
      cache_skill->target_enchantment->sprite_src.w = cache_skill->target_enchantment->sprite_sheet->h;
      cache_skill->target_enchantment->sprite_src.h = cache_skill->target_enchantment->sprite_sheet->h;
    }
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->target_enchantment->animation_duration = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->target_enchantment->life_time = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->target_enchantment->effect_interval = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->target_enchantment->hp_incrase = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->target_enchantment->mp_incrase = atoi(cache_path);
    
    /* read in stats */
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->target_enchantment->stats.max_hp = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->target_enchantment->stats.hp_reg_rate = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->target_enchantment->stats.max_mp = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->target_enchantment->stats.mp_reg_rate = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->target_enchantment->stats.p_atk = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->target_enchantment->stats.p_def = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->target_enchantment->stats.atk_speed = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->target_enchantment->stats.m_atk = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->target_enchantment->stats.m_def = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->target_enchantment->stats.cast_speed = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->target_enchantment->stats.walk_speed = atoi(cache_path);
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->target_enchantment->stats.physical_changse = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->target_enchantment->stats.physical_resistance = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->target_enchantment->stats.magic_changse = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->target_enchantment->stats.magic_resistance = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->target_enchantment->stats.reflect_changse = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->target_enchantment->stats.block_changse = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->target_enchantment->stats.absorb_hp_rate = atoi(cache_path);
    
    if(fgets(cache_path, 1024, reader) == NULL)
      return NULL;
    cache_skill->target_enchantment->stats.absorb_mp_rate = atoi(cache_path);
    
    cache_skill->target_enchantment->next = NULL;
  }
  else
  {
    free(cache_skill->target_enchantment);
    cache_skill->target_enchantment = NULL;
  }
  
  cache_skill->next = NULL;
  
  fclose(reader);
  return cache_skill;
}
new_game_unit *load_game_unit(const char *name)
{
  new_game_unit *game_unit = NULL;
  new_skill *cache_skill = NULL;
  
  char cache_path[1024] = {0};
  FILE *reader = NULL;
  
  if((game_unit = malloc(sizeof(struct new_game_unit))) == NULL)
    return NULL;
  
  sprintf(cache_path, "character/%s/character.conf", name);
  if((reader = fopen(cache_path, "r")) == NULL)
    return NULL;
  
  game_unit->x = 0;
  game_unit->y = 0;
  game_unit->last_step = 0;
  
  /* graphic variables */
  sprintf(cache_path, "character/%s/sprite.png", name);
  if((game_unit->sprite_sheet = load_image(cache_path)) == NULL)
    return NULL;
  
  game_unit->sprite_src.x = 0;
  game_unit->sprite_src.y = 0;
  game_unit->sprite_src.w = game_unit->sprite_sheet->h/8;
  game_unit->sprite_src.h = game_unit->sprite_sheet->h/8;
  
  game_unit->time_stop = 0;
  game_unit->sprite_mode = mode_stand;
  
  game_unit->enchantment_list = NULL;
  game_unit->skill_list = NULL;
  
  game_unit->current_skill = NULL;
  game_unit->current_skill_start_time = 0;
  
  game_unit->skill_object_list = NULL;
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->collision_w = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->collision_h = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->frame_atk = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->frame_cast = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->frame_bow = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->frame_die = atoi(cache_path);
  
  /* base stats */
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->base_stats.max_hp = atoi(cache_path);
  game_unit->hp = game_unit->base_stats.max_hp;
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->base_stats.hp_reg_rate = atoi(cache_path);
  game_unit->last_hp_reg = SDL_GetTicks();
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->base_stats.max_mp = atoi(cache_path);
  game_unit->mp = game_unit->base_stats.max_mp;
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->base_stats.mp_reg_rate = atoi(cache_path);
  game_unit->last_mp_reg = SDL_GetTicks();
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->base_stats.p_atk = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->base_stats.p_def = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->base_stats.atk_speed = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->base_stats.m_atk = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->base_stats.m_def = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->base_stats.cast_speed = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->base_stats.walk_speed = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->base_stats.physical_changse = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->base_stats.physical_resistance = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->base_stats.magic_changse = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->base_stats.magic_resistance = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->base_stats.reflect_changse = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->base_stats.block_changse = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->base_stats.absorb_hp_rate = atoi(cache_path);
  
  if(fgets(cache_path, 1024, reader) == NULL)
    return NULL;
  game_unit->base_stats.absorb_mp_rate = atoi(cache_path);
  
  /* load skills */
  game_unit->skill_list = NULL;
  
  /* load other skills */
  while(fgets(cache_path, 1024, reader))
  {
    /* remove '\n' or '\r\n' sign in cache_path */
    if(cache_path[strlen(cache_path) - 2] == '\r')
      cache_path[strlen(cache_path) - 2] = '\0';
    else if(cache_path[strlen(cache_path) - 1] == '\n')
      cache_path[strlen(cache_path) - 1] = '\0';
    
    /* load first skill */
    if(game_unit->skill_list == NULL)
    {
      if((game_unit->skill_list = load_skill(cache_path)) == NULL)
        continue;
      
      cache_skill = game_unit->skill_list;
    }
    else
    {
      if((cache_skill->next = load_skill(cache_path)) == NULL)
        continue;
      
      cache_skill = cache_skill->next;
    }
  }
  
  fclose(reader);
  return game_unit;
}
void free_game_unit(new_game_unit *game_unit)
{
  new_enchantment *cache_enchantment = NULL;
  new_skill *cache_skill = NULL;
  new_skill_object *cache_skill_object = NULL;
  
  if(game_unit == NULL)
    return;
  
  Mix_HaltChannel(-1);
  SDL_FreeSurface(game_unit->sprite_sheet);
  
  /* free enchantments */
  while(game_unit->enchantment_list != NULL)
  {
    cache_enchantment = game_unit->enchantment_list;
    game_unit->enchantment_list = game_unit->enchantment_list->next;
    
    free(cache_enchantment);
  }
  
  /* free skills */
  while(game_unit->skill_list != NULL)
  {
    cache_skill = game_unit->skill_list;
    game_unit->skill_list = game_unit->skill_list->next;
    
    /* free sunds */
    if(cache_skill->cast_sound != NULL)
      Mix_FreeChunk(cache_skill->cast_sound);
    
    if(cache_skill->object_start_sound != NULL)
      Mix_FreeChunk(cache_skill->object_start_sound);
    
    if(cache_skill->object_hit_sound != NULL)
      Mix_FreeChunk(cache_skill->object_hit_sound);
    
    if(cache_skill->self_enchantment_sound != NULL)
      Mix_FreeChunk(cache_skill->self_enchantment_sound);
    
    if(cache_skill->target_enchantment_sound != NULL)
      Mix_FreeChunk(cache_skill->target_enchantment_sound);
    
    /* free surfaces */
    SDL_FreeSurface(cache_skill->icon);
    SDL_FreeSurface(cache_skill->cast_sprite_sheet);
    SDL_FreeSurface(cache_skill->object_sprite_sheet);
    SDL_FreeSurface(cache_skill->object_hit_sprite_sheet);
    
    if(cache_skill->self_enchantment != NULL)
    {
      SDL_FreeSurface(cache_skill->self_enchantment->icon);
      SDL_FreeSurface(cache_skill->self_enchantment->sprite_sheet);
      
      free(cache_skill->self_enchantment);
    }
    
    if(cache_skill->target_enchantment != NULL)
    {
      SDL_FreeSurface(cache_skill->target_enchantment->icon);
      SDL_FreeSurface(cache_skill->target_enchantment->sprite_sheet);
      
      free(cache_skill->target_enchantment);
    }
    
    free(cache_skill);
  }
  
  /* free skill_objects */
  while(game_unit->skill_object_list != NULL)
  {
    cache_skill_object = game_unit->skill_object_list;
    game_unit->skill_object_list = game_unit->skill_object_list->next;
    
    free(cache_skill_object);
  }
  
  free(game_unit);
}

void move_game_unit(new_game_unit *game_unit)
{
  new_skill_object *cache_skill_object = game_unit->skill_object_list;
  float temp_speed;
  
  /* move game_unit, if hp is > 0 and sprite_mode is mode_walk */
  if(game_unit->hp > 0 &&
    game_unit->sprite_mode == mode_walk &&
    game_unit->direction != dir_none &&
    game_unit->stats.walk_speed > 1)
  {
    temp_speed = (float)(game_unit->stats.walk_speed)/10.0;

    if(game_unit->direction != dir_up && game_unit->direction != dir_down && game_unit->direction != dir_left && game_unit->direction != dir_right)
      temp_speed *= 0.85;
     
    temp_speed *= (float)(SDL_GetTicks() - game_unit->last_step)/(float)((55 - game_unit->stats.walk_speed/3));

    if(split_dir_x(game_unit->direction) == dir_left)
      game_unit->x -= temp_speed;
    else if(split_dir_x(game_unit->direction) == dir_right)
      game_unit->x += temp_speed;
     
    if(split_dir_y(game_unit->direction) == dir_up)
      game_unit->y -= temp_speed;
    else if(split_dir_y(game_unit->direction) == dir_down)
      game_unit->y += temp_speed;

    game_unit->last_step = SDL_GetTicks();
  }
  else
    game_unit->last_step = SDL_GetTicks();
  
  /* handle skill_objects */
  while(cache_skill_object != NULL)
  {
    temp_speed = (float)(cache_skill_object->speed)/10.0;
    
    if(cache_skill_object->direction != dir_up && cache_skill_object->direction != dir_down && cache_skill_object->direction != dir_left && cache_skill_object->direction != dir_right)
      temp_speed *= 0.85;
     
    temp_speed *= (float)(SDL_GetTicks() - cache_skill_object->last_step)/(float)((55 - cache_skill_object->speed/3));
    
    if(split_dir_x(cache_skill_object->direction) == dir_left)
      cache_skill_object->x -= temp_speed;
    else if(split_dir_x(cache_skill_object->direction) == dir_right)
      cache_skill_object->x += temp_speed;
     
    if(split_dir_y(cache_skill_object->direction) == dir_up)
      cache_skill_object->y -= temp_speed;
    else if(split_dir_y(cache_skill_object->direction) == dir_down)
      cache_skill_object->y += temp_speed;
      
    cache_skill_object->last_step = SDL_GetTicks();
    
    cache_skill_object = cache_skill_object->next;
  }
}
void animate_game_unit(new_game_unit *game_unit)
{
  new_enchantment *cache_enchantment = game_unit->enchantment_list;
  new_skill_object *cache_skill_object = game_unit->skill_object_list;
  
  if(game_unit->stats.walk_speed > 0 &&
    game_unit->hp > 0 &&
    ((game_unit->sprite_mode == mode_stand) || (game_unit->sprite_mode == mode_walk)) &&
    game_unit->current_skill == NULL)
  {
    /* set direction */
    switch(game_unit->direction)
    {
      case dir_left:
        game_unit->sprite_src.y = 0;
        break;
      case dir_up_left:
        game_unit->sprite_src.y = game_unit->sprite_src.h;
        break;
      case dir_up:
        game_unit->sprite_src.y = game_unit->sprite_src.h * 2;
        break;
      case dir_up_right:
        game_unit->sprite_src.y = game_unit->sprite_src.h * 3;
        break;
      case dir_right:
        game_unit->sprite_src.y = game_unit->sprite_src.h * 4;
        break;
      case dir_down_right:
        game_unit->sprite_src.y = game_unit->sprite_src.h * 5;
        break;
      case dir_down:
        game_unit->sprite_src.y = game_unit->sprite_src.h * 6;
        break;
      case dir_down_left:
        game_unit->sprite_src.y = game_unit->sprite_src.h * 7;
        break;
    }
  }
  
  /* set frame */
  if(game_unit->hp > 0 &&
    !(game_unit->sprite_mode == mode_cast && SDL_GetTicks() - game_unit->time_stop < (605 - game_unit->stats.cast_speed * 6) * 4) &&
    !(game_unit->sprite_mode == mode_bow && SDL_GetTicks() - game_unit->time_stop < (305 - game_unit->stats.atk_speed * 3) * 4) &&
    !(game_unit->sprite_mode == mode_atk && SDL_GetTicks() - game_unit->time_stop < (205 - game_unit->stats.atk_speed * 2) * 4))
  {
    if(game_unit->sprite_mode != mode_stand && game_unit->direction == dir_none)
    {
      game_unit->sprite_mode = mode_stand;
      game_unit->time_stop = SDL_GetTicks();
    }
    else if(game_unit->sprite_mode != mode_walk && game_unit->direction != dir_none)
    {
      game_unit->sprite_mode = mode_walk;
      game_unit->time_stop = SDL_GetTicks();
    }
  }
  
  if(game_unit->hp <= 0)
  {
    if(SDL_GetTicks() - game_unit->time_stop > 500)
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * (game_unit->frame_die + 5);
    }
    else if(SDL_GetTicks() - game_unit->time_stop > 400)
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * (game_unit->frame_die + 4);
    }
    else if(SDL_GetTicks() - game_unit->time_stop > 300)
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * (game_unit->frame_die + 3);
    }
    else if(SDL_GetTicks() - game_unit->time_stop > 200)
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * (game_unit->frame_die + 2);
    }
    else if(SDL_GetTicks() - game_unit->time_stop > 100)
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * (game_unit->frame_die + 1);
    }
    else
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * game_unit->frame_die;
    }
  }
  else if((game_unit->sprite_mode == mode_stand) || (game_unit->stats.walk_speed < 1))
  {
    if(SDL_GetTicks() - game_unit->time_stop > 1200)
    {
      game_unit->time_stop = SDL_GetTicks();
    }
    else if(SDL_GetTicks() - game_unit->time_stop > 1000)
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w;
    }
    else if(SDL_GetTicks() - game_unit->time_stop > 800)
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * 2;
    }
    else if(SDL_GetTicks() - game_unit->time_stop > 600)
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * 3;
    }
    else if(SDL_GetTicks() - game_unit->time_stop > 400)
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * 2;
    }
    else if(SDL_GetTicks() - game_unit->time_stop > 200)
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w;
    }
    else
    {
      game_unit->sprite_src.x = 0;
    }
  }
  else if(game_unit->sprite_mode == mode_walk)
  {
    if(SDL_GetTicks() - game_unit->time_stop > (175 - game_unit->stats.walk_speed * 1.5) * 8)
    {
      game_unit->time_stop = SDL_GetTicks();
    }
    else if(SDL_GetTicks() - game_unit->time_stop > (175 - game_unit->stats.walk_speed * 1.5) * 7)
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * 11;
    }
    else if(SDL_GetTicks() - game_unit->time_stop > (175 - game_unit->stats.walk_speed * 1.5) * 6)
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * 10;
    }
    else if(SDL_GetTicks() - game_unit->time_stop > (175 - game_unit->stats.walk_speed * 1.5) * 5)
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * 9;
    }
    else if(SDL_GetTicks() - game_unit->time_stop > (175 - game_unit->stats.walk_speed * 1.5) * 4)
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * 8;
    }
    else if(SDL_GetTicks() - game_unit->time_stop > (175 - game_unit->stats.walk_speed * 1.5) * 3)
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * 7;
    }
    else if(SDL_GetTicks() - game_unit->time_stop > (175 - game_unit->stats.walk_speed * 1.5) * 2)
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * 6;
    }
    else if(SDL_GetTicks() - game_unit->time_stop > 175 - game_unit->stats.walk_speed * 1.5)
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * 5;
    }
    else
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * 4;
    }
  }
  else if(game_unit->sprite_mode == mode_atk)
  {
    if(SDL_GetTicks() - game_unit->time_stop > (205 - game_unit->stats.atk_speed * 2) * 3)
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * (game_unit->frame_atk + 3);
    }
    else if(SDL_GetTicks() - game_unit->time_stop > (205 - game_unit->stats.atk_speed * 2) * 2)
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * (game_unit->frame_atk + 2);
    }
    else if(SDL_GetTicks() - game_unit->time_stop > (205 - game_unit->stats.atk_speed * 2))
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * (game_unit->frame_atk + 1);
    }
    else
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * game_unit->frame_atk;
    }
  }
  else if(game_unit->sprite_mode == mode_cast)
  {
    if(SDL_GetTicks() - game_unit->time_stop > 605 - game_unit->stats.cast_speed * 6)
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * (game_unit->frame_cast + 3);
    }
    else if(SDL_GetTicks() - game_unit->time_stop > 300)
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * (game_unit->frame_cast + 2);
    }
    else if(SDL_GetTicks() - game_unit->time_stop > 150)
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * (game_unit->frame_cast + 1);
    }
    else
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * game_unit->frame_cast;
    }
  }
  else if(game_unit->sprite_mode == mode_bow)
  {
    if(SDL_GetTicks() - game_unit->time_stop > (305 - game_unit->stats.atk_speed * 3) * 3)
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * (game_unit->frame_bow + 3);
    }
    else if(SDL_GetTicks() - game_unit->time_stop > (305 - game_unit->stats.atk_speed * 3) * 2)
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * (game_unit->frame_bow + 2);
    }
    else if(SDL_GetTicks() - game_unit->time_stop > (305 - game_unit->stats.atk_speed * 3))
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * (game_unit->frame_bow + 1);
    }
    else
    {
      game_unit->sprite_src.x = game_unit->sprite_src.w * game_unit->frame_bow;
    }
  }
  
  /* handle enchantment animations */
  while(cache_enchantment != NULL)
  {
    if(cache_enchantment->sprite_sheet != NULL &&
      SDL_GetTicks() - cache_enchantment->animation_time_stop > 100)
    {
      if(cache_enchantment->sprite_src.x + cache_enchantment->sprite_src.w >= cache_enchantment->sprite_sheet->w)
        cache_enchantment->sprite_src.x = 0;
      else
        cache_enchantment->sprite_src.x += cache_enchantment->sprite_src.w;
      
      cache_enchantment->animation_time_stop = SDL_GetTicks();
    }
    cache_enchantment = cache_enchantment->next;
  }
  
  /* handle casting_animation */
  if(game_unit->current_skill != NULL &&
    game_unit->current_skill->cast_sprite_sheet != NULL &&
    SDL_GetTicks() - game_unit->current_skill->animation_time_stop > 100)
  {
    if(game_unit->current_skill->cast_sprite_src.x + game_unit->current_skill->cast_sprite_src.w >= game_unit->current_skill->cast_sprite_sheet->w)
      game_unit->current_skill->cast_sprite_src.x = 0;
    else
      game_unit->current_skill->cast_sprite_src.x += game_unit->current_skill->cast_sprite_src.w;
    
    game_unit->current_skill->animation_time_stop = SDL_GetTicks();
  }
  
  /* handle new_skill_object animations */
  while(cache_skill_object != NULL)
  {
    /* set direction */
    switch(cache_skill_object->direction)
    {
      case dir_left:
        cache_skill_object->sprite_src.y = 0;
        break;
      case dir_up_left:
        cache_skill_object->sprite_src.y = cache_skill_object->sprite_src.h;
        break;
      case dir_up:
        cache_skill_object->sprite_src.y = cache_skill_object->sprite_src.h * 2;
        break;
      case dir_up_right:
        cache_skill_object->sprite_src.y = cache_skill_object->sprite_src.h * 3;
        break;
      case dir_right:
        cache_skill_object->sprite_src.y = cache_skill_object->sprite_src.h * 4;
        break;
      case dir_down_right:
        cache_skill_object->sprite_src.y = cache_skill_object->sprite_src.h * 5;
        break;
      case dir_down:
        cache_skill_object->sprite_src.y = cache_skill_object->sprite_src.h * 6;
        break;
      case dir_down_left:
        cache_skill_object->sprite_src.y = cache_skill_object->sprite_src.h * 7;
        break;
    }
    
    if(cache_skill_object->sprite_sheet != NULL && SDL_GetTicks() - cache_skill_object->time_stop > 100)
    {
      if(cache_skill_object->sprite_src.x + cache_skill_object->sprite_src.w >= cache_skill_object->sprite_sheet->w)
        cache_skill_object->sprite_src.x = 0;
      else
        cache_skill_object->sprite_src.x += cache_skill_object->sprite_src.w;
      
      cache_skill_object->time_stop = SDL_GetTicks();
    }
    
    cache_skill_object = cache_skill_object->next;
  }
}

void extend_game_unit_time(new_game_unit *game_unit, const Uint32 time)
{
  new_enchantment *cache_enchantment = game_unit->enchantment_list;
  new_skill *cache_skill = game_unit->skill_list;
  new_skill_object *cache_skill_object = game_unit->skill_object_list;
  
  game_unit->last_step += time;
  game_unit->time_stop += time;
  game_unit->last_hp_reg += time;
  game_unit->last_mp_reg += time;
  
  /* enchantments */
  while(cache_enchantment != NULL)
  {
    cache_enchantment->animation_start_time += time;
    cache_enchantment->animation_time_stop += time;
    
    cache_enchantment->start_time += time;
    cache_enchantment->effect_timestop += time;
    
    cache_enchantment = cache_enchantment->next;
  }
  
  /* skills */
  while(cache_skill != NULL)
  {
    cache_skill->animation_time_stop += time;
    cache_skill->start_time += time;
    
    cache_skill = cache_skill->next;
  }
  
  game_unit->current_skill_start_time += time;
  
  /* skill objects */
  while(cache_skill_object != NULL)
  {
    cache_skill_object->start_time += time;
    cache_skill_object->last_step += time;
    cache_skill_object->time_stop += time;
    
    cache_skill_object = cache_skill_object->next;
  }
}
void update_game_unit_stats(new_game_unit *game_unit)
{
  new_enchantment *cache_enchantment = game_unit->enchantment_list;
  game_unit->stats = game_unit->base_stats;
  
  while(cache_enchantment != NULL)
  {
    game_unit->stats.max_hp += cache_enchantment->stats.max_hp;
    game_unit->stats.hp_reg_rate += cache_enchantment->stats.hp_reg_rate;

    game_unit->stats.max_mp += cache_enchantment->stats.max_mp;
    game_unit->stats.mp_reg_rate += cache_enchantment->stats.mp_reg_rate;

    game_unit->stats.p_atk += cache_enchantment->stats.p_atk;
    game_unit->stats.p_def += cache_enchantment->stats.p_def;
    game_unit->stats.atk_speed += cache_enchantment->stats.atk_speed;

    game_unit->stats.m_atk += cache_enchantment->stats.m_atk;
    game_unit->stats.m_def += cache_enchantment->stats.m_def;
    game_unit->stats.cast_speed += cache_enchantment->stats.cast_speed;

    game_unit->stats.walk_speed += cache_enchantment->stats.walk_speed;

    game_unit->stats.physical_changse += cache_enchantment->stats.physical_changse;
    game_unit->stats.physical_resistance += cache_enchantment->stats.physical_resistance;

    game_unit->stats.magic_changse += cache_enchantment->stats.magic_changse;
    game_unit->stats.magic_resistance += cache_enchantment->stats.magic_resistance;
    
    game_unit->stats.reflect_changse += cache_enchantment->stats.reflect_changse;
    game_unit->stats.block_changse += cache_enchantment->stats.block_changse;
    game_unit->stats.absorb_hp_rate += cache_enchantment->stats.absorb_hp_rate;
    game_unit->stats.absorb_mp_rate += cache_enchantment->stats.absorb_mp_rate;
    
    cache_enchantment = cache_enchantment->next;
  }
  
  
  if(game_unit->stats.max_hp < 0)
    game_unit->stats.max_hp = 0;
  
  if(game_unit->stats.hp_reg_rate < 0)
    game_unit->stats.hp_reg_rate = 0;
  else if(game_unit->stats.hp_reg_rate > 100)
    game_unit->stats.hp_reg_rate = 100;
  
  if(game_unit->stats.max_mp < 0)
    game_unit->stats.max_mp = 0;
  
  if(game_unit->stats.mp_reg_rate < 0)
    game_unit->stats.mp_reg_rate = 0;
  else if(game_unit->stats.mp_reg_rate > 100)
    game_unit->stats.mp_reg_rate = 100;
  
  if(game_unit->stats.p_atk < 0)
    game_unit->stats.p_atk = 0;
  
  if(game_unit->stats.p_def < 0)
    game_unit->stats.p_def = 0;
  
  if(game_unit->stats.atk_speed < 0)
    game_unit->stats.atk_speed = 0;
  else if(game_unit->stats.atk_speed > 100)
    game_unit->stats.atk_speed = 100;
  
  if(game_unit->stats.m_atk < 0)
    game_unit->stats.m_atk = 0;
  
  if(game_unit->stats.m_def < 0)
    game_unit->stats.m_def = 0;
  
  if(game_unit->stats.cast_speed < 0)
    game_unit->stats.cast_speed = 0;
  else if(game_unit->stats.cast_speed > 100)
    game_unit->stats.cast_speed = 100;
  
  if(game_unit->stats.walk_speed < 0)
    game_unit->stats.walk_speed = 0;
  else if(game_unit->stats.walk_speed > 100)
    game_unit->stats.walk_speed = 100;
  
  if(game_unit->stats.physical_changse < 0)
    game_unit->stats.physical_changse = 0;
  
  if(game_unit->stats.physical_resistance < 0)
    game_unit->stats.physical_resistance = 0;
  
  if(game_unit->stats.magic_changse < 0)
    game_unit->stats.magic_changse = 0;
  
  if(game_unit->stats.magic_resistance < 0)
    game_unit->stats.magic_resistance = 0;
  
  if(game_unit->stats.reflect_changse < 0)
    game_unit->stats.reflect_changse = 0;
  
  if(game_unit->stats.block_changse < 0)
    game_unit->stats.block_changse = 0;
  
  if(game_unit->stats.absorb_hp_rate < 0)
    game_unit->stats.absorb_hp_rate = 0;
  else if(game_unit->stats.absorb_hp_rate > 100)
    game_unit->stats.absorb_hp_rate = 100;
  
  if(game_unit->stats.absorb_mp_rate < 0)
    game_unit->stats.absorb_mp_rate = 0;
  else if(game_unit->stats.absorb_mp_rate > 100)
    game_unit->stats.absorb_mp_rate = 100;
}
void handle_game_unit(new_game_unit *game_unit)
{
  new_enchantment *cache_enchantment = game_unit->enchantment_list;
  new_enchantment *cache_enchantment_2 = game_unit->enchantment_list;
  
  new_skill_object *cache_skill_object = game_unit->skill_object_list;
  new_skill_object *cache_skill_object_2 = game_unit->skill_object_list;
  
  /* handle hp and mp regeneration */
  if(game_unit->stats.hp_reg_rate <= 0)
    game_unit->last_hp_reg = SDL_GetTicks();
  else if(game_unit->hp > 0 && SDL_GetTicks() - game_unit->last_hp_reg > (105 - game_unit->stats.hp_reg_rate) * 20)
  {
    game_unit->hp += ((SDL_GetTicks() - game_unit->last_hp_reg)/((105 - game_unit->stats.hp_reg_rate) * 20)) * ((int)((float)(game_unit->stats.max_hp)/100.0));
    game_unit->last_hp_reg = SDL_GetTicks();
  }
  
  if(game_unit->stats.mp_reg_rate <= 0)
    game_unit->last_mp_reg = SDL_GetTicks();
  else if(game_unit->hp > 0 && SDL_GetTicks() - game_unit->last_mp_reg > (105 - game_unit->stats.mp_reg_rate) * 20)
  {
    game_unit->mp += ((SDL_GetTicks() - game_unit->last_mp_reg)/((105 - game_unit->stats.mp_reg_rate) * 20)) * (int)((float)(game_unit->stats.max_mp)/100.0);
    game_unit->last_mp_reg = SDL_GetTicks();
  }
  
  /* handle enchantments */
  while(cache_enchantment != NULL)
  {
    if(SDL_GetTicks() - cache_enchantment->start_time > cache_enchantment->life_time)
    {
      if(cache_enchantment == game_unit->enchantment_list)
      {
        game_unit->enchantment_list = game_unit->enchantment_list->next;
        free(cache_enchantment);
        cache_enchantment = game_unit->enchantment_list;
        continue;
      }
      else
      {
        cache_enchantment_2->next = cache_enchantment_2->next->next;
        free(cache_enchantment);
        cache_enchantment = cache_enchantment_2->next;
        continue;
      }
    }
    else if(game_unit->hp > 0 && cache_enchantment->effect_interval > 0 && SDL_GetTicks() - cache_enchantment->effect_timestop > cache_enchantment->effect_interval)
    {
      game_unit->hp += (SDL_GetTicks() - cache_enchantment->effect_timestop)/cache_enchantment->effect_interval * cache_enchantment->hp_incrase;
      game_unit->mp += (SDL_GetTicks() - cache_enchantment->effect_timestop)/cache_enchantment->effect_interval * cache_enchantment->mp_incrase;
      
      if(game_unit->hp <= 0)
        game_unit->time_stop = SDL_GetTicks();
      
      cache_enchantment->effect_timestop = SDL_GetTicks();
    }
    
    cache_enchantment_2 = cache_enchantment;
    cache_enchantment = cache_enchantment->next;
  }
  
  /* handle skill_objects */
  while(cache_skill_object != NULL)
  {
    /* check life_time of cache_skill_object */
    if((cache_skill_object->direction == dir_none && cache_skill_object->sprite_sheet == NULL) ||
      (SDL_GetTicks() - cache_skill_object->start_time > cache_skill_object->life_time))
    {
      if(cache_skill_object == game_unit->skill_object_list)
      {
        game_unit->skill_object_list = game_unit->skill_object_list->next;
        free(cache_skill_object);
        cache_skill_object = game_unit->skill_object_list;
        continue;
      }
      else
      {
        cache_skill_object_2->next = cache_skill_object_2->next->next;
        free(cache_skill_object);
        cache_skill_object = cache_skill_object_2->next;
        continue;
      }
    }
    
    cache_skill_object_2 = cache_skill_object;
    cache_skill_object = cache_skill_object->next;
  }
  
  /* correct hp and mp */
  if(game_unit->hp < 0)
    game_unit->hp = 0;
  else if(game_unit->hp > game_unit->stats.max_hp)
    game_unit->hp = game_unit->stats.max_hp;
  
  if(game_unit->mp < 0)
    game_unit->mp = 0;
  else if(game_unit->mp > game_unit->stats.max_mp)
    game_unit->mp = game_unit->stats.max_mp;
  
  /* interrupt current skill */
  if(game_unit->current_skill != NULL &&
    ((game_unit->hp <= 0) ||
    (game_unit->hp < game_unit->current_skill->hp_cost) ||
    (game_unit->mp < game_unit->current_skill->mp_cost) ||
    (game_unit->current_skill->skill_type == magic_skill && game_unit->stats.cast_speed <= 0) ||
    (game_unit->current_skill->skill_type != magic_skill && game_unit->stats.atk_speed <= 0)))
  {
    game_unit->sprite_mode = mode_stand;
    game_unit->current_skill = NULL;
  }
  
  /* handle current skill */
  if(game_unit->current_skill != NULL &&
    ((game_unit->current_skill->skill_type == physical_skill && SDL_GetTicks() - game_unit->current_skill_start_time > (105 - game_unit->stats.atk_speed) * 4) ||
    (game_unit->current_skill->skill_type == ranged_skill && SDL_GetTicks() - game_unit->current_skill_start_time > (205 - game_unit->stats.atk_speed * 2) * 4) ||
    (game_unit->current_skill->skill_type == magic_skill && SDL_GetTicks() - game_unit->current_skill_start_time > (605 - game_unit->stats.cast_speed * 6) * 4)))
  {
    game_unit->hp -= game_unit->current_skill->hp_cost;
    game_unit->mp -= game_unit->current_skill->mp_cost;
    
    if(game_unit->hp <= 0)
      game_unit->time_stop = SDL_GetTicks();
    
    if(game_unit->current_skill->self_enchantment != NULL)
    {
      append_enchantment(game_unit, game_unit->current_skill->self_enchantment);
      
      if(game_unit->current_skill->self_enchantment_sound != NULL)
        Mix_PlayChannel(-1, game_unit->current_skill->self_enchantment_sound, 0);
    }
    
    append_current_skill_object(game_unit);
    
    game_unit->current_skill->start_time = SDL_GetTicks();
    game_unit->current_skill = NULL;
  }
}
void handle_game_unit_damage(new_game_unit *game_unit, new_game_unit *game_unit_target)
{
  new_skill_object *cache_skill_object = game_unit->skill_object_list;
  int cache_mode = damage_normal;
  int cache_damage;
  
  while(cache_skill_object != NULL)
  {
    /* check collision between cache_skill_object and game_unit_target */
    if(game_unit_target->hp > 0 &&
      cache_skill_object->direction != dir_none &&
      cache_skill_object->x + cache_skill_object->collision_w >= game_unit_target->x - game_unit->collision_w &&
      cache_skill_object->x - cache_skill_object->collision_w <= game_unit_target->x + game_unit->collision_w &&
      cache_skill_object->y + cache_skill_object->collision_h >= game_unit_target->y &&
      cache_skill_object->y - cache_skill_object->collision_h <= game_unit_target->y + game_unit->collision_h)
    {
      /* play sound */
      if(cache_skill_object->hit_sound != NULL)
        Mix_PlayChannel(-1, cache_skill_object->hit_sound, 0);
      
      if(game_unit_target->stats.reflect_changse > 0 && rand() % 100 + 1 <= game_unit_target->stats.reflect_changse)
      {
        reflect_skill_object(game_unit, cache_skill_object, game_unit_target);
        cache_skill_object = game_unit->skill_object_list;
        append_damage(game_unit_target->x, game_unit_target->y, 0, damage_reflect);
        continue;
      }
      else if(game_unit_target->stats.block_changse > 0 && rand() % 100 + 1 <= game_unit_target->stats.block_changse)
        append_damage(game_unit_target->x, game_unit_target->y, 0, damage_block);
      else
      {
        /* apply enchantment */
        if(cache_skill_object->enchantment != NULL &&
          ((cache_skill_object->base_changse > 0 && rand() % 100 + 1 <= cache_skill_object->base_changse) ||
        
          (cache_skill_object->base_changse <= 0 &&
          cache_skill_object->physical_changse - game_unit_target->stats.physical_resistance > 0 &&
          rand() % 100 + 1 <= cache_skill_object->physical_changse - game_unit_target->stats.physical_resistance) ||
          
          (cache_skill_object->base_changse <= 0 &&
          cache_skill_object->magic_changse - game_unit_target->stats.magic_resistance > 0 &&
          rand() % 100 + 1 <= cache_skill_object->magic_changse - game_unit_target->stats.magic_resistance)))
        {
          append_enchantment(game_unit_target, cache_skill_object->enchantment);
          
          if(cache_skill_object->enchantment_sound != NULL)
            Mix_PlayChannel(-1, cache_skill_object->enchantment_sound, 0);
          
          cache_mode = damage_success;
        }
        
        /* apply damage, calculatet with hp/mp absorb rate */
        if(cache_skill_object->p_atk - game_unit_target->stats.p_def > 0)
        {
          cache_damage = (cache_skill_object->p_atk - game_unit_target->stats.p_def) -
                        ((float)(game_unit_target->stats.absorb_hp_rate)/100.0 * (cache_skill_object->p_atk - game_unit_target->stats.p_def)) -
                        ((float)(game_unit_target->stats.absorb_mp_rate)/100.0 * (cache_skill_object->p_atk - game_unit_target->stats.p_def));
          
          if(cache_damage > 0)
            game_unit_target->hp -= cache_damage;
          
          game_unit_target->hp += (float)(game_unit_target->stats.absorb_hp_rate)/100.0 * (cache_skill_object->p_atk - game_unit_target->stats.p_def);
          game_unit_target->mp += (float)(game_unit_target->stats.absorb_mp_rate)/100.0 * (cache_skill_object->p_atk - game_unit_target->stats.p_def);
          
          if(cache_damage > 0)
            append_damage(game_unit_target->x, game_unit_target->y, cache_damage, cache_mode);
          else
            append_damage(game_unit_target->x, game_unit_target->y, 0, cache_mode);
        }
        else if(cache_skill_object->m_atk - game_unit_target->stats.m_def > 0)
        {
          cache_damage = (cache_skill_object->m_atk - game_unit_target->stats.m_def) -
                        ((float)(game_unit_target->stats.absorb_hp_rate)/100.0 * (cache_skill_object->m_atk - game_unit_target->stats.m_def)) -
                        ((float)(game_unit_target->stats.absorb_mp_rate)/100.0 * (cache_skill_object->m_atk - game_unit_target->stats.m_def));
          
          if(cache_damage > 0)
            game_unit_target->hp -= cache_damage;
          
          game_unit_target->hp += (float)(game_unit_target->stats.absorb_hp_rate)/100.0 * (cache_skill_object->m_atk - game_unit_target->stats.m_def);
          game_unit_target->mp += (float)(game_unit_target->stats.absorb_mp_rate)/100.0 * (cache_skill_object->m_atk - game_unit_target->stats.m_def);
          
          if(cache_damage > 0)
            append_damage(game_unit_target->x, game_unit_target->y, cache_damage, cache_mode);
          else
            append_damage(game_unit_target->x, game_unit_target->y, 0, cache_mode);
        }
        else
          append_damage(game_unit_target->x, game_unit_target->y, 0, cache_mode);
        
        if(game_unit_target->hp <= 0)
          game_unit_target->time_stop = SDL_GetTicks();
      }
      
      /* set cache_skill_object to hit animation mode */
      cache_skill_object->direction = dir_none;
      cache_skill_object->sprite_sheet = cache_skill_object->hit_sprite_sheet;
      
      if(cache_skill_object->sprite_sheet != NULL)
      {
        cache_skill_object->x = game_unit_target->x;
        cache_skill_object->y = game_unit_target->y;
        
        cache_skill_object->sprite_src.x = 0;
        cache_skill_object->sprite_src.y = 0;
        cache_skill_object->sprite_src.w = cache_skill_object->sprite_sheet->h;
        cache_skill_object->sprite_src.h = cache_skill_object->sprite_sheet->h;
        
        cache_skill_object->time_stop = SDL_GetTicks();
        cache_skill_object->start_time = SDL_GetTicks();
        cache_skill_object->life_time = 100 * (int)(cache_skill_object->sprite_sheet->w/cache_skill_object->sprite_sheet->h);
      }
    }
    
    cache_skill_object = cache_skill_object->next;
  }
}

void use_skill(new_game_unit *game_unit, new_skill *skill)
{
  /* add skill to a game unit */
  /* check if game unit is not casting already, and skill is ready to re-use */
  if(game_unit->current_skill == NULL &&
    ((game_unit->sprite_mode == mode_stand) || (game_unit->sprite_mode == mode_walk)) &&
    SDL_GetTicks() - skill->start_time > skill->reuse_time &&
    game_unit->hp > 0 &&
    game_unit->hp >= skill->hp_cost &&
    game_unit->mp >= skill->mp_cost)
  {
    if(skill->skill_type == physical_skill && game_unit->stats.atk_speed > 0)
      game_unit->sprite_mode = mode_atk;
    else if(skill->skill_type == ranged_skill && game_unit->stats.atk_speed > 0)
      game_unit->sprite_mode = mode_bow;
    else if(skill->skill_type == magic_skill && game_unit->stats.cast_speed > 0)
      game_unit->sprite_mode = mode_cast;
    else
      return;
    
    game_unit->current_skill = skill;
    game_unit->current_skill_start_time = SDL_GetTicks();
    
    game_unit->time_stop = SDL_GetTicks();
    
    if(game_unit->current_skill->cast_sound != NULL)
      Mix_PlayChannel(-1, game_unit->current_skill->cast_sound, 0);
  }
}

void append_current_skill_object(new_game_unit *game_unit)
{
  new_skill_object *cache_skill_object = game_unit->skill_object_list;
  int counter = 0;
  
  if((game_unit->current_skill == NULL) || (game_unit->current_skill->object_life_time <= 0))
    return;
  
  while(counter < game_unit->current_skill->object_amount)
  {
    /* create first new_skill_object element */
    if(game_unit->skill_object_list == NULL)
    {
      game_unit->skill_object_list = malloc(sizeof(struct new_skill_object));
      cache_skill_object = game_unit->skill_object_list;
    }
    else
    {
      /* search last element in list and append new */
      cache_skill_object = game_unit->skill_object_list;
      while(cache_skill_object->next != NULL)
        cache_skill_object = cache_skill_object->next;
      
      cache_skill_object->next = malloc(sizeof(struct new_skill_object));
      cache_skill_object = cache_skill_object->next;
    }
    
    /* other stats */
    cache_skill_object->collision_w = game_unit->current_skill->collision_w;
    cache_skill_object->collision_h = game_unit->current_skill->collision_h;
    
    cache_skill_object->start_time = SDL_GetTicks();
    cache_skill_object->life_time = game_unit->current_skill->object_life_time;
    
    cache_skill_object->speed = game_unit->current_skill->object_speed;
    cache_skill_object->last_step = SDL_GetTicks();
    
    /* set direction */
    if(game_unit->current_skill->object_random_direction)
    {
      switch(rand() % 8)
      {
        case 0:
          cache_skill_object->direction = dir_down;
          break;
        case 1:
          cache_skill_object->direction = dir_down_left;
          break;
        case 2:
          cache_skill_object->direction = dir_down_right;
          break;
        case 3:
          cache_skill_object->direction = dir_left;
          break;
        case 4:
          cache_skill_object->direction = dir_right;
          break;
        case 5:
          cache_skill_object->direction = dir_up;
          break;
        case 6:
          cache_skill_object->direction = dir_up_left;
          break;
        case 7:
          cache_skill_object->direction = dir_up_right;
          break;
      }
      
    }
    else
    {
      /* get game_unit direction */
      if(game_unit->sprite_src.y == 0)
        cache_skill_object->direction = dir_left;
      else if(game_unit->sprite_src.y == game_unit->sprite_src.h)
        cache_skill_object->direction = dir_up_left;
      else if(game_unit->sprite_src.y == game_unit->sprite_src.h * 2)
        cache_skill_object->direction = dir_up;
      else if(game_unit->sprite_src.y == game_unit->sprite_src.h * 3)
        cache_skill_object->direction = dir_up_right;
      else if(game_unit->sprite_src.y == game_unit->sprite_src.h * 4)
        cache_skill_object->direction = dir_right;
      else if(game_unit->sprite_src.y == game_unit->sprite_src.h * 5)
        cache_skill_object->direction = dir_down_right;
      else if(game_unit->sprite_src.y == game_unit->sprite_src.h * 6)
        cache_skill_object->direction = dir_down;
      else if(game_unit->sprite_src.y == game_unit->sprite_src.h * 7)
        cache_skill_object->direction = dir_down_left;
    }
    
    /* handle skill object spawn position */
    if(game_unit->current_skill->object_amount > 1)
    {
      cache_skill_object->x = rand() % (game_unit->collision_w * 2) + (game_unit->x - game_unit->collision_w);
      cache_skill_object->y = rand() % (game_unit->collision_h * 2) + (game_unit->y - game_unit->collision_h);
    }
    else if(game_unit->current_skill->skill_type == physical_skill)
    {
      switch(cache_skill_object->direction)
      {
        case dir_down:
          cache_skill_object->x = game_unit->x;
          cache_skill_object->y = game_unit->y + game_unit->collision_h;
          break;
        case dir_down_left:
          cache_skill_object->x = game_unit->x - game_unit->collision_w;
          cache_skill_object->y = game_unit->y + game_unit->collision_h;
          break;
        case dir_down_right:
          cache_skill_object->x = game_unit->x + game_unit->collision_w;
          cache_skill_object->y = game_unit->y + game_unit->collision_h;
          break;
        case dir_left:
          cache_skill_object->x = game_unit->x - game_unit->collision_w;
          cache_skill_object->y = game_unit->y + (int)(game_unit->collision_h/2);
          break;
        case dir_right:
          cache_skill_object->x = game_unit->x + game_unit->collision_w;
          cache_skill_object->y = game_unit->y + (int)(game_unit->collision_h/2);
          break;
        case dir_up:
          cache_skill_object->x = game_unit->x;
          cache_skill_object->y = game_unit->y;
          break;
        case dir_up_left:
          cache_skill_object->x = game_unit->x - game_unit->collision_w;
          cache_skill_object->y = game_unit->y;
          break;
        case dir_up_right:
          cache_skill_object->x = game_unit->x + game_unit->collision_w;
          cache_skill_object->y = game_unit->y;
          break;
      }
    }
    else if(game_unit->current_skill->skill_type == ranged_skill)
    {
      switch(cache_skill_object->direction)
      {
        case dir_down:
          cache_skill_object->x = game_unit->x;
          cache_skill_object->y = game_unit->y;
          break;
        case dir_down_left:
          cache_skill_object->x = game_unit->x;
          cache_skill_object->y = game_unit->y;
          break;
        case dir_down_right:
          cache_skill_object->x = game_unit->x;
          cache_skill_object->y = game_unit->y;
          break;
        case dir_left:
          cache_skill_object->x = game_unit->x - game_unit->collision_w;
          cache_skill_object->y = game_unit->y;
          break;
        case dir_right:
          cache_skill_object->x = game_unit->x + game_unit->collision_w;
          cache_skill_object->y = game_unit->y;
          break;
        case dir_up:
          cache_skill_object->x = game_unit->x;
          cache_skill_object->y = game_unit->y;
          break;
        case dir_up_left:
          cache_skill_object->x = game_unit->x - game_unit->collision_w;
          cache_skill_object->y = game_unit->y - (int)(game_unit->collision_h/2);
          break;
        case dir_up_right:
          cache_skill_object->x = game_unit->x + game_unit->collision_w;
          cache_skill_object->y = game_unit->y - (int)(game_unit->collision_h/2);
          break;
      }
    }
    else
    {
      cache_skill_object->x = game_unit->x;
      cache_skill_object->y = game_unit->y;
    }
    
    /* skill object sprite sheet */
    cache_skill_object->hit_sprite_sheet = game_unit->current_skill->object_hit_sprite_sheet;
    cache_skill_object->hit_sound = game_unit->current_skill->object_hit_sound;
    
    cache_skill_object->sprite_sheet = game_unit->current_skill->object_sprite_sheet;
    if(cache_skill_object->sprite_sheet != NULL)
    {
      cache_skill_object->sprite_src.x = 0;
      cache_skill_object->sprite_src.w = (int)(cache_skill_object->sprite_sheet->h/8);
      cache_skill_object->sprite_src.h = (int)(cache_skill_object->sprite_sheet->h/8);
    }
    
    cache_skill_object->time_stop = SDL_GetTicks();
    
    cache_skill_object->base_changse = game_unit->current_skill->base_changse;
    if(game_unit->current_skill->skill_type == magic_skill)
    {
      cache_skill_object->p_atk = 0;
      cache_skill_object->m_atk = game_unit->stats.m_atk + game_unit->current_skill->m_atk;
      
      cache_skill_object->physical_changse = 0;
      cache_skill_object->magic_changse = game_unit->stats.magic_changse + game_unit->current_skill->magic_changse;
    }
    else
    {
      cache_skill_object->p_atk = game_unit->stats.p_atk + game_unit->current_skill->p_atk;
      cache_skill_object->m_atk = 0;
      
      cache_skill_object->physical_changse = game_unit->stats.physical_changse + game_unit->current_skill->physical_changse;
      cache_skill_object->magic_changse = 0;
    }
    
    cache_skill_object->enchantment_sound = game_unit->current_skill->target_enchantment_sound;
    cache_skill_object->enchantment = game_unit->current_skill->target_enchantment;
    
    cache_skill_object->next = NULL;
    
    /* play sound */
    if(game_unit->current_skill->object_start_sound != NULL)
      Mix_PlayChannel(-1,game_unit->current_skill->object_start_sound, 0);
    
    counter++;
  }
}
void reflect_skill_object(new_game_unit *game_unit, new_skill_object *skill_object, new_game_unit *game_unit_target)
{
  new_skill_object *cache_skill_object = game_unit->skill_object_list;
  new_skill_object *cache_skill_object_2 = game_unit->skill_object_list;
  
  /* invert skill object direction and reset timer */
  skill_object->start_time = SDL_GetTicks();
  switch(skill_object->direction)
  {
    case dir_down:
      skill_object->direction = dir_up;
      break;
    case dir_down_left:
      skill_object->direction = dir_up_right;
      break;
    case dir_down_right:
      skill_object->direction = dir_up_left;
      break;
    case dir_left:
      skill_object->direction = dir_right;
      break;
    case dir_right:
      skill_object->direction = dir_left;
      break;
    case dir_up:
      skill_object->direction = dir_down;
      break;
    case dir_up_left:
      skill_object->direction = dir_down_right;
      break;
    case dir_up_right:
      skill_object->direction = dir_down_left;
      break;
  }
  
  while(cache_skill_object != NULL)
  {
    if(cache_skill_object == skill_object)
    {
      if(cache_skill_object == game_unit->skill_object_list)
      {
        game_unit->skill_object_list = game_unit->skill_object_list->next;
        break;
      }
      else
      {
        cache_skill_object_2->next = cache_skill_object_2->next->next;
        break;
      }
    }
    
    cache_skill_object_2 = cache_skill_object;
    cache_skill_object = cache_skill_object->next;
  }
  
  skill_object->next = NULL;
  
  if(game_unit_target->skill_object_list == NULL)
    game_unit_target->skill_object_list = skill_object;
  else
  {
    cache_skill_object = game_unit_target->skill_object_list;
    
    while(cache_skill_object->next != NULL)
      cache_skill_object = cache_skill_object->next;
    
    cache_skill_object->next = skill_object;
  }
}

void append_enchantment(new_game_unit *game_unit, new_enchantment *enchantment)
{
  new_enchantment *cache_enchantment = game_unit->enchantment_list;
  new_enchantment *cache_enchantment_2 = game_unit->enchantment_list;
  
  if(enchantment == NULL)
    return;
  
  /* if the same enchantment is already appended, the old enchantment gets removed */
  while(cache_enchantment != NULL)
  {
    if(cache_enchantment->life_time == enchantment->life_time &&
      cache_enchantment->effect_interval == enchantment->effect_interval &&
      
      cache_enchantment->hp_incrase == enchantment->hp_incrase &&
      cache_enchantment->mp_incrase == enchantment->mp_incrase &&
      
      cache_enchantment->stats.max_hp == enchantment->stats.max_hp &&
      cache_enchantment->stats.hp_reg_rate == enchantment->stats.hp_reg_rate &&
      
      cache_enchantment->stats.max_mp == enchantment->stats.max_mp &&
      cache_enchantment->stats.mp_reg_rate == enchantment->stats.mp_reg_rate &&
      
      cache_enchantment->stats.p_atk == enchantment->stats.p_atk &&
      cache_enchantment->stats.p_def == enchantment->stats.p_def &&
      cache_enchantment->stats.atk_speed == enchantment->stats.atk_speed &&
      
      cache_enchantment->stats.m_atk == enchantment->stats.m_atk &&
      cache_enchantment->stats.m_def == enchantment->stats.m_def &&
      cache_enchantment->stats.cast_speed == enchantment->stats.cast_speed &&
      
      cache_enchantment->stats.walk_speed == enchantment->stats.walk_speed &&
      
      cache_enchantment->stats.physical_changse == enchantment->stats.physical_changse &&
      cache_enchantment->stats.physical_resistance == enchantment->stats.physical_resistance &&
      
      cache_enchantment->stats.magic_changse == enchantment->stats.magic_changse &&
      cache_enchantment->stats.magic_resistance == enchantment->stats.magic_resistance &&
      
      cache_enchantment->stats.reflect_changse == enchantment->stats.reflect_changse &&
      cache_enchantment->stats.block_changse == enchantment->stats.block_changse &&
      cache_enchantment->stats.absorb_hp_rate == enchantment->stats.absorb_hp_rate &&
      cache_enchantment->stats.absorb_mp_rate == enchantment->stats.absorb_mp_rate)
      {
        if(cache_enchantment == game_unit->enchantment_list)
        {
          game_unit->enchantment_list = game_unit->enchantment_list->next;
          free(cache_enchantment);
          cache_enchantment = game_unit->enchantment_list;
          continue;
        }
        else
        {
          cache_enchantment_2->next = cache_enchantment_2->next->next;
          free(cache_enchantment);
          cache_enchantment = cache_enchantment_2->next;
          continue;
        }
      }
    
    cache_enchantment_2 = cache_enchantment;
    cache_enchantment = cache_enchantment->next;
  }
  
  /* create first enchantment element */
  if(game_unit->enchantment_list == NULL)
  {
    game_unit->enchantment_list = malloc(sizeof(struct new_enchantment));
    cache_enchantment = game_unit->enchantment_list;
  }
  else
  {
    /* search last element in list and append new */
    cache_enchantment = game_unit->enchantment_list;
    while(cache_enchantment->next != NULL)
      cache_enchantment = cache_enchantment->next;
    
    cache_enchantment->next = malloc(sizeof(struct new_enchantment));
    cache_enchantment = cache_enchantment->next;
  }
  
  cache_enchantment->icon = enchantment->icon;
  
  cache_enchantment->sprite_sheet = enchantment->sprite_sheet;
  cache_enchantment->animation_duration = enchantment->animation_duration;
  cache_enchantment->animation_start_time = SDL_GetTicks();
  cache_enchantment->animation_time_stop = SDL_GetTicks();
  
  cache_enchantment->sprite_src = enchantment->sprite_src;
  
  cache_enchantment->start_time = SDL_GetTicks();
  cache_enchantment->life_time = enchantment->life_time;
  
  cache_enchantment->effect_timestop = SDL_GetTicks();
  cache_enchantment->effect_interval = enchantment->effect_interval;
  
  cache_enchantment->hp_incrase = enchantment->hp_incrase;
  cache_enchantment->mp_incrase = enchantment->mp_incrase;
  
  cache_enchantment->stats.max_hp = enchantment->stats.max_hp;
  cache_enchantment->stats.hp_reg_rate = enchantment->stats.hp_reg_rate;
  
  cache_enchantment->stats.max_mp = enchantment->stats.max_mp;
  cache_enchantment->stats.mp_reg_rate = enchantment->stats.mp_reg_rate;
  
  cache_enchantment->stats.p_atk = enchantment->stats.p_atk;
  cache_enchantment->stats.p_def = enchantment->stats.p_def;
  cache_enchantment->stats.atk_speed = enchantment->stats.atk_speed;
  
  cache_enchantment->stats.m_atk = enchantment->stats.m_atk;
  cache_enchantment->stats.m_def = enchantment->stats.m_def;
  cache_enchantment->stats.cast_speed = enchantment->stats.cast_speed;
  
  cache_enchantment->stats.walk_speed = enchantment->stats.walk_speed;
  
  cache_enchantment->stats.physical_changse = enchantment->stats.physical_changse;
  cache_enchantment->stats.physical_resistance = enchantment->stats.physical_resistance;
  
  cache_enchantment->stats.magic_changse = enchantment->stats.magic_changse;
  cache_enchantment->stats.magic_resistance = enchantment->stats.magic_resistance;
  
  cache_enchantment->stats.reflect_changse = enchantment->stats.reflect_changse;
  cache_enchantment->stats.block_changse = enchantment->stats.block_changse;
  cache_enchantment->stats.absorb_hp_rate = enchantment->stats.absorb_hp_rate;
  cache_enchantment->stats.absorb_mp_rate = enchantment->stats.absorb_mp_rate;
  
  cache_enchantment->next = NULL;
}
void cancel_all_enchantments(new_game_unit *game_unit)
{
  new_enchantment *cache_enchantment = NULL;
  
  while(game_unit->enchantment_list != NULL)
  {
    cache_enchantment = game_unit->enchantment_list;
    game_unit->enchantment_list = game_unit->enchantment_list->next;
    
    free(cache_enchantment);
  }
}