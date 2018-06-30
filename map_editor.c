#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <SDL_gfxPrimitives.h>

#include <map.h>

#define interface_size 288
#define interface_bar_h 20

#define interface_menu_pos_y 0
#define interface_layer_pos_y (interface_menu_pos_y + interface_bar_h * 3)
#define interface_map_position_y (interface_layer_pos_y + interface_bar_h)

#define interface_tile_menu_pos_y (interface_map_position_y + interface_bar_h)
#define interface_tile_menu_tiles_w (Sint16)(interface_size/tile_w)
#define interface_tile_menu_tiles_h (Sint16)((screen->h - interface_tile_menu_pos_y)/tile_h)

#define interface_buffer_max_chars (Sint16)(((screen->w - interface_size)/8) - 3)
#define buffer_length 1024

#define undo_redo_steps 10

new_map undo_redo[undo_redo_steps];
unsigned short current_step;

static void load_from_map(const unsigned short step)
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
        undo_redo[step][current_layer][x][y][0] = get_tile(current_layer, x, y, 0);
        undo_redo[step][current_layer][x][y][1] = get_tile(current_layer, x, y, 1);
        
        y++;
      }
      x++;
    }
    
    current_layer++;
  }
}
static void load_to_map(const unsigned short step)
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
        write_tile(current_layer, x, y, undo_redo[step][current_layer][x][y][0] - ' ', undo_redo[step][current_layer][x][y][1] - ' ');
        y++;
      }
      x++;
    }
    
    current_layer++;
  }
}
static void check_changes()
{
  unsigned short x = 0;
  unsigned short y = 0;
  unsigned short current_layer = 0;
  unsigned short step = 0;
  
  while(current_layer < max_layer)
  {
    x = 0;
    while(x < max_tile_x)
    {
      y = 0;
      while(y < max_tile_y)
      {
        if(!(undo_redo[undo_redo_steps - 1][current_layer][x][y][0] == get_tile(current_layer, x, y, 0) && undo_redo[undo_redo_steps - 1][current_layer][x][y][0] == get_tile(current_layer, x, y, 0)))
        {
          /* if map changed, move all steps back */
          while(step < undo_redo_steps - 1)
          {
            current_layer = 0;
            while(current_layer < max_layer)
            {
              x = 0;
              while(x < max_tile_x)
              {
                y = 0;
                while(y < max_tile_y)
                {
                  undo_redo[step][current_layer][x][y][0] = undo_redo[step + 1][current_layer][x][y][0];
                  undo_redo[step][current_layer][x][y][1] = undo_redo[step + 1][current_layer][x][y][1];
                  
                  y++;
                }
                x++;
              }
              
              current_layer++;
            }
            
            step++;
          }
          
          current_step = undo_redo_steps - 1;
          load_from_map(current_step);
          return;
        }
        
        y++;
      }
      x++;
    }
    
    current_layer++;
  }
}
static void draw_collision_layer(const new_camera *camera, SDL_Surface *dest_surface)
{
  short counter_x = camera->x/tile_w;
  short counter_y = camera->y/tile_h;
  
  SDL_Rect temp;
  
  SDL_Rect tile_dest;
  tile_dest.x = 0;
  tile_dest.y = 0;
  tile_dest.w = tile_w;
  tile_dest.h = tile_h;
  
  char cache[5];
  
  while(counter_x <= (camera->x+camera->w)/tile_w)
  {
    tile_dest.y = 0;
    counter_y = camera->y/tile_h;
    
    while(counter_y <= (camera->y+camera->h)/tile_h)
    {
      if(get_tile(max_layer - 1, counter_x, counter_y, 0) != ' ')
      {
        temp = tile_dest;
        
        temp.x -= camera->x%tile_w;
        temp.y -= camera->y%tile_h;
        
        boxRGBA(dest_surface, temp.x, temp.y, temp.x + tile_w - 1, temp.y + tile_h - 1, color_dark);
        
        sprintf(cache, "%i", get_tile(max_layer - 1, counter_x, counter_y, 0) - ' ');
        stringRGBA(dest_surface, temp.x + (Sint16)(tile_w/2 - (strlen(cache) * 4)), temp.y + (Sint16)(tile_h/2 - 10), cache, color_bright);
        sprintf(cache, "%i", get_tile(max_layer - 1, counter_x, counter_y, 1) - ' ');
        stringRGBA(dest_surface, temp.x + (Sint16)(tile_w/2 - (strlen(cache) * 4)), temp.y + (Sint16)(tile_h/2 + 1), cache, color_bright);
      }
      tile_dest.y += tile_h;
      counter_y += 1;
    }
    tile_dest.x += tile_w;
    counter_x += 1;
  }
}

#ifdef __WIN32__
  int WinMain(int argc, char *argv[])
#else
  int main(int argc, char *argv[])
