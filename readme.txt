Project - 2D Space Game [Strike Fighters]
Group Member: Warden Guo , Wenjing Huang 
Student ID: 101237545 & 101261410

Implementation Details

1. Game mechanics:
 Control:
 	W/S: Forward/backward movement
 	A/D: Rotate ship left/right
 	Z/C: Strafe left/right
 	SPACE: Fire weapon
 	Q: Switch between weapons (Laser/Missile)
 	ESC: Exit game
- The game has two levels, in the first level, the player needs to destroy five enemies.
- When enemies' counter reaches 5, the game switches to the second level and generates a boss in the center of the map.
- Once the player dies, the game ends and the player loses.
- When the player reaches the second level and defeats the boss, the game ends and the player wins.

2. Enemies:
- There are four different enemies (including boss) in the game. Enemies in the first level will track the player when they detect the player, while the boss in the second level is fixed in the center of the map. All enemies (exclude boss) use a state machine (Patrolling → Intercepting → Attacking)
- Slow enemies have higher health and can fire missiles that do not track but do more damage.
- Fast enemies have lower health, fire forward bullets that do less damage, but move faster.
- Shotgun enemies have moderate health, speed, and damage, but can fire bullets in five directions at once.
- The boss is a robotic enemy with three links. It turns counterclockwise on its own, firing bullets in five directions at once. It should be noted that the boss is immune to missile damage, and the player can only attack it with normal bullets.

3. Weapons:
- The player have two weapons, press the space bar to fire, press the q key will switch weapons.
- Regular bullet weapons will fire forward and deal lower damage.
- Missile weapons will automatically track enemies and deal high damage.

4. Collectible items:
- There are three different collectibles in the game.
- Players collect 5 blue circles, will enter invincible mode for 10 seconds, immune to all enemy damage.
- The player collects a yellow circle, which will increase the movement speed for 10 seconds.
- The player collects a red circle that increases the damage of all weapons for 10 seconds.

5. Game world:
- The game world consists of two levels, each with a different background.
- The game world has boundaries that the player cannot move beyond a certain distance.
- The game world will randomly generate 5 asteroids that can bounce off the player, each with a different size, according to the size will have a different rebound force.

6. Particle Systems:
- The game has two instances of the particle system.
- The game world will always be floating with white cosmic dust.
- All missiles have a flame effect on the base.

7. UI:
- The player has a graphical blood bar that will be displayed according to the amount of blood the player has left.
- There is also a display of the duration of the game, and a current player defeated enemy counter. It should be noted that only enemies defeated by the player with weapons will be counted.
- There is a mini-map at the top left of the game that will show the position of the player and the enemy in real time. Red dots are enemies, blue dots are players, and green dots are bosses.

Sprite Credits:
- Player's spaceship sprite taken from "Tiny Ships (FREE Spaceships)" by Fearless Design on itch.io.
  URL: https://fearless-design.itch.io/tiny-ships-free-spaceships
  License: Creative Commons Attribution v4.0 International (CC BY 4.0).

- Enemy's spaceship sprite taken from "Tiny Ships (FREE Spaceships)" by Fearless Design on itch.io.
  URL: https://fearless-design.itch.io/tiny-ships-free-spaceships
  License: Creative Commons Attribution v4.0 International (CC BY 4.0).

- Explosion sprite taken from "Five Free Pixel Explosions" by unTied Games:
  URL: https://untiedgames.itch.io/five-free-pixel-explosions
  Author: unTied Games
  (Pack is free; see itch.io page for license/usage details.)

- Asteroid sprite taken from "Free Celestial Objects Pixel Art Pack" by Fearless Design on itch.io.
  URL: https://norma-2d.itch.io/celestial-objects-pixel-art-pack
  Author: Norma2D
  (Pack is free; see itch.io page for details.)

- Health Bar sprite taken from "pixel-health-bar-asset-pack" by adwitr on itch.io.
  URL: https://adwitr.itch.io/pixel-health-bar-asset-pack
  Author: adwitr
  (Pack is free; see itch.io page for details.)

- Background texture taken from "Space" by PiiiXL on itch.io.
  URL: https://piiixl.itch.io/space
  Author: PiiiXL
  License: Creative Commons Attribution v4.0 International (CC BY 4.0).

- Boss sprite taken from "Sci-fi Turret Sprite Pack" by Felmir Productions on itch.io.
  URL: https://felmir-productions.itch.io/sci-fi-turret-sprite-pack
  Author: Felmir Productions
  License: Creative Commons Attribution v4.0 International (CC BY 4.0).

- Particle effects created using Pixilart.
  URL: https://www.pixilart.com/draw
  Author: Self-created content

- Laser bullets and missile graphics from "Space Shooter - Ship Constructor" by anim86 on itch.io.
  URL: https://anim86.itch.io/space-shooter-ship-constructor
  Author: anim86
  Published: Oct 11, 2017 (Updated: Apr 01, 2024)
  License: Standard itch.io license for purchased assets


Audio Credits:
- Created using https://sfxr.me/ (sound effect generation).
- Background Music: "Universe" by SergePavkinMusic
  (Used under Pixabay Content ID license)
- Sound effects from "200 Free SFX" by Kronbits:
  URL: https://kronbits.itch.io/freesfx
  Author: Kronbits
  License: Creative Commons Zero v1.0 Universal

Build Notes:
- Project uses CMake for cross-platform compatibility
- Requires OpenGL, GLEW, GLFW, GLM, and SOIL libraries

Known Issues
- The game has some memory leak issues that have been identified but not fully resolved. These leaks do not affect gameplay or performance during normal play sessions.