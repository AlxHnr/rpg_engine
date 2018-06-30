#include <settings.h>

static int resolution_x;
static int resolution_y;
static Uint8 fullscreen_toggle;

static int music_volume;
static int sfx_volume;

void load_settings()
{
  int file;
  
  if((file = open("setting.conf", O_RDONLY)) != -1)
  {
    read(file, &resolution_x, sizeof(resolution_x));
    read(file, &resolution_y, sizeof(resolution_y));
    read(file, &fullscreen_toggle, sizeof(fullscreen_toggle));
    
    read(file, &music_volume, sizeof(music_volume));
    read(file, &sfx_volume, sizeof(sfx_volume));
    
    close(file);
  }
  else
  {
    /* create and safe new settings */
    resolution_x = 800;
    resolution_y = 600;
    fullscreen_toggle = 0;
    
    music_volume = (int)(MIX_MAX_VOLUME/2);
    sfx_volume = (int)(MIX_MAX_VOLUME/2);
    
    save_settings();
  }
}
void save_settings()
{
  int file;
  
  if((file = open("setting.conf", O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE)) != -1)
  {
    write(file, &resolution_x, sizeof(resolution_x));
    write(file, &resolution_y, sizeof(resolution_y));
    write(file, &fullscreen_toggle, sizeof(fullscreen_toggle));
    
    write(file, &music_volume, sizeof(music_volume));
    write(file, &sfx_volume, sizeof(sfx_volume));
    
    close(file);
  }
}

int get_resolution_x()
{
  return resolution_x;
}
void set_resolution_x(const int num)
{
  resolution_x = num;
}

int get_resolution_y()
{
  return resolution_y;
}
void set_resolution_y(const int num)
{
  resolution_y = num;
}

int get_fullscreen_toggle()
{
  return fullscreen_toggle;
}
void set_fullscreen_toggle(const int num)
{
  fullscreen_toggle = num;
}

int get_music_volume()
{
  return music_volume;
}
void set_music_volume(const int num)
{
  if(num < 0)
    music_volume = 0;
  else if(num > MIX_MAX_VOLUME)
    music_volume = MIX_MAX_VOLUME;
  else
    music_volume = num;
}

int get_sfx_volume()
{
  return sfx_volume;
}
void set_sfx_volume(const int num)
{
  if(num < 0)
    sfx_volume = 0;
  else if(num > MIX_MAX_VOLUME)
    sfx_volume = MIX_MAX_VOLUME;
  else
    sfx_volume = num;
}