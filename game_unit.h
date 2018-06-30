#ifndef GAME_UNIT_H
#define GAME_UNIT_H

#include <SDL.h>
#include <SDL_mixer.h>
#include <stdlib.h>
#include <string.h>
#include <graphic.h>
#include <map.h>
#include <damage.h>

#define dir_none 0

#define dir_down 1
#define dir_left 2
#define dir_right 4
#define dir_up 6

#define dir_down_left 3
#define dir_down_right 5
#define dir_up_left 8
#define dir_up_right 10

#define mode_stand 0
#define mode_walk 1
#define mode_atk 2
#define mode_cast 3
#define mode_bow 4

#define physical_skill 0
#define ranged_skill 1
#define magic_skill 2

typedef struct game_unit_stats{
  /* combat points */
  int max_hp;
  int hp_reg_rate;
  
  int max_mp;
  int mp_reg_rate;
  
  /* base stats */
  int p_atk;
  int p_def;
  int atk_speed;
  
  int m_atk;
  int m_def;
  int cast_speed;
  
  int walk_speed;
  
  /* changses */
  int physical_changse;
  int physical_resistance;
  
  int magic_changse;
  int magic_resistance;
  
  int reflect_changse;
  int block_changse;
  int absorb_hp_rate;
  int absorb_mp_rate;
}game_unit_stats;

typedef struct new_enchantment{
  /* graphic variables */
  SDL_Surface *icon;
  
  /* animation variables */
  SDL_Surface *sprite_sheet;
  SDL_Rect sprite_src;
  
  Uint32 animation_start_time;
  Uint32 animation_time_stop;
  Uint32 animation_duration;
  
  /* time variables */
  Uint32 start_time;
  Uint32 life_time;
  
  Uint32 effect_timestop;
  Uint32 effect_interval;
  
  int hp_incrase;
  int mp_incrase;
  
  game_unit_stats stats;
  
  struct new_enchantment *next;
}new_enchantment;

typedef struct new_skill{
  /* graphic variables */
  Mix_Chunk *cast_sound;
  SDL_Surface *cast_sprite_sheet;
  SDL_Rect cast_sprite_src;
  Uint32 animation_time_stop;
  
  /* skill variables */
  int skill_type;
  SDL_Surface *icon;
  
  Uint32 start_time;
  Uint32 reuse_time;
  
  int hp_cost;
  int mp_cost;
  
  int p_atk;
  int m_atk;
  
  int base_changse;
  int physical_changse;
  int magic_changse;
  
  /* new_skill_object variables */
  Mix_Chunk *object_start_sound;
  Mix_Chunk *object_hit_sound;
  SDL_Surface *object_sprite_sheet;
  SDL_Surface *object_hit_sprite_sheet;
  int object_random_direction;
  int object_amount;
  int collision_w;
  int collision_h;
  int object_speed;
  Uint32 object_life_time;
  
  Mix_Chunk *self_enchantment_sound;
  new_enchantment *self_enchantment;
  
  Mix_Chunk *target_enchantment_sound;
  new_enchantment *target_enchantment;
  
  struct new_skill *next;
}new_skill;

typedef struct new_skill_object{
  /* object variables */
  double x;
  double y;
  int collision_w;
  int collision_h;
  
  Uint32 start_time;
  Uint32 life_time;
  
  int direction;
  int speed;
  Uint32 last_step;
  
  /* graphic variables */
  Mix_Chunk *hit_sound;
  SDL_Surface *sprite_sheet;
  SDL_Surface *hit_sprite_sheet;
  SDL_Rect sprite_src;
  Uint32 time_stop;
  
  /* stats */
  int p_atk;
  int m_atk;
  
  int base_changse;
  int physical_changse;
  int magic_changse;
  
  Mix_Chunk *enchantment_sound;
  new_enchantment *enchantment;
  
  struct new_skill_object *next;
}new_skill_object;

typedef struct new_game_unit{
  /* game variables */
  double x;
  double y;
  int collision_w;
  int collision_h;
  int direction;
  Uint32 last_step;
  
  /* graphic variables */
  SDL_Surface *sprite_sheet;
  SDL_Rect sprite_src;
  Uint32 time_stop;
  int sprite_mode;
  
  int frame_atk;
  int frame_cast;
  int frame_bow;
  int frame_die;
  
  /* stats */
  int hp;
  int mp;
  Uint32 last_hp_reg;
  Uint32 last_mp_reg;
  
  game_unit_stats base_stats;
  game_unit_stats stats;
  
  /* ability list */
  new_enchantment *enchantment_list;
  new_skill *skill_list;
  
  new_skill *current_skill;
  Uint32 current_skill_start_time;
  
  new_skill_object *skill_object_list;
}new_game_unit;

void set_spawn(new_game_unit *game_unit, const int spawn_num);

void set_camera(new_camera *camera, const new_game_unit *game_unit);
void blit_game_unit(const new_camera *camera, const new_game_unit *game_unit, SDL_Surface *screen);
char split_dir_x(const char direction);
char split_dir_y(const char direction);
void handle_collision(new_game_unit *game_unit, const new_camera *camera);

new_skill *load_skill(const char *name);
new_game_unit *load_game_unit(const char *name);
void free_game_unit(new_game_unit *game_unit);

void animate_game_unit(new_game_unit *game_unit);
void move_game_unit(new_game_unit *game_unit);

void extend_game_unit_time(new_game_unit *game_unit, const Uint32 time);
void update_game_unit_stats(new_game_unit *game_unit);
void handle_game_unit(new_game_unit *game_unit);
void handle_game_unit_damage(new_game_unit *game_unit, new_game_unit *game_unit_target);

void use_skill(new_game_unit *game_unit, new_skill *skill);

void append_current_skill_object(new_game_unit *game_unit);
void reflect_skill_object(new_game_unit *game_unit, new_skill_object *skill_object, new_game_unit *game_unit_target);

void append_enchantment(new_game_unit *game_unit, new_enchantment *enchantment);
void cancel_all_enchantments(new_game_unit *game_unit);

#endif