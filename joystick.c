#include <joystick.h>

static SDL_Joystick *player_joystick[PLAYER_MAX];
static SDL_Event player_input[PLAYER_MAX][BUTTON_MAX];
static Uint8 button_state[PLAYER_MAX][BUTTON_MAX];

void load_joystick()
{
  int file;
  int counter = 0;
  
  if((file = open("joystick.conf", O_RDONLY)) != -1)
  {
    read(file, &player_input, sizeof(player_input));
    
    close(file);
  }
  else
  {
    /* create and safe new settings */
    while(counter < PLAYER_MAX)
    {
      player_input[counter][BUTTON_LEFT].type = SDL_KEYDOWN;
      player_input[counter][BUTTON_LEFT].key.keysym.sym = SDLK_LEFT;
      
      player_input[counter][BUTTON_RIGHT].type = SDL_KEYDOWN;
      player_input[counter][BUTTON_RIGHT].key.keysym.sym = SDLK_RIGHT;
      
      player_input[counter][BUTTON_UP].type = SDL_KEYDOWN;
      player_input[counter][BUTTON_UP].key.keysym.sym = SDLK_UP;
      
      player_input[counter][BUTTON_DOWN].type = SDL_KEYDOWN;
      player_input[counter][BUTTON_DOWN].key.keysym.sym = SDLK_DOWN;
      
      player_input[counter][BUTTON_1].type = SDL_KEYDOWN;
      player_input[counter][BUTTON_1].key.keysym.sym = SDLK_1;
      
      player_input[counter][BUTTON_2].type = SDL_KEYDOWN;
      player_input[counter][BUTTON_2].key.keysym.sym = SDLK_2;
      
      player_input[counter][BUTTON_3].type = SDL_KEYDOWN;
      player_input[counter][BUTTON_3].key.keysym.sym = SDLK_3;
      
      player_input[counter][BUTTON_4].type = SDL_KEYDOWN;
      player_input[counter][BUTTON_4].key.keysym.sym = SDLK_4;
      
      player_input[counter][BUTTON_5].type = SDL_KEYDOWN;
      player_input[counter][BUTTON_5].key.keysym.sym = SDLK_5;
      
      player_input[counter][BUTTON_6].type = SDL_KEYDOWN;
      player_input[counter][BUTTON_6].key.keysym.sym = SDLK_6;
      
      player_input[counter][BUTTON_7].type = SDL_KEYDOWN;
      player_input[counter][BUTTON_7].key.keysym.sym = SDLK_7;
      
      player_input[counter][BUTTON_8].type = SDL_KEYDOWN;
      player_input[counter][BUTTON_8].key.keysym.sym = SDLK_8;
      
      player_input[counter][BUTTON_PAUSE].type = SDL_KEYDOWN;
      player_input[counter][BUTTON_PAUSE].key.keysym.sym = SDLK_ESCAPE;
      
      player_input[counter][BUTTON_OK].type = SDL_KEYDOWN;
      player_input[counter][BUTTON_OK].key.keysym.sym = SDLK_SPACE;
      
      player_input[counter][BUTTON_ABORT].type = SDL_KEYDOWN;
      player_input[counter][BUTTON_ABORT].key.keysym.sym = SDLK_ESCAPE;
      
      counter++;
    }
    
    save_joystick();
  }
  
  /* open joysticks */
  counter = 0;
  while(counter < PLAYER_MAX)
  {
    player_joystick[counter] = SDL_JoystickOpen(counter);
    counter++;
  }
  
  /* reset button states */
  reset_all_buttons();
  
  SDL_JoystickEventState(SDL_ENABLE);
}
void free_joystick()
{
  int counter = 0;
  
  while(counter < PLAYER_MAX)
  {
    if(player_joystick[counter] != NULL)
      SDL_JoystickClose(player_joystick[counter]);
    player_joystick[counter] = NULL;
    
    counter++;
  }
}
void save_joystick()
{
  int file;
  
  if((file = open("joystick.conf", O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE)) != -1)
  {
    write(file, &player_input, sizeof(player_input));
    
    close(file);
  }
}

