#include <user_interface.h>

SDL_Surface *hp_bar = NULL;
SDL_Surface *mp_bar = NULL;
SDL_Surface *empty_bar = NULL;

SDL_Surface *skill_frame = NULL;

TTF_Font *interface_font = NULL;
static const SDL_Color interface_font_color = {255, 255, 255};

int load_user_interface()
{
  if((hp_bar = load_image("gui/red.png")) == NULL)
    return -1;
  if((mp_bar = load_image("gui/blue.png")) == NULL)
    return -1;
  if((empty_bar = load_image("gui/empty.png")) == NULL)
    return -1;
  
  if((skill_frame = load_image("gui/skill_frame.png")) == NULL)
    return -1;
  
  if((interface_font = TTF_OpenFont("amiri-regular.ttf", 25)) == NULL)
      return -1;
  
  return 0;
}
void free_user_interface()
{
  SDL_FreeSurface(hp_bar);
  SDL_FreeSurface(mp_bar);
  SDL_FreeSurface(empty_bar);
  
  SDL_FreeSurface(skill_frame);
  
  TTF_CloseFont(interface_font);
}

void blit_user_interface(const new_game_unit *game_unit, const new_camera *camera, SDL_Surface *screen)
{
  SDL_Rect bar_src;
  SDL_Rect bar_dst;
  
  SDL_Surface *cache_surface = NULL;
  char cache_string[1024];
  
  new_enchantment *cache_enchantment = game_unit->enchantment_list;
  new_skill *cache_skill = game_unit->skill_list;
  
  /* blit hp */
  bar_dst.x = camera->x_movement;
  bar_dst.y = camera->y_movement;
  
  bar_src.x = 0;
  bar_src.y = 0;
  bar_src.w = (Sint16)(empty_bar->w * ((float)(game_unit->hp)/(float)(game_unit->stats.max_hp)));
  bar_src.h = empty_bar->h;
  
  SDL_BlitSurface(empty_bar, NULL, screen, &bar_dst);
  
  if(game_unit->hp > 0)
    SDL_BlitSurface(hp_bar, &bar_src, screen, &bar_dst);
  bar_dst.y += empty_bar->h;
  
  /* blit mp */
  if(game_unit->base_stats.max_mp > 0)
  {
    bar_src.w = (Sint16)(empty_bar->w * ((float)(game_unit->mp)/(float)(game_unit->stats.max_mp)));
    
    SDL_BlitSurface(empty_bar, NULL, screen, &bar_dst);
    
    if(game_unit->mp > 0)
      SDL_BlitSurface(mp_bar, &bar_src, screen, &bar_dst);
    bar_dst.y += empty_bar->h;
  }
  
  /* blit enchantments, bar_dst used as icon dest, bar_src is used as interface_font dest */
  bar_dst.x = camera->x_movement;
  
  while(cache_enchantment != NULL)
  {
    if(bar_dst.x + cache_enchantment->icon->w > camera->x_movement + empty_bar->w)
    {
      bar_dst.x = camera->x_movement;
      bar_dst.y += cache_enchantment->icon->h;
    }
    
    SDL_BlitSurface(cache_enchantment->icon, NULL, screen, &bar_dst);
    
    /* blit countdown */
    if(cache_enchantment->life_time - (SDL_GetTicks() - cache_enchantment->start_time) <= 9000)
    {
      bar_src = bar_dst;
      bar_src.x += 9;
      bar_src.y -= 8;
      
      sprintf(cache_string, "%i", (int)((cache_enchantment->life_time - (SDL_GetTicks() - cache_enchantment->start_time))/1000) + 1);
      
      boxRGBA(screen, bar_dst.x, bar_dst.y, bar_dst.x + cache_enchantment->icon->w - 1,
              bar_dst.y + cache_enchantment->icon->h - 1, 0, 0, 0, 127);
      
      cache_surface = TTF_RenderText_Blended(interface_font, cache_string, interface_font_color);
      SDL_BlitSurface(cache_surface, NULL, screen, &bar_src);
      SDL_FreeSurface(cache_surface);
    }
    
    bar_dst.x += cache_enchantment->icon->w;
    cache_enchantment = cache_enchantment->next;
  }
  
  /* blit skill bar */
  bar_dst.x = camera->x_movement;
  while(cache_skill != NULL)
  {
    bar_dst.y = camera->y_movement + camera->h - cache_skill->icon->h;
    SDL_BlitSurface(cache_skill->icon, NULL, screen, &bar_dst);
    
    /* blit reuse box */
    if(SDL_GetTicks() - cache_skill->start_time < cache_skill->reuse_time)
    {
      /* reuse box */
      boxRGBA(screen, bar_dst.x, bar_dst.y, bar_dst.x + cache_skill->icon->w - 1,
              bar_dst.y + cache_skill->icon->h - (int)(((float)(SDL_GetTicks() - cache_skill->start_time)/(float)(cache_skill->reuse_time)) * cache_skill->icon->h) - 1,
              0, 0, 0, 200);
      
      /* dark box */
      boxRGBA(screen, bar_dst.x, bar_dst.y, bar_dst.x + cache_skill->icon->w - 1,
            bar_dst.y + cache_skill->icon->h - 1, 0, 0, 0, 75);
    }
    else if((game_unit->hp < cache_skill->hp_cost) || (game_unit->mp < cache_skill->mp_cost))
      boxRGBA(screen, bar_dst.x, bar_dst.y, bar_dst.x + cache_skill->icon->w - 1,
            bar_dst.y + cache_skill->icon->h - 1, 0, 0, 0, 75);
    
    SDL_BlitSurface(skill_frame, NULL, screen, &bar_dst);
    
    bar_dst.x += cache_skill->icon->w;
    cache_skill = cache_skill->next;
  }
}