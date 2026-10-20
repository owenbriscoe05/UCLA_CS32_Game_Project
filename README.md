# UCLA CS32 Dungeon Game

A turn-based dungeon crawler written in C++ for UCLA's CS32 course, which I took during my freshman year after CS31. The project was designed to demonstrate mastery of C++ through a complete, playable game. This repository preserves the original coursework, including its learning-stage rough edges.

## Gameplay

Explore five randomly generated dungeon floors (numbered 0–4), with rooms connected by hallways. You play as `@`: fight monsters by moving toward them, collect weapons and scrolls, and descend using the stairs (`>`). Collect the golden idol (`&`) on the fifth floor to win.

Bogeymen (`B`) and snakewomen (`S`) chase nearby players, goblins (`G`) use recursive pathfinding within a configurable smell distance, and dragons (`D`) stay in place and can regenerate health. Combat uses randomized attack and defense rolls based on character statistics and weapons. Monsters may drop equipment; scrolls can improve statistics or teleport the player, and magic fangs can put an opponent to sleep.

## C++ concepts

- Class hierarchies, inheritance, virtual functions, and polymorphism for actors and game objects.
- Constructors, destructors, pointers, and manual management of dynamically allocated objects.
- Recursive search and backtracking for goblin movement.
- Arrays, references, and runtime casts (`dynamic_cast`) for inventory and grid management.
- Coordinating game state: health, armor, strength, dexterity, sleep, equipment, and turns.

## Build and run

With a C++11-compatible compiler on macOS or Linux, run from the project directory:

```sh
c++ -std=c++11 main.cpp Game.cpp Actor.cpp GameObject.cpp utilities.cpp -o dungeon
./dungeon
```

Use an interactive terminal for keyboard input and screen clearing. No external libraries are required.

## Controls

| Key | Action |
| --- | --- |
| Arrow keys or `h` / `j` / `k` / `l` | Move left / down / up / right; attack an adjacent monster |
| `g` | Pick up an item or the golden idol |
| `i` | Show inventory |
| `w`, then an inventory letter | Wield a weapon |
| `r`, then an inventory letter | Read a scroll |
| `>` | Descend while standing on stairs |
| `c` | Cheat: set strength to 9 and maximum health to 50 |
| `q` | Quit |

Weapons appear as `)`, scrolls as `?`, and walls as `#`.

## Source layout

- `Game.cpp` / `Game.h`: dungeon generation, game loop, spawning, and turn coordination.
- `Actor.cpp` / `Actor.h`: player and monster classes, combat, movement, and inventory.
- `GameObject.cpp` / `GameObject.h`: weapons and scrolls.
- `main.cpp` and `utilities.cpp` / `utilities.h`: entry point and course-provided terminal/random utilities.
- [report.txt](report.txt): original write-up on goblin recursion and dungeon generation.