void update_button_state()
{
  SDL_Event user_input;
  int player_counter;
  int button_counter;
  
  /* set buttons */
  while(SDL_PollEvent(&user_input))
  {
    player_counter = 0;
    while(player_counter < PLAYER_MAX)
    {
      button_counter = 0;
      while(button_counter < BUTTON_MAX)
      {
        /* handle keyboard buttons */
        if(player_input[player_counter][button_counter].type == SDL_KEYDOWN &&
          user_input.key.keysym.sym == player_input[player_counter][button_counter].key.keysym.sym)
        {
          if(user_input.type == SDL_KEYDOWN)
            button_state[player_counter][button_counter] = 1;
          else if(user_input.type == SDL_KEYUP)
            button_state[player_counter][button_counter] = 0;
        }
        else if(player_input[player_counter][button_counter].type == SDL_JOYBUTTONDOWN &&
                player_input[player_counter][button_counter].jbutton.which == user_input.jbutton.which &&
                user_input.jbutton.button == player_input[player_counter][button_counter].jbutton.button)
        {
          if(user_input.jbutton.type == SDL_JOYBUTTONDOWN)
            button_state[player_counter][button_counter] = 1;
          else if(user_input.type == SDL_JOYBUTTONUP)
            button_state[player_counter][button_counter] = 0;
        }
        else if(user_input.type == SDL_JOYAXISMOTION &&
                player_input[player_counter][button_counter].type == SDL_JOYAXISMOTION &&
                player_input[player_counter][button_counter].jaxis.which == user_input.jaxis.which &&
                user_input.jaxis.axis == player_input[player_counter][button_counter].jaxis.axis)
        {
          if(player_input[player_counter][button_counter].jaxis.value < -AXIS_DEAD_ZONE)
          {
            if(user_input.jaxis.value < -AXIS_DEAD_ZONE)
              button_state[player_counter][button_counter] = 1;
            else
              button_state[player_counter][button_counter] = 0;
          }
          else if(player_input[player_counter][button_counter].jaxis.value > AXIS_DEAD_ZONE)
          {
            if(user_input.jaxis.value > AXIS_DEAD_ZONE)
              button_state[player_counter][button_counter] = 1;
            else
              button_state[player_counter][button_counter] = 0;
          }
        }
        button_counter++;
      }
      
      player_counter++;
    }
  }
}
int get_button_state(const int player, const int button)
{
  if(player >= 0 && player < PLAYER_MAX && button >= 0 && button < BUTTON_MAX)
    return button_state[player][button];
  else
    return 0;
}
void set_button_state(const int player, const int button, const int value)
{
  if(player >= 0 && player < PLAYER_MAX && button >= 0 && button < BUTTON_MAX)
    button_state[player][button] = value;
}
void hard_set_button_state(const int player, const int button, const SDL_Event hard_button_state)
{
  if(player >= 0 && player < PLAYER_MAX && button >= 0 && button < BUTTON_MAX)
    player_input[player][button] = hard_button_state;
}
void reset_all_buttons()
{
  int counter = 0;
  int counter_2;
  
  while(counter < PLAYER_MAX)
  {
    counter_2 = 0;
    while(counter_2 < BUTTON_MAX)
    {
      button_state[counter][counter_2] = 0;
      
      counter_2++;
    }
    
    counter++;
  }
}

int get_all_button_state(const int button)
{
  int counter = 0;
  
  if(button >= 0 && button < BUTTON_MAX)
  {
    while(counter < PLAYER_MAX)
    {
      if(get_button_state(counter, button))
        return button_state[counter][button];
      
      counter++;
    }
  }
  
  return 0;
}
void set_all_button_state(const int button, const int value)
{
  int counter = 0;
  
  if(button >= 0 && button < BUTTON_MAX)
  {
    while(counter < PLAYER_MAX)
    {
      set_button_state(counter, button, value);
      
      counter++;
    }
  }
}