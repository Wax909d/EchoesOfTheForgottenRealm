#!/usr/bin/env python3
"""Import two CraftPix characters as enemy sprites:
  Archer  <- 1 Characters/1  (D_Idle, front-facing, 4 frames of 32x32)
  Ghost   <- 1 Characters/3  (D_Walk, the wizard, 6 frames of 32x32)
Output -> assets/textures/{archer,ghost}.png  (single-row strips)
"""
import os
from PIL import Image

PACK = "/Users/waxd/Downloads/craftpix-net-436971-free-top-down-roguelike-game-kit-pixel-art"
OUT  = os.path.join(os.path.dirname(__file__), "..", "assets", "textures")

def grab(char, anim):
    return Image.open(os.path.join(PACK, "1 Characters", char, anim + ".png")).convert("RGBA")

# Archer = ranger (Character 1) standing idle, facing the player.
grab("1", "D_Idle").save(os.path.join(OUT, "archer.png"))
print("wrote archer.png  (Character 1 idle, 4 frames)")

# Ghost = wizard (Character 3) walking.
grab("3", "D_Walk").save(os.path.join(OUT, "ghost.png"))
print("wrote ghost.png   (Character 3 walk, 6 frames)")

# Slime -> Dungeon Enemy 1 (small gray imp), a low-tier dungeon creature.
Image.open(os.path.join(PACK, "3 Dungeon Enemies", "1", "D_Walk.png")) \
     .convert("RGBA").save(os.path.join(OUT, "slime.png"))
print("wrote slime.png   (Dungeon Enemy 1 walk, 6 frames)")
