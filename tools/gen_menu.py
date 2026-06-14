#!/usr/bin/env python3
"""Build a dungeon-style main-menu background (480x360 logical) from the
CraftPix tiles + torches + fog, so the menu matches the in-game look.
Output -> assets/textures/menu_bg.png   (the menu title text is drawn by code)
"""
import os
from PIL import Image, ImageDraw

PACK = "/Users/waxd/Downloads/craftpix-net-436971-free-top-down-roguelike-game-kit-pixel-art"
OUT  = os.path.join(os.path.dirname(__file__), "..", "assets", "textures")
TS   = 24
W, H = 480, 360

tex   = lambda n: Image.open(os.path.join(OUT, n + ".png")).convert("RGBA")
floor = tex("floor")
wall  = tex("wall")

img = Image.new("RGBA", (W, H), (18, 18, 26, 255))
# floor everywhere
for y in range(0, H, TS):
    for x in range(0, W, TS):
        img.alpha_composite(floor, (x, y))
# wall frame: top 2 rows, bottom row, side columns
cols, rows = W // TS, H // TS
for cx in range(cols):
    img.alpha_composite(wall, (cx * TS, 0))
    img.alpha_composite(wall, (cx * TS, 1 * TS))
    img.alpha_composite(wall, (cx * TS, (rows - 1) * TS))
for ry in range(rows):
    img.alpha_composite(wall, (0, ry * TS))
    img.alpha_composite(wall, ((cols - 1) * TS, ry * TS))

# --- torches mounted on the top wall, with warm glow ---
TOBJ = os.path.join(PACK, "2 Dungeon Tileset", "2 Objects", "Torches")
FIRE = os.path.join(PACK, "2 Dungeon Tileset", "3 Animated objects", "Fire1.png")
torch_files = sorted(f for f in os.listdir(TOBJ) if f.endswith(".png"))
torch = Image.open(os.path.join(TOBJ, torch_files[2])).convert("RGBA")  # a lit one
fire  = Image.open(FIRE).convert("RGBA")
fframe = fire.crop((0, 0, fire.height, fire.height))   # first flame frame
SC = 3
torch_s = torch.resize((torch.width * SC, torch.height * SC), Image.NEAREST)
fire_s  = fframe.resize((fframe.width * 2, fframe.height * 2), Image.NEAREST)

def glow(cx, cy, r, colour):
    g = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    gd = ImageDraw.Draw(g)
    for i in range(r, 0, -2):
        a = int(70 * (i / r) * 0.5)
        gd.ellipse([cx - (r - i), cy - (r - i), cx + (r - i), cy + (r - i)],
                   fill=(colour[0], colour[1], colour[2], 70 - a))
    return g

for tx in (90, 240, 390):
    ty = 44
    img = Image.alpha_composite(img, glow(tx, ty, 60, (255, 150, 50)))
    img.alpha_composite(torch_s, (tx - torch_s.width // 2, ty - 6))
    img.alpha_composite(fire_s,  (tx - fire_s.width // 2, ty - 22))

# --- fog overlay (subtle, tiled) ---
fog = Image.open(os.path.join(PACK, "4 GUI", "Fog.png")).convert("RGBA")
fog = fog.resize((fog.width * 3, fog.height * 3), Image.NEAREST)
fog_layer = Image.new("RGBA", (W, H), (0, 0, 0, 0))
for y in range(0, H, fog.height):
    for x in range(0, W, fog.width):
        fog_layer.alpha_composite(fog, (x, y))
fog_layer.putalpha(fog_layer.getchannel("A").point(lambda a: a // 4))
img = Image.alpha_composite(img, fog_layer)

# --- darken + vignette so the title text reads clearly ---
dark = Image.new("RGBA", (W, H), (0, 0, 0, 90))
img = Image.alpha_composite(img, dark)
vig = Image.new("RGBA", (W, H), (0, 0, 0, 0))
vd = ImageDraw.Draw(vig)
for i in range(80):
    a = max(0, 120 - i * 2)
    vd.rectangle([i, i, W - 1 - i, H - 1 - i], outline=(0, 0, 0, a))
img = Image.alpha_composite(img, vig)

# soft dark band behind the title (code draws title around y60-110)
band = Image.new("RGBA", (W, H), (0, 0, 0, 0))
bd = ImageDraw.Draw(band)
bd.rectangle([40, 52, W - 40, 120], fill=(0, 0, 0, 90))
img = Image.alpha_composite(img, band)

img.save(os.path.join(OUT, "menu_bg.png"))
print("wrote menu_bg.png", img.size)
