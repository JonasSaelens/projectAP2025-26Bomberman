# Bomberman: AP2025-26 Project

- **Author:** Jonas Saelens
- **Student number:** *s0240883*
- **Repository:** https://github.com/JonasSaelens/projectAP2025-26Bomberman
- **documentation (doxygen):** https://jonassaelens.github.io/projectAP2025-26Bomberman


## Building

```bash
cmake -B build -S .
cmake --build build
```

SFML 2.6.1 is fetched automatically via `FetchContent`. Run the `bomberman` executable produced
under `build/representation/`.

**Controls:** Arrow keys to move, Space to place a bomb.

## Design choices

- **Logic / representation split**: `logic` has no SFML dependency and only knows about game
  rules; `representation` handles windowing, sprites, sound, and animation.
- **Observer**: every entity (`EntityModel`) is a `Subject`. `EntityView`s and `Score` are
  `Observer`s that react to entity events (moved, died, bomb exploded, power-up collected, ...)
  to update sprites/animations/sound and score, with zero coupling back to `logic`.
- **Abstract Factory**: `World` creates entities through an `AbstractFactory` interface;
  `ConcreteFactory` (in `representation`) builds the entity and its matching view together.
- **State**: `Game` delegates to `MenuState` / `PlayingState` / `GameOverState` via a
  `StateManager`.
- **Entity hierarchy**: `Character` (player + `Bot`), `Bomb`, `Wall`, `Flame`, and `PowerUp`
  (with `Fire`/`ExtraBomb`/`Skates` subclasses) all derive from `EntityModel`.
- **Singletons**: `Random` and `Stopwatch` for RNG and delta-time.

## Class diagram

<img width="1089" height="711" alt="image" src="https://github.com/user-attachments/assets/1341fffe-555f-4365-b5ae-685a89a17117" />

## Bonus features

**sounds and music** were implemented as a bonus extension: background music per state
(menu/playing/game over) and sound effects for bomb ticks, explosions, power-up pickups, and
game over all triggered through the same Observer events used for animation.

