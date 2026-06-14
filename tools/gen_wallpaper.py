#!/usr/bin/env python3
"""Generate the 480x360 main-menu wallpaper -> assets/textures/menu_bg.png"""
import os, math, random
from PIL import Image, ImageDraw

OUT = os.path.join(os.path.dirname(__file__), "..", "assets", "textures")
W, H = 480, 360
random.seed(7)

img = Image.new("RGBA", (W, H), (0, 0, 0, 255))
d = ImageDraw.Draw(img)

# Vertical gradient: deep indigo -> dusky purple
top = (16, 14, 38)
bot = (60, 26, 60)
for y in range(H):
    t = y / (H - 1)
    r = int(top[0] + (bot[0]-top[0]) * t)
    g = int(top[1] + (bot[1]-top[1]) * t)
    b = int(top[2] + (bot[2]-top[2]) * t)
    d.line([(0, y), (W, y)], fill=(r, g, b, 255))

# Stars
for _ in range(140):
    x = random.randint(0, W-1); y = random.randint(0, int(H*0.62))
    bright = random.choice([140, 180, 220, 255])
    s = random.choice([0, 0, 1])
    d.rectangle([x, y, x+s, y+s], fill=(bright, bright, 255, 255))

# Moon with soft halo
mx, my, mr = 380, 80, 34
for i in range(10, 0, -1):
    a = int(10 + i * 3)
    rr = mr + i * 4
    d.ellipse([mx-rr, my-rr, mx+rr, my+rr], fill=(180, 180, 230, a))
d.ellipse([mx-mr, my-mr, mx+mr, my+mr], fill=(225, 225, 245, 255))
d.ellipse([mx-12, my-8, mx-2, my+2], fill=(205, 205, 230, 255))
d.ellipse([mx+6, my+8, mx+16, my+18], fill=(205, 205, 230, 255))

# Distant ruined towers as a silhouette along the bottom
horizon = 250
d.rectangle([0, horizon, W, H], fill=(20, 14, 30, 255))
def tower(x, w, h, broken=True):
    top_y = horizon - h
    d.rectangle([x, top_y, x+w, horizon], fill=(12, 9, 22, 255))
    if broken:
        # jagged broken top
        for bx in range(x, x+w, 6):
            jag = random.randint(0, 8)
            d.rectangle([bx, top_y - jag, bx+3, top_y], fill=(12, 9, 22, 255))
    # a couple of lit windows
    for _ in range(max(1, h // 30)):
        wx = random.randint(x+3, x+w-5); wy = random.randint(top_y+10, horizon-10)
        d.rectangle([wx, wy, wx+2, wy+3], fill=(255, 200, 90, 255))

tower(10, 46, 120)
tower(70, 30, 70)
tower(150, 26, 50, broken=False)
tower(300, 40, 95)
tower(360, 28, 60)
tower(420, 50, 130)

# Foreground ground mist (subtle purple haze along the very bottom)
for y in range(H-30, H):
    a = int((y - (H-30)) / 30 * 22)
    d.line([(0, y), (W, y)], fill=(90, 70, 120, a))

# Subtle vignette
vig = Image.new("RGBA", (W, H), (0, 0, 0, 0))
vd = ImageDraw.Draw(vig)
for i in range(60):
    a = int(i * 1.2)
    vd.rectangle([i, i, W-1-i, H-1-i], outline=(0, 0, 0, max(0, 70 - a)))
img = Image.alpha_composite(img, vig)

img.save(os.path.join(OUT, "menu_bg.png"))
print("wrote menu_bg.png", img.size)
