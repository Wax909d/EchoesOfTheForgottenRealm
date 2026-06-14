#!/usr/bin/env python3
"""Generate the three 30x22 level files.

Legend (geometry + player + exit + pickups ONLY — enemies are now placed
randomly at runtime by Game::spawnEnemiesRandomly):
  #  wall      .  floor     P  player spawn   E  exit
  H  potion    R  rune shard (treasure)       Y  key (opens the exit)

Run from project root: python3 tools/gen_levels.py
"""
import os

W, H = 30, 22
OUT = os.path.join(os.path.dirname(__file__), "..", "assets", "levels")

def blank():
    g = [['.' for _ in range(W)] for _ in range(H)]
    for x in range(W):
        g[0][x] = '#'; g[H-1][x] = '#'
    for y in range(H):
        g[y][0] = '#'; g[y][W-1] = '#'
    return g

def rect(g, x0, y0, x1, y1, ch='#'):
    for y in range(y0, y1+1):
        for x in range(x0, x1+1):
            g[y][x] = ch

def put(g, x, y, ch):
    assert g[y][x] != '#', f"item on wall at {x},{y}"
    g[y][x] = ch

def write(name, g):
    assert len(g) == H and all(len(r) == W for r in g)
    open(os.path.join(OUT, name), 'w').write(
        '\n'.join(''.join(r) for r in g) + '\n')
    print("wrote", name, f"{W}x{H}")

# --- LEVEL 1 -------------------------------------------------------------
g = blank()
rect(g, 6, 5, 7, 8)
rect(g, 20, 4, 23, 5)
rect(g, 12, 12, 15, 13)
rect(g, 24, 14, 25, 17)
put(g, 2, 2, 'P')
put(g, 27, 3, 'H')
put(g, 14, 18, 'R')
put(g, 21, 16, 'Y')        # key
put(g, 27, 19, 'E')
write("level1.txt", g)

# --- LEVEL 2 -------------------------------------------------------------
g = blank()
rect(g, 10, 3, 11, 10)
rect(g, 18, 11, 19, 18)
rect(g, 4, 14, 7, 15)
rect(g, 22, 4, 25, 5)
put(g, 2, 2, 'P')
put(g, 27, 3, 'H')
put(g, 4, 19, 'R')
put(g, 14, 9, 'Y')         # key
put(g, 27, 19, 'E')
write("level2.txt", g)

# --- LEVEL 3 -------------------------------------------------------------
g = blank()
# central arena kept purely as decoration, with gaps to walk through
rect(g, 11, 7, 18, 7)
rect(g, 11, 14, 18, 14)
rect(g, 11, 7, 11, 14)
rect(g, 18, 7, 18, 14)
g[7][14] = '.'; g[14][15] = '.'
g[10][11] = '.'; g[12][18] = '.'
put(g, 2, 2, 'P')
put(g, 27, 2, 'H')
put(g, 14, 11, 'R')        # treasure inside the arena
put(g, 2, 19, 'Y')         # key
put(g, 27, 19, 'E')
write("level3.txt", g)

print("levels done")
