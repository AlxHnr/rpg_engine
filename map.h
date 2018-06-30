#ifndef MAP_H
#define MAP_H

#include <SDL.h>
#include <SDL_image.h>

#define max_layer 7

#define max_tile_x 50
#define max_tile_y 50

#define tile_w 32
#define tile_h 32

#define color_dark 50, 50, 50, 200
#define color_bright 200, 200, 200, 200

typedef unsigned short new_map[max_layer][max_tile_x][max_tile_y][2];

typedef struct{
  Sint16 x;
  Sint16 y;
  
  Sint16 x_movement;
  Sint16 y_movement;
  Sint16 w;
  Sint16 h;
}new_camera;

char *get_tile_set_path();
SDL_Surface *get_tile_set();
int load_tile_set(const char *tile_path);

int load_map(const char *map_path);
int save_map(const char *map_path);

void clear_map();

void blit_layer(const unsigned short current_layer, const new_camera *camera, SDL_Surface *dest_surface);

void write_tile(const unsigned short current_layer, const unsigned short x, const unsigned short y, const unsigned short current_tile_x, const unsigned short current_tile_y);
unsigned short get_tile(const unsigned short current_layer, const unsigned short x, const unsigned short y, const unsigned short tile_x_y);

#endif