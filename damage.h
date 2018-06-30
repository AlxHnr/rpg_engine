#ifndef DAMAGE_H
#define DAMAGE_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <map.h>

#define damage_normal 0
#define damage_success 1
#define damage_reflect 2
#define damage_block 3

#define damage_duration 500
#define damage_frames 25

typedef struct damage_object{
  double x;
  double y;
  
  int damage;
  int damage_mode;
  Uint32 time_stop;
  Uint32 start_time;
  
  struct damage_object *next;
}damage_object;

int load_damage();
void free_damage();

void append_damage(const double x, const double y, const int damage, const int mode);
void extend_damage_time(const Uint32 time);
void blit_damage(const new_camera *camera, SDL_Surface *screen);

#endif