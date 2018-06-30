#ifndef SETTINGS_H
#define SETTINGS_H

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include <SDL_mixer.h>

void load_settings();
void save_settings();

int get_resolution_x();
void set_resolution_x(const int num);

int get_resolution_y();
void set_resolution_y(const int num);

int get_fullscreen_toggle();
void set_fullscreen_toggle(const int num);

int get_music_volume();
void set_music_volume(const int num);

int get_sfx_volume();
void set_sfx_volume(const int num);

#endif