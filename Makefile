P_NAME  = rpg_engine
OBJECTS = main.o map.o game_unit.o graphic.o user_interface.o menu.o settings.o joystick.o framerate.o game.o damage.o

# flags
CFLAGS  = -Wall -O2 `sdl-config --cflags` -I.
LDFLAGS = `sdl-config --libs` -lSDL_gfx -lSDL_image -lSDL_ttf -lSDL_mixer

# main application
$(P_NAME)/$(P_NAME): $(OBJECTS:%.o=obj/%.o)
	mkdir -p $(P_NAME)
	$(CC) $(OBJECTS:%.o=obj/%.o) $(LDFLAGS) -o $(P_NAME)/$(P_NAME)

map_editor: obj/map.o obj/map_editor.o
	$(CC) obj/map.o obj/map_editor.o $(LDFLAGS) -o $(P_NAME)/map_editor

# compile objects
obj/%.o: %.c
	mkdir -p obj
	$(CC) $(CFLAGS) -c $<
	mv *.o obj

# run application
run: $(P_NAME)/$(P_NAME)
	cd $(P_NAME) && ./$(P_NAME)

# additional stuff

clean:
	- rm -rf $(P_NAME)/$(P_NAME) $(P_NAME)/map_editor $(P_NAME)/*.conf obj
