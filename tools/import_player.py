#!/usr/bin/env python3
"""Build the player's animation sheets from CraftPix Character 2.

For each animation (idle/walk/attack/hurt/death) it assembles a 4-row sheet
(0 Down, 1 Up, 2 Left, 3 Right) from the D_/U_/S_ strips. "Side" faces LEFT,
so Left = side as-is and Right = side mirrored.

Output -> assets/textures/player_{idle,walk,attack,hurt,death}.png
"""
import os
from PIL import Image

PACK = "/Users/waxd/Downloads/craftpix-net-436971-free-top-down-roguelike-game-kit-pixel-art"
SRC  = os.path.join(PACK, "1 Characters", "2")
OUT  = os.path.join(os.path.dirname(__file__), "..", "assets", "textures")
FW   = 32

# game clip name -> CraftPix file suffix
ANIMS = {"idle": "Idle", "walk": "Walk", "attack": "Attack",
         "hurt": "Hurt", "death": "Death"}

def load(name):
    return Image.open(os.path.join(SRC, name + ".png")).convert("RGBA")

for clip, suffix in ANIMS.items():
    d = load("D_" + suffix)
    u = load("U_" + suffix)
    s = load("S_" + suffix)
    frames = d.width // FW
    sheet  = Image.new("RGBA", (frames * FW, 4 * FW), (0, 0, 0, 0))
    sheet.paste(d, (0, 0 * FW))                                    # Down
    sheet.paste(u, (0, 1 * FW))                                    # Up
    sheet.paste(s, (0, 2 * FW))                                    # Left
    sheet.paste(s.transpose(Image.FLIP_LEFT_RIGHT), (0, 3 * FW))   # Right
    sheet.save(os.path.join(OUT, f"player_{clip}.png"))
    print(f"wrote player_{clip}.png  ({frames} frames x 4 rows)")
