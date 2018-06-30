#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include <SDL.h>

#define BUTTON_LEFT 0
#define BUTTON_RIGHT 1
#define BUTTON_UP 2
#define BUTTON_DOWN 3

#define BUTTON_1 4
#define BUTTON_2 5
#define BUTTON_3 6
#define BUTTON_4 7
#define BUTTON_5 8
#define BUTTON_6 9
#define BUTTON_7 10
#define BUTTON_8 11

#define BUTTON_PAUSE 12
#define BUTTON_OK 13
#define BUTTON_ABORT 14

#define BUTTON_MAX 15
#define PLAYER_MAX 4

#define AXIS_DEAD_ZONE 3200

void load_joystick();
void free_joystick();
void save_joystick();

void update_button_state();
int get_button_state(const int player, const int button);
void set_button_state(const int player, const int button, const int value);
void hard_set_button_state(const int player, const int button, const SDL_Event hard_button_state);
void reset_all_buttons();

int get_all_button_state(const int button);
void set_all_button_state(const int button, const int value);

#endif