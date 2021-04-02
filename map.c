#include <map.h>

/* 7 layers; 3 lower layers, 3 upper and 1 colision layer */
/* collision layer uses only [0], but [1] is loaded/saved too */
new_map map;
SDL_Surface *tile_set = NULL;
char tile_set_path[1024] = {0};

char *get_tile_set_path()
{
  return tile_set_path;
}
SDL_Surface *get_tile_set()
{
  return tile_set;
}
int load_tile_set(const char *tile_path)
{
  SDL_Surface *temp = NULL;
  
  if(strcmp(tile_set_path, tile_path) != 0)
  {
    if((temp = IMG_Load(tile_path)) == NULL)
    {
      fprintf(stderr, "Error: Failed to load tileset file: %s\n", tile_path);
      return -1;
    }
    
    if(tile_set != NULL)
      SDL_FreeSurface(tile_set);
    
    tile_set = SDL_DisplayFormatAlpha(temp);
    SDL_FreeSurface(temp);
    
    strcpy(tile_set_path, tile_path);
  }
  
  return 0;
}

int load_map(const char *map_path)
{
  FILE *reader = NULL;
  char tile_set_path_cache[1024];
  
  unsigned short x = 0;
  unsigned short y = 0;
  unsigned short current_layer = 0;
  
  if(!((reader = fopen(map_path, "r")) != NULL && fgets(tile_set_path_cache, 1024, reader)))
  {
    return -1;
  }
  
  /* remove '\n' or '\r\n' sign in tile_set_path_chache */
  if(tile_set_path_cache[strlen(tile_set_path_cache) - 2] == '\r')
    tile_set_path_cache[strlen(tile_set_path_cache) - 2] = '\0';
  else
    tile_set_path_cache[strlen(tile_set_path_cache) - 1] = '\0';
  
  if(load_tile_set(tile_set_path_cache) == -1)
    return -1;
  
  while((map[current_layer][x][y][0] = fgetc(reader)) && (map[current_layer][x][y][1] = fgetc(reader)) && current_layer < max_layer)
  {
    /* x and y are swapped, vertical(y) lines are readed first */
    y++;
    if(y >= max_tile_y)
    {
      y = 0;
      
      x++;
      if(x >= max_tile_x)
      {
        x = 0;
        current_layer++;
      }
    }
  }
  
  fclose(reader);
  return 0;
}
int save_map(const char *map_path)
{
  FILE *writer = NULL;
  unsigned short x = 0;
  unsigned short y = 0;
  unsigned short current_layer = 0;
  
  if(!((writer = fopen(map_path, "w")) != NULL && tile_set_path[0] != '\0'))
  {
    return -1;
  }
  
  fprintf(writer, "%s\n",tile_set_path);
  
  while(current_layer < max_layer)
  {
    x = 0;
    while(x < max_tile_x)
    {
      y = 0;
      while(y < max_tile_y)
      {
        fputc(map[current_layer][x][y][0], writer);
        fputc(map[current_layer][x][y][1], writer);
        
        y++;
      }
      x++;
    }
    
    current_layer++;
  }
  
  fclose(writer);
  return 0;
}

void clear_map()
{
  unsigned short x = 0;
  unsigned short y = 0;
  unsigned short current_layer = 0;
  
  while(current_layer < max_layer)
  {
    x = 0;
    while(x < max_tile_x)
    {
      y = 0;
      while(y < max_tile_y)
      {
        map[current_layer][x][y][0] = ' ';
        map[current_layer][x][y][1] = ' ';
        
        y++;
      }
      x++;
    }
    
    current_layer++;
  }
}

void blit_layer(const unsigned short current_layer, const new_camera *camera, SDL_Surface *dest_surface)
{
  if(!(current_layer < max_layer && current_layer >= 0))
  {
    return;
  }
  
  short counter_x = camera->x/tile_w;
  short counter_y = camera->y/tile_h;
  
  SDL_Rect temp;
  
  SDL_Rect tile_source;
  tile_source.x = 0;
  tile_source.y = 0;
  
  SDL_Rect tile_dest;
  tile_dest = tile_source;
  tile_dest.x = camera->x_movement;
  
  while(counter_x <= (camera->x+camera->w)/tile_w)
  {
    tile_dest.y = camera->y_movement;
    counter_y = camera->y/tile_h;
    
    while(counter_y <= (camera->y+camera->h)/tile_h)
    {
      if(map[current_layer][counter_x][counter_y][0] != ' ')
      {
        /* set and reset tile_source */
        tile_source.x = (map[current_layer][counter_x][counter_y][0] - '!') * tile_w;
        tile_source.y = (map[current_layer][counter_x][counter_y][1] - '!') * tile_h;
        tile_source.w = tile_w;
        tile_source.h = tile_h;
        
        temp = tile_dest;
        
        /* ensure smooth scrolling */
        temp.x -= camera->x%tile_w;
        temp.y -= camera->y%tile_h;
        
        /* 'cut' off area, which is not in camera range */
        if(temp.x < camera->x_movement)
        {
          temp.x += camera->x%tile_w;
          
          tile_source.x += camera->x%tile_w;
          tile_source.w = tile_w - camera->x%tile_w;
        }
        else if(temp.x + tile_w > camera->x_movement + camera->w)
        {
          tile_source.w = tile_w - ((temp.x + tile_w) - (camera->x_movement + camera->w));
        }
        
        if(temp.y < camera->y_movement)
        {
          temp.y += camera->y%tile_h;
          
          tile_source.y += camera->y%tile_h;
          tile_source.h = tile_h - camera->y%tile_h;
        }
        else if(temp.y + tile_h > camera->y_movement + camera->h)
        {
          tile_source.h = tile_h - ((temp.y + tile_h) - (camera->y_movement + camera->h));
        }
        
        SDL_BlitSurface(tile_set, &tile_source, dest_surface, &temp);
      }
      tile_dest.y += tile_h;
      counter_y += 1;
    }
    tile_dest.x += tile_w;
    counter_x += 1;
  }
}

void write_tile(const unsigned short current_layer, const unsigned short x, const unsigned short y, const unsigned short current_tile_x, const unsigned short current_tile_y)
{
  if(current_layer >= 0 && current_layer < max_layer && x < max_tile_x && x >= 0 && y < max_tile_y && y >= 0)
  {
    map[current_layer][x][y][0] = current_tile_x + ' ';
    map[current_layer][x][y][1] = current_tile_y + ' ';
  }
}
unsigned short get_tile(const unsigned short current_layer, const unsigned short x, const unsigned short y, const unsigned short tile_x_y)
{
  return map[current_layer][x][y][tile_x_y];
}