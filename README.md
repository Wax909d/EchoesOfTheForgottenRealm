# Echoes of the Forgotten Realm

A small Game Boy–style top-down adventure built with **C++17** and **SFML 2.6**.
Explore three dungeon levels, fight **three enemy types** (Slime, Skeleton,
Archer), and find the **Key** that unlocks the exit. **Rune Shards** are
optional treasure shown on the HUD, and **Health Potions** restore hearts.

Enemy positions are **randomised every run** (no two playthroughs are the same),
and gameplay numbers — player HP, enemy stats, per-level enemy counts — live in
[`assets/config/game.cfg`](assets/config/game.cfg) so they can be tuned without
recompiling.

This project was written for a university **OOP course** and is deliberately
structured to showcase inheritance, polymorphism, abstract classes,
encapsulation, composition, smart pointers, `dynamic_cast` and modern C++17.

---

## Controls

| Key | Action |
|-----|--------|
| `W A S D` / Arrow keys | Move |
| `Space` | Melee attack (swing in the facing direction) |
| `Esc` | Pause / resume (or quit from the menu) |
| `Enter` | Start game / confirm on menus |

**Goal of each level:** find the **Key** (cyan gem) — it opens the exit door —
then step on the exit to descend. Grab Health Potions to heal and Rune Shards
for score along the way. Survive all three levels to win. If your hearts run
out, it's game over.

---

## Building

You need SFML 2.6 and CMake ≥ 3.16.

### CLion
Open the project folder. CLion detects `CMakeLists.txt` automatically.
Build with `⌘F9`, run with `⌃R`. (CLion builds into `cmake-build-debug/`; the
`assets/` folder is copied there automatically.)

### Command line
```bash
mkdir -p build && cd build
cmake ..
cmake --build . --parallel
./EchoesOfTheForgottenRealm
```

> **macOS note:** the build pins SFML 2.6 explicitly (Homebrew may also have
> SFML 3 installed) via `set(SFML_DIR ...)` in `CMakeLists.txt`. Adjust that
> path if your SFML lives elsewhere.

> Assets are copied next to the binary at **CMake configure time**. If you edit
> a level or sprite, re-run CMake (or rebuild after a CMake change) so the
> updated files are copied.

---

## Class architecture

```
GameObject (abstract)
├─ Player                         melee hero, 4-directional, knockback
├─ Enemy (abstract)
│  ├─ Slime      slow random wanderer
│  ├─ Skeleton   slow, tanky, wall-respecting chaser
│  └─ Archer     stationary; fires Projectiles (arrows)
├─ Projectile                     fixed-direction, expiring arrow
└─ Pickup (abstract)
   ├─ HealthPotion                restores HP
   ├─ RuneShard                   optional treasure (HUD score)
   └─ Key                         unlocks the level exit
```

Supporting (non-entity) classes:

| Class | Responsibility |
|-------|----------------|
| `Game` | Window, game-state machine, the entity list, camera, collisions, random enemy spawning |
| `TileMap` | Loads a level from text, draws it, answers collision queries, lists free floor tiles |
| `Animation` | Plays a (optionally multi-row) spritesheet; composed by every `GameObject` |
| `TextureManager` | Owns all textures for the program lifetime |
| `AudioManager` | Small pooled sound-effect player |
| `Config` | Reads `key = value` gameplay parameters from `game.cfg` |

### Where each OOP concept lives

| Concept | Example |
|---------|---------|
| **Abstract class** | `GameObject`, `Enemy`, `Pickup` (pure virtual methods) |
| **Inheritance** | every entity derives from `GameObject` |
| **Polymorphism** | `Game` stores `vector<unique_ptr<GameObject>>` and calls `update()/draw()` on the base type; `Pickup::apply(Player*)` |
| **`dynamic_cast` (RTTI)** | `Game::handleCollisions` and `absorbSpawned` downcast `GameObject*` to `Enemy*` / `Projectile*` / `Pickup*` to dispatch behaviour |
| **Encapsulation** | data members are `protected`/`private`, exposed via getters |
| **Composition** | each `GameObject` *has an* `Animation`; `Game` *has a* `TileMap`, `TextureManager`, `AudioManager` |
| **Smart pointers** | `std::unique_ptr<GameObject>` for all owned entities |
| **C++17** | `enum class`, `std::make_unique`, range-for, `constexpr`, lambdas |

---

## Game states

```cpp
enum class GameState { Menu, Playing, Paused, GameOver, Win };
```
Handled with plain `switch` statements (no State pattern), as required.

---

## Levels & the map format

Levels are plain text in `assets/levels/`. Each is 30×22 tiles; the window
scrolls (a follow-camera) to keep the player centred. Maps define only the
geometry, player spawn, exit and pickups — **enemies are placed on random
free floor tiles at load time**, with per-level counts read from `game.cfg`.

| Char | Meaning |
|------|---------|
| `#` / `=` | wall |
| `.` | floor |
| `P` | player spawn |
| `E` | exit (locked until the Key is collected) |
| `H` | health potion |
| `R` | rune shard (optional treasure) |
| `Y` | key (opens the exit) |

---

## Regenerating assets

All art and audio are generated procedurally — no binary assets are
hand-painted. Regenerate them from the project root:

```bash
python3 tools/gen_textures.py    # entity & tile sprites
python3 tools/gen_extra.py       # 4-dir player, hearts, skeleton, ghost
python3 tools/gen_wallpaper.py   # main-menu background
python3 tools/gen_audio.py       # SFX + looping music
python3 tools/gen_levels.py      # the three level files
```
(Requires `Pillow` for images; audio uses only the Python standard library.)
To use your **own** art instead, just drop a PNG into `assets/textures/` with
the same frame size — the loader picks it up, and any entity with no texture
falls back to a coloured rectangle automatically.

---

## Project layout

```
EchoesOfTheForgottenRealm/
├── CMakeLists.txt
├── README.md
├── src/            all C++ sources & headers
├── assets/
│   ├── levels/     level1-3.txt
│   ├── textures/   sprites, tiles, menu_bg
│   ├── audio/      sfx + music
│   └── fonts/      PressStart2P.ttf
└── tools/          asset generator scripts
```
