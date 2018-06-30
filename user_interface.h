#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <graphic.h>
#include <game_unit.h>
#include <map.h>

int load_user_interface();
void free_user_interface();

void blit_user_interface(const new_game_unit *game_unit, const new_camera *camera, SDL_Surface *screen);

#endif