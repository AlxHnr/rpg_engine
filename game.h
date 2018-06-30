#ifndef GAME_H
#define GAME_H

#include <string.h>
#include <stdlib.h>
#include <SDL.h>
#include <game_unit.h>
#include <user_interface.h>
#include <joystick.h>
#include <framerate.h>
#include <map.h>
#include <menu.h>

#define SPAWN_POINTS 8
#define RESPAWN_DELAY 5000

int start_game(const char *player_path[PLAYER_MAX], int *team_list, SDL_Surface *screen);

#endif