#endif
{
  /* variables */
  char program_run = 1;
  SDL_Event user_input;
  
  SDL_Surface *screen = NULL;
  
  Uint32 frame_timestop;
  
  SDL_Rect current_tile_src = {0, 0, tile_w, tile_h};
  SDL_Rect current_tile_dest;
  int mouse_x, mouse_y;
  
  unsigned short current_layer = 0;
  char show_layer[max_layer] = {1, 1, 1, 1, 1, 1, 0};
  
  /* draw state; 1 = draw, 2 = delete, 3 = tile_menu scrolling left/right, 4 = select from map, 5 = enter collision value */
  char draw_state = 0;
  
  char show_interface = 1;
  
  char draw_size = 1;
  SDL_Rect cache_tile_dest;
  
  char collision_cache[5];
  char collision_x = 1;
  char collision_y = 1;
  unsigned short collision_cache_x;
  unsigned short collision_cache_y;
  
  /* buffer_mode; 1 = open, 2 = save to, 3 = tileset */
  char buffer_mode = 0;
  char map_path[buffer_length] = {0};
  char buffer[buffer_length];
  char cache_buffer[buffer_length];
  unsigned short buffer_cursor_pos = 0;
  
  /* init SDL start */
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
    exit(EXIT_FAILURE);
  
  SDL_WM_SetCaption("Map Editor", NULL);
  
  if((screen = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE | SDL_RESIZABLE)) == NULL)
    exit(EXIT_FAILURE);
  
  atexit(SDL_Quit);
  SDL_EnableUNICODE(SDL_ENABLE);
  SDL_EnableKeyRepeat(200, 50);
  /* init SDL end */
  
  new_camera camera = {0, 0, 0, 0, screen->w, screen->h};
  
  /* tile_menu variables */
  SDL_Rect tile_menu_src;
  tile_menu_src.x = 0;
  tile_menu_src.y = 0;
  tile_menu_src.w = interface_tile_menu_tiles_w * tile_w;
  tile_menu_src.h = (interface_tile_menu_tiles_h + 1) * tile_h;
  
  SDL_Rect tile_menu_dest;
  tile_menu_dest.x = 0;
  tile_menu_dest.y = interface_tile_menu_pos_y + 1;
  
  clear_map();
  
  #ifndef __WIN32__
    if(argc > 0 && load_map(argv[1]) != -1)
    {
      strcpy(map_path, argv[1]);
    }
  #endif
  
  /* use buffer_cursor_pos as cache_counter */
  while(buffer_cursor_pos < undo_redo_steps)
  {
    load_from_map(buffer_cursor_pos);
    
    buffer_cursor_pos++;
  }
  
  /* main loop start */
  while(program_run)
  {
    frame_timestop = SDL_GetTicks();
    
    /* handle user input */
    while(SDL_PollEvent(&user_input))
    {
      SDL_GetMouseState(&mouse_x, &mouse_y);
      
      if(user_input.type == SDL_QUIT)
      {
        program_run = 0;
      }
      else if(user_input.type == SDL_VIDEORESIZE)
      {
        screen = SDL_SetVideoMode(user_input.resize.w, user_input.resize.h, 32, SDL_HWSURFACE | SDL_RESIZABLE);
        
        camera.x = 0;
        camera.y = 0;
        camera.w = user_input.resize.w;
        camera.h = user_input.resize.h;
        
        tile_menu_src.y = 0;
        tile_menu_src.h = (interface_tile_menu_tiles_h + 1) * tile_h;
      }
      else if(user_input.type == SDL_MOUSEBUTTONDOWN && get_tile_set() != NULL && draw_state != 4)
      {
        if(!(show_interface && mouse_x < interface_size))
            check_changes();
        
        if(user_input.button.button == SDL_BUTTON_LEFT)
        {
          draw_state = 1;
        }
        else if(user_input.button.button == SDL_BUTTON_RIGHT)
        {
          draw_state = 2;
        }
        else if(user_input.button.button == SDL_BUTTON_MIDDLE)
        {
          draw_state = 3;
        }
      }
      else if(user_input.type == SDL_MOUSEBUTTONUP)
      {
        if(draw_state < 3)
        {
          draw_state = 0;
          
          if(!(show_interface && mouse_x < interface_size))
            check_changes();
        }
        if(user_input.button.button == SDL_BUTTON_MIDDLE && draw_state != 4)
        {
          draw_state = 0;
        }
        
        /* while mouse not in interface area */
        if(!(show_interface && mouse_x < interface_size))
        {
          if(user_input.button.button == SDL_BUTTON_MIDDLE)
          {
            if(show_interface)
              show_interface = 0;
            else
              show_interface = 1;
          }
          else if(user_input.button.button == SDL_BUTTON_WHEELUP && draw_size > 1)
          {
            draw_size--;
          }
          else if(user_input.button.button == SDL_BUTTON_WHEELDOWN && draw_size < 8)
          {
            draw_size++;
          }
          
          if(user_input.button.button == SDL_BUTTON_LEFT && draw_state == 4)
          {
            current_tile_dest.x = (mouse_x + camera.x - (Sint16)(camera.x%tile_w))/tile_w;
            current_tile_dest.y = (mouse_y + camera.y - (Sint16)(camera.y%tile_h))/tile_h;
            
            if(get_tile(current_layer, current_tile_dest.x, current_tile_dest.y, 0) != ' ')
            {
              if(current_layer < max_layer - 1)
              {
                current_tile_src.x = (get_tile(current_layer, current_tile_dest.x, current_tile_dest.y, 0) - '!') * tile_w;
                current_tile_src.y = (get_tile(current_layer, current_tile_dest.x, current_tile_dest.y, 1) - '!') * tile_h;
              }
              else
              {
                collision_x = (get_tile(current_layer, current_tile_dest.x, current_tile_dest.y, 0) - ' ');
                collision_y = (get_tile(current_layer, current_tile_dest.x, current_tile_dest.y, 1) - ' ');
                
                sprintf(buffer, "%ix%i", collision_x, collision_y);
                buffer_cursor_pos = strlen(buffer);
              }
            }
            draw_state = 0;
          }
        }
        else if(draw_state != 4)
        {
          if(get_tile_set() != NULL && mouse_y > interface_tile_menu_pos_y && mouse_y < screen->h)
          {
            if(draw_state == 3)
            {
              if(user_input.button.button == SDL_BUTTON_WHEELUP && tile_menu_src.x > 0)
              {
                tile_menu_src.x -= tile_w;
              }
              else if(user_input.button.button == SDL_BUTTON_WHEELDOWN && tile_menu_src.x < get_tile_set()->w - interface_tile_menu_tiles_w * tile_h)
              {
                tile_menu_src.x += tile_w;
              }
            }
            else
            {
              if(user_input.button.button == SDL_BUTTON_WHEELUP && tile_menu_src.y > 0)
              {
                tile_menu_src.y -= tile_h;
              }
              else if(user_input.button.button == SDL_BUTTON_WHEELDOWN && tile_menu_src.y < get_tile_set()->h - interface_tile_menu_tiles_h * tile_h)
              {
                tile_menu_src.y += tile_h;
              }
            }
          }
          
          if(user_input.button.button == SDL_BUTTON_LEFT)
          {
            /* press clear */
            if(mouse_x > 0 && mouse_x < (Sint16)(interface_size/3) && mouse_y > interface_menu_pos_y && mouse_y < interface_menu_pos_y + interface_bar_h)
            {
              clear_map();
              
              check_changes();
            }
            
            /* press open */
            if(mouse_x > (Sint16)(interface_size/3) && mouse_x < (Sint16)(interface_size/3) * 2 && mouse_y > interface_menu_pos_y && mouse_y < interface_menu_pos_y + interface_bar_h)
            {
              strcpy(buffer, map_path);
              buffer_cursor_pos = strlen(buffer);
              buffer_mode = 1;
            }
            
            /* press load */
            if(mouse_x > (Sint16)(interface_size/3) * 2 && mouse_x < interface_size && mouse_y > interface_menu_pos_y && mouse_y < interface_menu_pos_y + interface_bar_h)
            {
              load_map(map_path);
              
              check_changes();
            }
            
            /* press tileset */
            if(mouse_x > 0 && mouse_x < (Sint16)(interface_size/3) && mouse_y > interface_menu_pos_y + interface_bar_h && mouse_y < interface_menu_pos_y + interface_bar_h * 2)
            {
              strcpy(buffer, get_tile_set_path());
              buffer_cursor_pos = strlen(buffer);
              buffer_mode = 3;
            }
            
            /* press save to */
            if(mouse_x > (Sint16)(interface_size/3) && mouse_x < (Sint16)(interface_size/3) * 2 && mouse_y > interface_menu_pos_y + interface_bar_h && mouse_y < interface_menu_pos_y + interface_bar_h * 2)
            {
              strcpy(buffer, map_path);
              buffer_cursor_pos = strlen(buffer);
              buffer_mode = 2;
            }
            
            /* press save */
            if(mouse_x > (Sint16)(interface_size/3) * 2 && mouse_x < interface_size && mouse_y > interface_menu_pos_y + interface_bar_h && mouse_y < interface_menu_pos_y + interface_bar_h * 2)
            {
              save_map(map_path);
            }
            
            /* press undo */
            if(mouse_x > 0 && mouse_x < (Sint16)(interface_size/3) && mouse_y > interface_menu_pos_y + interface_bar_h * 2 && mouse_y < interface_menu_pos_y + interface_bar_h * 3)
            {
              if(current_step > 0)
              {
                current_step--;
                load_to_map(current_step);
              }
            }
            
            /* press redo */
            if(mouse_x > (Sint16)(interface_size/3) && mouse_x < (Sint16)(interface_size/3) * 2 && mouse_y > interface_menu_pos_y + interface_bar_h * 2 && mouse_y < interface_menu_pos_y + interface_bar_h * 3)
            {
              if(current_step < undo_redo_steps - 1)
              {
                current_step++;
                load_to_map(current_step);
              }
            }
            
            /* press select */
            if(mouse_x > (Sint16)(interface_size/3) * 2 && mouse_x < interface_size && mouse_y > interface_menu_pos_y + interface_bar_h * 2 && mouse_y < interface_menu_pos_y + interface_bar_h * 3)
            {
              draw_state = 4;
            }
            
            /* change current_layer start */
            if(mouse_x > 0 && mouse_x < 23 && mouse_y > interface_layer_pos_y && mouse_y < interface_layer_pos_y + interface_bar_h)
              current_layer = 0;
            else if(mouse_x > 24 && mouse_x < 47 && mouse_y > interface_layer_pos_y && mouse_y < interface_layer_pos_y + interface_bar_h)
              current_layer = 1;
            else if(mouse_x > 48 && mouse_x < 71 && mouse_y > interface_layer_pos_y && mouse_y < interface_layer_pos_y + interface_bar_h)
              current_layer = 2;
            else if(mouse_x > 72 && mouse_x < 95 && mouse_y > interface_layer_pos_y && mouse_y < interface_layer_pos_y + interface_bar_h)
              current_layer = 3;
            else if(mouse_x > 96 && mouse_x < 119 && mouse_y > interface_layer_pos_y && mouse_y < interface_layer_pos_y + interface_bar_h)
              current_layer = 4;
            else if(mouse_x > 120 && mouse_x < 143 && mouse_y > interface_layer_pos_y && mouse_y < interface_layer_pos_y + interface_bar_h)
              current_layer = 5;
            else if(mouse_x > 144 && mouse_x < 167 && mouse_y > interface_layer_pos_y && mouse_y < interface_layer_pos_y + interface_bar_h)
            {
              current_layer = 6;
              show_layer[6] = 1;
              
              buffer_mode = 4;
              sprintf(buffer, "%ix%i", collision_x, collision_y);
              buffer_cursor_pos = strlen(buffer);
            }
            /* change current_layer end */
            
            /* choose tile from tile_menu */
            if(show_interface && mouse_x < interface_size && mouse_y > interface_tile_menu_pos_y && mouse_y < interface_tile_menu_pos_y + interface_tile_menu_tiles_h * tile_h)
            {
              current_tile_dest.x = (Sint16)(mouse_x/tile_w) * tile_w;
              current_tile_dest.y = (Sint16)((mouse_y - interface_tile_menu_pos_y)/tile_h) * tile_h;
              
              current_tile_src.x = current_tile_dest.x + tile_menu_src.x;
              current_tile_src.y = current_tile_dest.y + tile_menu_src.y;
            }
          }
          
          /* toggle show_layer start */
          if(user_input.button.button == SDL_BUTTON_RIGHT)
          {
            if(mouse_x > 0 && mouse_x < 23 && mouse_y > interface_layer_pos_y && mouse_y < interface_layer_pos_y + interface_bar_h)
            {
              if(show_layer[0])
                show_layer[0] = 0;
              else
                show_layer[0] = 1;
            }
            else if(mouse_x > 24 && mouse_x < 47 && mouse_y > interface_layer_pos_y && mouse_y < interface_layer_pos_y + interface_bar_h)
            {
              if(show_layer[1])
                show_layer[1] = 0;
              else
                show_layer[1] = 1;
            }
            else if(mouse_x > 48 && mouse_x < 71 && mouse_y > interface_layer_pos_y && mouse_y < interface_layer_pos_y + interface_bar_h)
            {
              if(show_layer[2])
                show_layer[2] = 0;
              else
                show_layer[2] = 1;
            }
            else if(mouse_x > 72 && mouse_x < 95 && mouse_y > interface_layer_pos_y && mouse_y < interface_layer_pos_y + interface_bar_h)
            {
              if(show_layer[3])
                show_layer[3] = 0;
              else
                show_layer[3] = 1;
            }
            else if(mouse_x > 96 && mouse_x < 119 && mouse_y > interface_layer_pos_y && mouse_y < interface_layer_pos_y + interface_bar_h)
            {
              if(show_layer[4])
                show_layer[4] = 0;
              else
                show_layer[4] = 1;
            }
            else if(mouse_x > 120 && mouse_x < 143 && mouse_y > interface_layer_pos_y && mouse_y < interface_layer_pos_y + interface_bar_h)
            {
              if(show_layer[5])
                show_layer[5] = 0;
              else
                show_layer[5] = 1;
            }
            else if(mouse_x > 144 && mouse_x < 167 && mouse_y > interface_layer_pos_y && mouse_y < interface_layer_pos_y + interface_bar_h)
            {
              if(show_layer[6])
                show_layer[6] = 0;
              else
                show_layer[6] = 1;
            }
          }
          /* toggle show_layer end */
        }
      }
      else if(user_input.type == SDL_KEYDOWN)
      {
        if(user_input.key.keysym.sym == SDLK_RETURN)
        {
          if(buffer_mode == 1)
          {
            if(load_map(buffer) != -1)
            {
              strcpy(map_path, buffer);
              check_changes();
                            
              tile_menu_src.x = 0;
              tile_menu_src.y = 0;
              
              buffer_mode = 0;
            }
          }
          else if(buffer_mode == 2)
          {
            if(save_map(buffer) != -1)
            {
              strcpy(map_path, buffer);
              
              buffer_mode = 0;
            }
          }
          else if(buffer_mode == 3)
          {
            if(load_tile_set(buffer) != -1)
            {
              tile_menu_src.x = 0;
              tile_menu_src.y = 0;
              
              buffer_mode = 0;
            }
          }
          else if(buffer_mode == 4)
          {
            if(strchr(buffer, 'x') != NULL)
            {
              strcpy(cache_buffer, buffer);
              
              collision_cache_y = atoi(strchr(cache_buffer, 'x') + 1);
              
              cache_buffer[strlen(cache_buffer) - strlen(strchr(cache_buffer, 'x'))] = '\0';
              collision_cache_x = atoi(cache_buffer);
              
              if(collision_cache_x > 0 && collision_cache_x < ('~' - '!') && collision_cache_y > 0 && collision_cache_y < ('~' - '!'))
              {
                collision_x = collision_cache_x;
                collision_y = collision_cache_y;
              }
            }
          }
        }
        else if(user_input.key.keysym.sym == SDLK_BACKSPACE)
        {
          if(buffer_cursor_pos > 0)
          {
            strcpy(cache_buffer, buffer);
            
            buffer[buffer_cursor_pos - 1] = '\0';
            
            strcat(buffer, &cache_buffer[buffer_cursor_pos]);
            
            buffer_cursor_pos--;
          }
            
        }
        else if(user_input.key.keysym.sym == SDLK_ESCAPE)
        {
          buffer_mode = 0;
        }
        else if(user_input.key.keysym.sym == SDLK_LEFT && buffer_cursor_pos > 0)
        {
          buffer_cursor_pos--;
        }
        else if(user_input.key.keysym.sym == SDLK_RIGHT && buffer_cursor_pos < strlen(buffer))
        {
          buffer_cursor_pos++;
        }
        else if(buffer_mode != 0)
        {
          if(strlen(buffer) < buffer_length - 1)
          {
            /* append only readable chars */
            if(user_input.key.keysym.unicode >= ' ' && user_input.key.keysym.unicode <= '~')
            {
              strcpy(cache_buffer, buffer);
              
              buffer[buffer_cursor_pos] = user_input.key.keysym.unicode;
              buffer[buffer_cursor_pos + 1] = '\0';
              
              strcat(buffer, &cache_buffer[buffer_cursor_pos]);
              buffer_cursor_pos++;
            }
          }
        }
      }
    }
    
    /* draw map */
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    
    if(show_layer[0])
      blit_layer(0, &camera, screen);
    if(show_layer[1])
      blit_layer(1, &camera, screen);
    if(show_layer[2])
      blit_layer(2, &camera, screen);
    if(show_layer[3])
      blit_layer(3, &camera, screen);
    if(show_layer[4])
      blit_layer(4, &camera, screen);
    if(show_layer[5])
      blit_layer(5, &camera, screen);
    if(show_layer[6])
      draw_collision_layer(&camera, screen);
    
    /* draw interface */
    if(show_interface)
    {
      boxRGBA(screen, 0, 0, interface_size, screen->h, color_dark);
      
      /* draw menu bar start */
      stringRGBA(screen, 0, interface_menu_pos_y + (Sint16)(interface_bar_h/2 - 5), "    Clear       Open        Load", color_bright);
      stringRGBA(screen, 0, interface_menu_pos_y + (Sint16)(interface_bar_h/2 - 5) + interface_bar_h, "   Tileset     Save to      Save", color_bright);
      stringRGBA(screen, 0, interface_menu_pos_y + (Sint16)(interface_bar_h/2 - 5) + interface_bar_h * 2, "    Undo        Redo       Select", color_bright);
      
      lineRGBA(screen, 0, interface_menu_pos_y + interface_bar_h, interface_size, interface_menu_pos_y + interface_bar_h, color_bright);
      lineRGBA(screen, 0, interface_menu_pos_y + interface_bar_h * 2, interface_size, interface_menu_pos_y + interface_bar_h * 2, color_bright);
      lineRGBA(screen, 0, interface_menu_pos_y + interface_bar_h * 3, interface_size, interface_menu_pos_y + interface_bar_h * 3, color_bright);
      
      lineRGBA(screen, (Sint16)(interface_size/3), interface_menu_pos_y, (Sint16)(interface_size/3), interface_menu_pos_y + interface_bar_h * 3, color_bright);
      lineRGBA(screen, (Sint16)(interface_size/3 * 2), interface_menu_pos_y, (Sint16)(interface_size/3 * 2), interface_menu_pos_y + interface_bar_h * 3, color_bright);
      
      /* highlight buttons */
      /* highlight button 1 */
      if(mouse_x > 0 && mouse_x < (Sint16)(interface_size/3) && mouse_y > interface_menu_pos_y && mouse_y < interface_menu_pos_y + interface_bar_h)
        boxRGBA(screen, 0, interface_menu_pos_y, (Sint16)(interface_size/3), interface_menu_pos_y + interface_bar_h, color_bright);
      
      /* highlight button 2 */
      if(mouse_x > (Sint16)(interface_size/3) && mouse_x < (Sint16)(interface_size/3) * 2 && mouse_y > interface_menu_pos_y && mouse_y < interface_menu_pos_y + interface_bar_h)
        boxRGBA(screen, (Sint16)(interface_size/3), interface_menu_pos_y, (Sint16)(interface_size/3) * 2, interface_menu_pos_y + interface_bar_h, color_bright);
      
      /* highlight button 3 */
      if(mouse_x > (Sint16)(interface_size/3) * 2 && mouse_x < interface_size && mouse_y > interface_menu_pos_y && mouse_y < interface_menu_pos_y + interface_bar_h)
        boxRGBA(screen, (Sint16)(interface_size/3) * 2, interface_menu_pos_y, interface_size, interface_menu_pos_y + interface_bar_h, color_bright);
      
      /* highlight button 4 */
      if(mouse_x > 0 && mouse_x < (Sint16)(interface_size/3) && mouse_y > interface_menu_pos_y + interface_bar_h && mouse_y < interface_menu_pos_y + interface_bar_h * 2)
        boxRGBA(screen, 0, interface_menu_pos_y + interface_bar_h, (Sint16)(interface_size/3), interface_menu_pos_y + interface_bar_h * 2, color_bright);
      
      /* highlight button 5 */
      if(mouse_x > (Sint16)(interface_size/3) && mouse_x < (Sint16)(interface_size/3) * 2 && mouse_y > interface_menu_pos_y + interface_bar_h && mouse_y < interface_menu_pos_y + interface_bar_h * 2)
        boxRGBA(screen, (Sint16)(interface_size/3), interface_menu_pos_y + interface_bar_h, (Sint16)(interface_size/3) * 2, interface_menu_pos_y + interface_bar_h * 2, color_bright);
      
      /* highlight button 6 */
      if(mouse_x > (Sint16)(interface_size/3) * 2 && mouse_x < interface_size && mouse_y > interface_menu_pos_y + interface_bar_h && mouse_y < interface_menu_pos_y + interface_bar_h * 2)
        boxRGBA(screen, (Sint16)(interface_size/3) * 2, interface_menu_pos_y + interface_bar_h, interface_size, interface_menu_pos_y + interface_bar_h * 2, color_bright);
      
      /* highlight button 7 */
      if(mouse_x > 0 && mouse_x < (Sint16)(interface_size/3) && mouse_y > interface_menu_pos_y + interface_bar_h * 2 && mouse_y < interface_menu_pos_y + interface_bar_h * 3)
        boxRGBA(screen, 0, interface_menu_pos_y + interface_bar_h * 2, (Sint16)(interface_size/3), interface_menu_pos_y + interface_bar_h * 3, color_bright);
      
      /* highlight button 8 */
      if(mouse_x > (Sint16)(interface_size/3) && mouse_x < (Sint16)(interface_size/3) * 2 && mouse_y > interface_menu_pos_y + interface_bar_h * 2 && mouse_y < interface_menu_pos_y + interface_bar_h * 3)
        boxRGBA(screen, (Sint16)(interface_size/3), interface_menu_pos_y + interface_bar_h * 2, (Sint16)(interface_size/3) * 2, interface_menu_pos_y + interface_bar_h * 3, color_bright);
      
      /* highlight button 9 */
      if(mouse_x > (Sint16)(interface_size/3) * 2 && mouse_x < interface_size && mouse_y > interface_menu_pos_y + interface_bar_h * 2 && mouse_y < interface_menu_pos_y + interface_bar_h * 3)
        boxRGBA(screen, (Sint16)(interface_size/3) * 2, interface_menu_pos_y + interface_bar_h * 2, interface_size, interface_menu_pos_y + interface_bar_h * 3, color_bright);
      
      /* highlight buttons */
      /* draw menu bar end */
      
      /* draw layer bar start */
      if(show_layer[0])
        boxRGBA(screen, 0, interface_layer_pos_y + 1, 23, interface_layer_pos_y + interface_bar_h, color_bright/2);
      if(show_layer[1])
        boxRGBA(screen, 24, interface_layer_pos_y + 1, 47, interface_layer_pos_y + interface_bar_h, color_bright/2);
      if(show_layer[2])
        boxRGBA(screen, 48, interface_layer_pos_y + 1, 71, interface_layer_pos_y + interface_bar_h, color_bright/2);
      if(show_layer[3])
        boxRGBA(screen, 72, interface_layer_pos_y + 1, 95, interface_layer_pos_y + interface_bar_h, color_bright/2);
      if(show_layer[4])
        boxRGBA(screen, 96, interface_layer_pos_y + 1, 119, interface_layer_pos_y + interface_bar_h, color_bright/2);
      if(show_layer[5])
        boxRGBA(screen, 120, interface_layer_pos_y + 1, 143, interface_layer_pos_y + interface_bar_h, color_bright/2);
      if(show_layer[6])
        boxRGBA(screen, 144, interface_layer_pos_y + 1, 167, interface_layer_pos_y + interface_bar_h, color_bright/2);
      
      switch(current_layer)
      {
        case 0:
          boxRGBA(screen, 0, interface_layer_pos_y + 1, 23, interface_layer_pos_y + interface_bar_h, color_bright);
          break;
        case 1:
          boxRGBA(screen, 24, interface_layer_pos_y + 1, 47, interface_layer_pos_y + interface_bar_h, color_bright);
          break;
        case 2:
          boxRGBA(screen, 48, interface_layer_pos_y + 1, 71, interface_layer_pos_y + interface_bar_h, color_bright);
          break;
        case 3:
          boxRGBA(screen, 72, interface_layer_pos_y + 1, 95, interface_layer_pos_y + interface_bar_h, color_bright);
          break;
        case 4:
          boxRGBA(screen, 96, interface_layer_pos_y + 1, 119, interface_layer_pos_y + interface_bar_h, color_bright);
          break;
        case 5:
          boxRGBA(screen, 120, interface_layer_pos_y + 1, 143, interface_layer_pos_y + interface_bar_h, color_bright);
          break;
        case 6:
          boxRGBA(screen, 144, interface_layer_pos_y + 1, 167, interface_layer_pos_y + interface_bar_h, color_bright);
          break;
      }
      
      stringRGBA(screen, 0, interface_layer_pos_y + (Sint16)(interface_bar_h/2 - 5), " 1  2  3  4  5  6  7", color_bright);
      
      lineRGBA(screen, 0, interface_layer_pos_y + interface_bar_h, interface_size, interface_layer_pos_y + interface_bar_h, color_bright);
      /* draw layer box end */
      
      /* draw menu_position bar */
      sprintf(cache_buffer, "Position: %03i/%03i", (mouse_x + camera.x - (Sint16)(camera.x%tile_w))/tile_w, (mouse_y + camera.y - (Sint16)(camera.y%tile_h))/tile_h);
      stringRGBA(screen, 0, interface_map_position_y + (Sint16)(interface_bar_h/2 - 5), cache_buffer, color_bright);
      lineRGBA(screen, 0, interface_map_position_y + interface_bar_h, interface_size, interface_map_position_y + interface_bar_h, color_bright);
      
      /* draw tile menu */
      SDL_BlitSurface(get_tile_set(), &tile_menu_src, screen, &tile_menu_dest);
    }
    
    /* while mouse not in interface area start */
    if(!(show_interface && mouse_x < interface_size))
    {
      SDL_ShowCursor(SDL_DISABLE);
      
      /* move camera */
      if(mouse_x <= tile_w && camera.x > 0)
      {
        camera.x -= 5;
      }
      else if(mouse_x >= screen->w - tile_w && camera.x + camera.w < max_tile_x * tile_w)
      {
        camera.x += 5;
      }
      else if(mouse_y <= tile_h && camera.y > 0)
      {
        camera.y -= 5;
      }
      else if(mouse_y >= screen->h - tile_h && camera.y + camera.h < max_tile_y * tile_h)
      {
        camera.y += 5;
      }
      
      /* draw in layer */
      current_tile_dest.x = (mouse_x + camera.x - (Sint16)(camera.x%tile_w))/tile_w;
      current_tile_dest.y = (mouse_y + camera.y - (Sint16)(camera.y%tile_h))/tile_h;
      cache_tile_dest = current_tile_dest;
      
      if(draw_state == 1)
      {
        if(current_layer < max_layer - 1)
        {
          while(cache_tile_dest.y < current_tile_dest.y + draw_size)
          {
            cache_tile_dest.x = current_tile_dest.x;
            while(cache_tile_dest.x < current_tile_dest.x + draw_size)
            {
              write_tile(current_layer, cache_tile_dest.x, cache_tile_dest.y, current_tile_src.x/tile_w + 1, current_tile_src.y/tile_h + 1);
              cache_tile_dest.x++;
            }
            cache_tile_dest.y++;
          }
        }
        else
        {
          while(cache_tile_dest.y < current_tile_dest.y + draw_size)
          {
            cache_tile_dest.x = current_tile_dest.x;
            while(cache_tile_dest.x < current_tile_dest.x + draw_size)
            {
              write_tile(current_layer, cache_tile_dest.x, cache_tile_dest.y, collision_x, collision_y);
              cache_tile_dest.x++;
            }
            cache_tile_dest.y++;
          }
        }
      }
      else if(draw_state == 2)
      {
        while(cache_tile_dest.y < current_tile_dest.y + draw_size)
        {
          cache_tile_dest.x = current_tile_dest.x;
          while(cache_tile_dest.x < current_tile_dest.x + draw_size)
          {
            write_tile(current_layer, cache_tile_dest.x, cache_tile_dest.y, 0, 0);
            cache_tile_dest.x++;
          }
          cache_tile_dest.y++;
        }
      }
      
      /* draw collision or tiled cursor*/
      current_tile_dest.x = (Sint16)(mouse_x/tile_w) * tile_w - (Sint16)(camera.x%tile_w);
      current_tile_dest.y = (Sint16)(mouse_y/tile_h) * tile_h - (Sint16)(camera.y%tile_h);
      cache_tile_dest = current_tile_dest;
      
      if(current_layer == max_layer - 1 && draw_state != 4)
      {
        while(cache_tile_dest.y + tile_h <= current_tile_dest.y + tile_h * draw_size)
        {
          cache_tile_dest.x = current_tile_dest.x;
          while(cache_tile_dest.x + tile_w <= current_tile_dest.x + tile_w * draw_size)
          {
            boxRGBA(screen, cache_tile_dest.x, cache_tile_dest.y, cache_tile_dest.x + tile_w - 1, cache_tile_dest.y + tile_h - 1, color_dark);
            
            sprintf(collision_cache, "%i", collision_x);
            stringRGBA(screen, cache_tile_dest.x + (Sint16)(tile_w/2 - (strlen(collision_cache) * 4)), cache_tile_dest.y + (Sint16)(tile_h/2 - 10), collision_cache, color_bright);
            sprintf(collision_cache, "%i", collision_y);
            stringRGBA(screen, cache_tile_dest.x + (Sint16)(tile_w/2 - (strlen(collision_cache) * 4)), cache_tile_dest.y + (Sint16)(tile_h/2 + 1), collision_cache, color_bright);
            
            cache_tile_dest.x += tile_w;
          }
          cache_tile_dest.y += tile_h;
        }
      }
      else if(draw_state != 4)
      {
        while(cache_tile_dest.y + tile_h <= current_tile_dest.y + tile_h * draw_size)
        {
          cache_tile_dest.x = current_tile_dest.x;
          while(cache_tile_dest.x + tile_w <= current_tile_dest.x + tile_w * draw_size)
          {
            SDL_BlitSurface(get_tile_set(), &current_tile_src, screen, &cache_tile_dest);
            cache_tile_dest.x += tile_w;
          }
          cache_tile_dest.y += tile_h;
        }
      }
      
      current_tile_dest.x = (Sint16)(mouse_x/tile_w) * tile_w - (Sint16)(camera.x%tile_w);
      current_tile_dest.y = (Sint16)(mouse_y/tile_h) * tile_h - (Sint16)(camera.y%tile_h);
      if(draw_state == 4)
      {
        boxRGBA(screen, current_tile_dest.x, current_tile_dest.y, current_tile_dest.x + tile_w - 1, current_tile_dest.y + tile_h, color_dark);
        rectangleRGBA(screen, current_tile_dest.x, current_tile_dest.y, current_tile_dest.x + tile_w - 1, current_tile_dest.y + tile_h, color_bright);
      }
      else
        rectangleRGBA(screen, current_tile_dest.x, current_tile_dest.y, current_tile_dest.x + tile_w * draw_size - 1, current_tile_dest.y + tile_h * draw_size - 1, color_dark);
    }
    else
    {
      SDL_ShowCursor(SDL_ENABLE);
      
      if(mouse_y > interface_tile_menu_pos_y && mouse_y < interface_tile_menu_pos_y + interface_tile_menu_tiles_h * tile_h)
      {
        current_tile_dest.x = (Sint16)(mouse_x/tile_w) * tile_w;
        current_tile_dest.y = (Sint16)((mouse_y - interface_tile_menu_pos_y)/tile_h) * tile_h + interface_tile_menu_pos_y;
        boxRGBA(screen, current_tile_dest.x, current_tile_dest.y, current_tile_dest.x + tile_w - 1, current_tile_dest.y + tile_h, color_dark);
        rectangleRGBA(screen, current_tile_dest.x, current_tile_dest.y, current_tile_dest.x + tile_w - 1, current_tile_dest.y + tile_h, color_bright);
      }
    }
    /* while mouse not in interface area end */
    
    /* draw buffer + message */
    if(buffer_mode != 0)
    {
      /* draw message */
      if(buffer_mode == 1)
        strcpy(cache_buffer, "Open Map:");
      else if(buffer_mode == 2)
        strcpy(cache_buffer, "Save Map to:");
      else if(buffer_mode == 3)
        strcpy(cache_buffer, "Open Tileset:");
      else if(buffer_mode == 4)
        strcpy(cache_buffer, "Enter Collision Value:");
      
      boxRGBA(screen, interface_size + 1, screen->h - 26, interface_size + strlen(cache_buffer) * 8 + 1, screen->h - 14, color_dark);
      stringRGBA(screen, interface_size + 2, screen->h - 24, cache_buffer, color_bright);
      
      /* draw buffer */
      boxRGBA(screen, interface_size + 1, screen->h - 13, screen->w - 1, screen->h - 1, color_dark);
      rectangleRGBA(screen, interface_size + 1, screen->h - 13, screen->w - 1, screen->h - 1, color_bright);
      
      if(buffer_cursor_pos > interface_buffer_max_chars)
      {
        strcpy(cache_buffer, &buffer[buffer_cursor_pos - interface_buffer_max_chars]);
        lineRGBA(screen, interface_size + interface_buffer_max_chars * 8 + 3, screen->h - 11, interface_size + interface_buffer_max_chars * 8 + 3, screen->h - 3, color_bright);
      }
      else
      {
        strcpy(cache_buffer, buffer);
        lineRGBA(screen, interface_size + buffer_cursor_pos * 8 + 3, screen->h - 11, interface_size + buffer_cursor_pos * 8 + 3, screen->h - 3, color_bright);
      }
      
      stringRGBA(screen, interface_size + 3, screen->h - 11, cache_buffer, color_bright);
    }
    
    SDL_Flip(screen);
    
    /* keep framerate */
    if(15 > (SDL_GetTicks() - frame_timestop))
      SDL_Delay(15 - (SDL_GetTicks() - frame_timestop));
  }
  /* main loop end */
  
  SDL_FreeSurface(get_tile_set());
  return EXIT_SUCCESS;
}