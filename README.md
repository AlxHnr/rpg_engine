![preview.gif](https://raw.github.com/AlxHnr/rpg_engine/master/preview.gif)

The demo above uses the
[Exterior 32x32 Town Tileset](https://opengameart.org/content/exterior-32x32-town-tileset).

This game is based on a [tile engine](https://github.com/AlxHnr/map_editor)
I wrote earlier. The controls default to the arrow keys, numbers, space and
escape. This can be changed in the menu. When configuring the controls
using a gamepad, make sure to disable the analog stick. Otherwise even the
slightest tilt can generate 50 events. This will assign all controls to the
analog stick. If you mess up the controls, remove the file
`rpg_engine/joystick.conf` and start the game again.

## Rendering a single frame
![rendering.gif](https://raw.github.com/AlxHnr/rpg_engine/master/rendering.gif)

## Building

This program requires the development files of the following libraries:

* SDL (_not_ SDL2)
* SDL\_image
* SDL\_mixer
* SDL\_gfx
* SDL\_ttf

Build the game using only one thread: `make -j1`. It can be started using
`make run`.

**Note**: The tileset used by the maps in this game is not included due to
unclear licensing. You could try saving something like
[this](https://telles0808.deviantart.com/art/RPG-Maker-VX-RTP-Tileset-159218223)
to `./rpg_engine/tileset.png`. Otherwise you would have to build your own
maps using a free 32x32 tileset.

The map editor has to be build using `make -j1 map_editor`. It can be
started by changing into the *rpg\_engine/* directory and running the
*map\_editor* binary from there. All tileset paths configured in the editor
must be relative to this directory. The editor can take the path to a map
as argument.

# Assets used

### Painterly Spell Icons [1](https://opengameart.org/content/painterly-spell-icons-part-1), [2](https://opengameart.org/content/painterly-spell-icons-part-2), [3](https://opengameart.org/content/painterly-spell-icons-part-3) and [4](https://opengameart.org/content/painterly-spell-icons-part-4)
* **Author**: J. W. Bjerk (eleazzaar) -- www.jwbjerk.com/art -- find this and other open art at: http://opengameart.org
* **Licenses**: CC-BY 3.0, CC-BY-SA 3.0, GPL 3.0, GPL 2.0

This icon pack is the main reason I wrote this game.

### The following assets by Clint Bellanger (license: CC-BY 3.0):

* [Isometric Hero and Heroine](https://opengameart.org/content/isometric-hero-and-heroine)
* [Lightning Shock Spell](https://opengameart.org/content/lightning-shock-spell)
* [Heal Spell](https://opengameart.org/content/heal-spell)
* [Shield Spell](https://opengameart.org/content/shield-spell)
* [Freeze Spell](https://opengameart.org/content/freeze-spell)
* [Sparks (Fire, Ice, Blood)](https://opengameart.org/content/sparks-fire-ice-blood)
* [Projectiles](https://opengameart.org/content/projectiles)
* [Quake Spell](https://opengameart.org/content/quake-spell)
* [Teleport Rune](https://opengameart.org/content/teleport-rune)
* [Fireball Spell](https://opengameart.org/content/fireball-spell)
* [Fire Spell Explosion](https://opengameart.org/content/fire-spell-explosion)
* [Icicle Spell](https://opengameart.org/content/icicle-spell)

The assets above have been a big motivation for me. I have scaled, tinted
and combined those graphics to fit my needs.

### [Random RPG Icons Part 1](https://opengameart.org/content/random-rpg-icons-part-1)
* **Author**: HorrorPen
* **License**: CC-BY 3.0

### [RPG GUI construction kit v1.0](https://opengameart.org/content/rpg-gui-construction-kit-v10)
* **Author**: Lamoot
* **License**: CC-BY 3.0

### [Health and Mana Bars](https://opengameart.org/content/health-and-mana-bars)
* **Author**: lancel
* **License**: CC-BY-SA 3.0

### [Fossil Cave](https://opengameart.org/content/fossil-cave)
* **Author**: Écrivain
* **License**: CC0

### [Sheet of old paper](https://opengameart.org/content/sheet-of-old-paper)
* **Author**: Q\_x
* **License**: CC0

## Sound effects

### [Spell Sounds Starter Pack](https://opengameart.org/content/spell-sounds-starter-pack)
* **Author**: p0ss
* **Licenses**: CC-BY-SA 3.0, GPL 3.0, GPL 2.0

### [Fire & Evil Spell](https://opengameart.org/content/fire-evil-spell)
* **Author**: artisticdude
* **License**: CC-BY 3.0

### [Arrow hit twang](https://opengameart.org/content/arrow-hit-twang)
* **Author**: qubodup
* **Licenses**: CC-BY-SA 3.0, GPL 3.0, GPL 2.0

### [Short wind sound](https://opengameart.org/content/short-wind-sound)
* **Author**: remaxim
* **License**: CC0

### [Freeze Spell](https://opengameart.org/content/freeze-spell-0)
* **Author**: artisticdude
* **License**: CC0

### [Spell 4 (fire)](https://opengameart.org/content/spell-4-fire)
* **Author**: Bart K.
* **Licenses**: CC-BY 3.0, CC-BY-SA 3.0, GPL 3.0, GPL 2.0, OGA-BY 3.0

### [Swish - bamboo stick weapon swhoshes](https://opengameart.org/content/swish-bamboo-stick-weapon-swhoshes)
* **Author**: Iwan Gabovitch
* **License**: CC0

### [Wizard Eye](https://freesound.org/people/Autistic%20Lucario/sounds/142610/)
* **Author**: Autistic Lucario
* **License**: CC BY 3.0

### [mystic 1](https://freesound.org/people/Zixem/sounds/69505/)
* **Author**: Zixem
* **License**: CC0

### [Teleport Spell](https://opengameart.org/content/teleport-spell)
* **Author**: Ogrebane
* **License**: CC0

### [Fire Whip Hit (Yo Frankie!)](https://opengameart.org/content/fire-whip-hit-yo-frankie)
* **Author**: Blender Foundation
* **License**: CC-BY 3.0

**Note**: This list of sound effects is incomplete. While _all_ of them are
free and come from either [opengameart.org](https://opengameart.org) or
[freesound.org](https://freesound.org), I failed find the some of the
sources. If you can help, please file an issue with a link to the original
work.

## Other assets

### Amiri Regular (Font)
* **Author**: Khaled Hosny
* **Licenses**: Public domain, GPL, OFL
