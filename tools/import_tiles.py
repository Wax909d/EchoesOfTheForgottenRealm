#!/usr/bin/env python3
"""Skin the map with CraftPix dungeon tiles, resized to the game's 24px tiles.
  wall.png      <- Tile_16 (brick wall)
  floor.png     <- Tile_45 (flagstone)
  exit.png      <- Door_D frame 0 (closed)
  exit_open.png <- Door_D last frame (open)
Output -> assets/textures/
"""
import os
from PIL import Image

PACK  = "/Users/waxd/Downloads/craftpix-net-436971-free-top-down-roguelike-game-kit-pixel-art"
TILES = os.path.join(PACK, "2 Dungeon Tileset", "1 Tiles")
AO    = os.path.join(PACK, "2 Dungeon Tileset", "3 Animated objects")
OUT   = os.path.join(os.path.dirname(__file__), "..", "assets", "textures")
TS    = 24

def to24(img):
    return img.resize((TS, TS), Image.NEAREST)

def tile(n):
    return Image.open(os.path.join(TILES, f"Tile_{n:02d}.png")).convert("RGBA")

to24(tile(16)).save(os.path.join(OUT, "wall.png"))
to24(tile(45)).save(os.path.join(OUT, "floor.png"))
print("wrote wall.png, floor.png")

# Door_D is an 80x20 strip = 4 frames of 20x20 (closed -> open).
door = Image.open(os.path.join(AO, "Door_D.png")).convert("RGBA")
fw = door.height                      # 20
closed = door.crop((0, 0, fw, fw))
opened = door.crop((door.width - fw, 0, door.width, fw))
to24(closed).save(os.path.join(OUT, "exit.png"))
to24(opened).save(os.path.join(OUT, "exit_open.png"))
print("wrote exit.png (closed), exit_open.png (open)")
