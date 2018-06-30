#include <damage.h>

damage_object *damage_list = NULL;
TTF_Font *damage_font[damage_frames];
static const SDL_Color damage_font_color = {255, 255, 0};
static const SDL_Color damage_font_success_color = {0, 255, 255};
static const SDL_Color damage_font_reflect_color = {127, 0, 127};
static const SDL_Color damage_font_block_color = {0, 0, 180};

int load_damage()
{
  int counter = 0;
  while(counter < damage_frames)
  {
    if((damage_font[counter] = TTF_OpenFont("amiri-regular.ttf", counter + 15)) == NULL)
        return -1;
    
    counter++;
  }
  
  return 0;
}
void free_damage()
{
  int counter = 0;
  damage_object *cache_damage_object = NULL;
  
  while(damage_list != NULL)
  {
    cache_damage_object = damage_list;
    damage_list = damage_list->next;
    
    free(cache_damage_object);
  }
  
  while(counter < damage_frames)
  {
    TTF_CloseFont(damage_font[counter]);		
    counter++;
  }
}

void append_damage(const double x, const double y, const int damage, const int mode)
{
  damage_object *cache_damage_object = damage_list;
  
  /* create first damage_list element */
  if(damage_list == NULL)
  {
    damage_list = malloc(sizeof(struct damage_object));
    cache_damage_object = damage_list;
  }
  else
  {
    /* search last element in list and append new */
    while(cache_damage_object->next != NULL)
      cache_damage_object = cache_damage_object->next;
    
    cache_damage_object->next = malloc(sizeof(struct damage_object));
    cache_damage_object = cache_damage_object->next;
  }
  
  cache_damage_object->x = x;
  cache_damage_object->y = y;
  
  cache_damage_object->damage = damage;
  cache_damage_object->damage_mode = mode;
  
  cache_damage_object->start_time = SDL_GetTicks();
  cache_damage_object->time_stop = SDL_GetTicks();
  
  cache_damage_object->next = NULL;
}
void extend_damage_time(const Uint32 time)
{
  damage_object *cache_damage_object = damage_list;
  
  while(cache_damage_object != NULL)
  {
    cache_damage_object->time_stop += time;
    cache_damage_object->start_time += time;
    
    cache_damage_object = cache_damage_object->next;
  }
}

void blit_damage(const new_camera *camera, SDL_Surface *screen)
{
  SDL_Rect bar_src;
  SDL_Rect bar_dst;
  
  SDL_Surface *cache_surface = NULL;
  char cache_string[1024];
  
  damage_object *cache_damage_object = damage_list;
  damage_object *cache_damage_object_2 = damage_list;
  
  while(cache_damage_object != NULL)
  {
    /* remove old damage_list elements */
    if(SDL_GetTicks() - cache_damage_object->start_time > damage_duration)
    {
      if(cache_damage_object == damage_list)
      {
        damage_list = damage_list->next;
        
        free(cache_damage_object);
        
        cache_damage_object = damage_list;
        continue;
      }
      else
      {
        cache_damage_object_2->next = cache_damage_object_2->next->next;
        
        free(cache_damage_object);
        
        cache_damage_object = cache_damage_object_2->next;
        continue;
      }
    }
    
    /* move current damage_list element */
    cache_damage_object->y -= (float)(SDL_GetTicks() - cache_damage_object->time_stop)/(float)(10);
    cache_damage_object->time_stop = SDL_GetTicks();
    
    /* render damage text */
    sprintf(cache_string, "%i", cache_damage_object->damage);
    if(cache_damage_object->damage_mode == damage_success)
      cache_surface = TTF_RenderText_Blended(damage_font[(int)((SDL_GetTicks() - cache_damage_object->start_time)/(damage_duration/damage_frames + 1))],
                                            cache_string, damage_font_success_color);
    else if(cache_damage_object->damage_mode == damage_reflect)
      cache_surface = TTF_RenderText_Blended(damage_font[(int)((SDL_GetTicks() - cache_damage_object->start_time)/(damage_duration/damage_frames + 1))],
                                            "Reflect!", damage_font_reflect_color);
    else if(cache_damage_object->damage_mode == damage_block)
      cache_surface = TTF_RenderText_Blended(damage_font[(int)((SDL_GetTicks() - cache_damage_object->start_time)/(damage_duration/damage_frames + 1))],
                                            "Block!", damage_font_block_color);
    else
      cache_surface = TTF_RenderText_Blended(damage_font[(int)((SDL_GetTicks() - cache_damage_object->start_time)/(damage_duration/damage_frames + 1))],
                                            cache_string, damage_font_color);
    
    /* blit current damage_list element */
    if(cache_damage_object->x + cache_surface->w/2 > camera->x &&
      cache_damage_object->x - cache_surface->w/2 < camera->x + camera->w &&
      cache_damage_object->y + cache_surface->h/2 > camera->y &&
      cache_damage_object->y - cache_surface->h/2 < camera->y + camera->h)
    {
      bar_src.x = 0;
      bar_src.y = 0;
      bar_src.w = cache_surface->w;
      bar_src.h = cache_surface->h;
      
      bar_dst.x = camera->x_movement + (cache_damage_object->x - cache_surface->w/2 - camera->x);
      bar_dst.y = camera->y_movement + (cache_damage_object->y - cache_surface->h/2 - camera->y);
      
      /* horizontal */
      if(bar_dst.x + cache_surface->w >= camera->x_movement &&
          bar_dst.x < camera->x_movement)
      {
        bar_src.x += camera->x_movement - bar_dst.x;
        bar_src.w -= camera->x_movement - bar_dst.x;
        
        bar_dst.x = camera->x_movement;
      }
      else if(bar_dst.x + cache_surface->w > camera->x_movement + camera->w &&
          bar_dst.x < camera->x_movement + camera->w)
      {
        bar_src.w -= (bar_dst.x + cache_surface->w) - (camera->x_movement + camera->w);
        
        bar_dst.x = camera->x_movement + camera->w - bar_src.w;
      }
      
      /* vertical */
      if(bar_dst.y + cache_surface->h >= camera->y_movement &&
          bar_dst.y < camera->y_movement)
      {
        bar_src.y += camera->y_movement - bar_dst.y;
        bar_src.h -= camera->y_movement - bar_dst.y;
        
        bar_dst.y = camera->y_movement;
      }
      else if(bar_dst.y + cache_surface->h > camera->y_movement + camera->h &&
          bar_dst.y < camera->y_movement + camera->h)
      {
        bar_src.h -= (bar_dst.y + cache_surface->h) - (camera->y_movement + camera->h);
        
        bar_dst.y = camera->y_movement + camera->h - bar_src.h;
      }
      
      SDL_BlitSurface(cache_surface, &bar_src, screen, &bar_dst);
    }
    
    SDL_FreeSurface(cache_surface);
    cache_damage_object_2 = cache_damage_object;
    cache_damage_object = cache_damage_object->next;
  }
}