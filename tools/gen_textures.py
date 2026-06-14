#!/usr/bin/env python3
"""Procedurally generate all pixel-art textures for the game.
Run from the project root: python3 tools/gen_textures.py
Output -> assets/textures/
"""
import os
from PIL import Image, ImageDraw

OUT = os.path.join(os.path.dirname(__file__), "..", "assets", "textures")
os.makedirs(OUT, exist_ok=True)

TRANSPARENT = (0, 0, 0, 0)

def new(w, h):
    return Image.new("RGBA", (w, h), TRANSPARENT)

def save(img, name):
    img.save(os.path.join(OUT, name))
    print("wrote", name, img.size)

def strip(frames):
    """Combine equal-size frames horizontally into one sheet."""
    w = sum(f.width for f in frames)
    h = max(f.height for f in frames)
    sheet = Image.new("RGBA", (w, h), TRANSPARENT)
    x = 0
    for f in frames:
        sheet.paste(f, (x, 0))
        x += f.width
    return sheet

def px(d, x, y, c):
    d.point((x, y), fill=c)

def outline_body(d, cells, color, oc):
    """Draw filled cells (list of (x,y)) plus a 1px darker outline."""
    s = set(cells)
    for (x, y) in cells:
        px(d, x, y, color)
    for (x, y) in cells:
        for nx, ny in ((x+1,y),(x-1,y),(x,y+1),(x,y-1)):
            if (nx, ny) not in s:
                px(d, nx, ny, oc)

# ---------------------------------------------------------------------------
# PLAYER  (16x16, 2 frames) - a green-cloaked adventurer
# ---------------------------------------------------------------------------
def player_frame(step):
    img = new(16, 16); d = ImageDraw.Draw(img)
    skin   = (240, 205, 165, 255)
    cloak  = (60, 180, 90, 255)
    cloakd = (35, 120, 60, 255)
    belt   = (90, 60, 30, 255)
    eye    = (30, 40, 35, 255)
    # head
    d.rectangle([6, 2, 9, 5], fill=skin)
    d.rectangle([5, 1, 10, 1], fill=cloakd)        # hair/hood top
    d.rectangle([5, 2, 5, 4], fill=cloakd)
    d.rectangle([10, 2, 10, 4], fill=cloakd)
    px(d, 7, 4, eye); px(d, 9, 4, eye)
    # body cloak
    d.rectangle([5, 6, 10, 11], fill=cloak)
    d.rectangle([5, 6, 5, 11], fill=cloakd)
    d.rectangle([10, 6, 10, 11], fill=cloakd)
    d.rectangle([5, 9, 10, 9], fill=belt)
    # arms
    d.rectangle([4, 7, 4, 9], fill=skin)
    d.rectangle([11, 7, 11, 9], fill=skin)
    # legs (animate)
    if step == 0:
        d.rectangle([6, 12, 7, 14], fill=cloakd)
        d.rectangle([9, 12, 10, 13], fill=cloakd)
    else:
        d.rectangle([6, 12, 7, 13], fill=cloakd)
        d.rectangle([9, 12, 10, 14], fill=cloakd)
    return img

save(strip([player_frame(0), player_frame(1)]), "player.png")

# ---------------------------------------------------------------------------
# SLIME  (16x16, 2 frames) - lime blob
# ---------------------------------------------------------------------------
def slime_frame(squash):
    img = new(16, 16); d = ImageDraw.Draw(img)
    body = (130, 210, 90, 255)
    bodd = (80, 160, 55, 255)
    hi   = (190, 240, 150, 255)
    eye  = (25, 45, 25, 255)
    top = 6 if squash else 4
    d.ellipse([2, top, 13, 14], fill=body, outline=bodd)
    d.ellipse([4, top+1, 8, top+4], fill=hi)        # shine
    ey = top + 4
    px(d, 6, ey, eye); px(d, 6, ey+1, eye)
    px(d, 10, ey, eye); px(d, 10, ey+1, eye)
    return img

save(strip([slime_frame(False), slime_frame(True)]), "slime.png")

# ---------------------------------------------------------------------------
# BAT  (14x14, 2 frames) - purple, flapping wings
# ---------------------------------------------------------------------------
def bat_frame(up):
    img = new(14, 14); d = ImageDraw.Draw(img)
    body = (170, 95, 215, 255)
    bodd = (110, 55, 150, 255)
    eye  = (255, 230, 90, 255)
    # body
    d.ellipse([5, 5, 9, 11], fill=body, outline=bodd)
    px(d, 6, 7, eye); px(d, 8, 7, eye)
    # ears
    px(d, 5, 4, bodd); px(d, 9, 4, bodd)
    # wings
    if up:
        d.polygon([(5,6),(0,2),(2,7)], fill=body, outline=bodd)
        d.polygon([(9,6),(14,2),(12,7)], fill=body, outline=bodd)
    else:
        d.polygon([(5,8),(0,10),(2,7)], fill=body, outline=bodd)
        d.polygon([(9,8),(14,10),(12,7)], fill=body, outline=bodd)
    return img

save(strip([bat_frame(True), bat_frame(False)]), "bat.png")

# ---------------------------------------------------------------------------
# ARCHER  (16x16, 2 frames) - orange ranger with a bow
# ---------------------------------------------------------------------------
def archer_frame(step):
    img = new(16, 16); d = ImageDraw.Draw(img)
    skin = (240, 205, 165, 255)
    tunic= (230, 140, 40, 255)
    tund = (170, 95, 20, 255)
    hat  = (120, 70, 20, 255)
    bow  = (150, 95, 40, 255)
    eye  = (30, 30, 30, 255)
    d.rectangle([6, 3, 9, 6], fill=skin)            # head
    d.rectangle([5, 2, 10, 2], fill=hat)            # hat brim
    d.rectangle([7, 0, 8, 1], fill=hat)             # hat top
    px(d, 7, 5, eye); px(d, 9, 5, eye)
    d.rectangle([6, 7, 9, 12], fill=tunic)          # body
    d.rectangle([6, 7, 6, 12], fill=tund)
    # bow on the right
    bx = 11 if step == 0 else 12
    d.arc([bx-1, 4, bx+2, 12], start=300, end=60, fill=bow)
    d.line([bx+1, 4, bx+1, 12], fill=(220,220,200,255))
    # legs
    d.rectangle([6, 13, 7, 15], fill=tund)
    d.rectangle([8, 13, 9, 15], fill=tund)
    return img

save(strip([archer_frame(0), archer_frame(1)]), "archer.png")

# ---------------------------------------------------------------------------
# BOSS  (24x24, 2 frames) - red armored warlock
# ---------------------------------------------------------------------------
def boss_frame(step):
    img = new(24, 24); d = ImageDraw.Draw(img)
    body = (210, 55, 55, 255)
    bodd = (140, 30, 30, 255)
    horn = (60, 20, 20, 255)
    eye  = (255, 230, 80, 255)
    robe = (150, 35, 40, 255)
    # horns
    d.polygon([(6,4),(3,0),(8,3)], fill=horn)
    d.polygon([(17,4),(20,0),(15,3)], fill=horn)
    # head
    d.ellipse([7, 3, 16, 12], fill=body, outline=bodd)
    glow = 1 if step == 0 else 0
    d.rectangle([9, 7, 10, 8+glow], fill=eye)
    d.rectangle([13, 7, 14, 8+glow], fill=eye)
    px(d, 11, 10, bodd); px(d, 12, 10, bodd)        # mouth
    # robe/body
    d.polygon([(5,13),(18,13),(21,23),(2,23)], fill=robe, outline=bodd)
    d.rectangle([10, 14, 13, 22], fill=body)        # robe seam
    # arms
    yo = 0 if step == 0 else 1
    d.rectangle([3, 13+yo, 5, 18+yo], fill=body)
    d.rectangle([18, 13-yo, 20, 18-yo], fill=body)
    return img

save(strip([boss_frame(0), boss_frame(1)]), "boss.png")

# ---------------------------------------------------------------------------
# PICKUPS (16x16, single frame)
# ---------------------------------------------------------------------------
def potion():
    img = new(16, 16); d = ImageDraw.Draw(img)
    glass=(250, 95, 135, 255); gl=(255,170,195,255)
    cork =(150, 95, 55, 255); neck=(220,220,230,255)
    d.rectangle([7, 1, 8, 2], fill=cork)            # cork
    d.rectangle([6, 3, 9, 4], fill=neck)            # neck
    d.ellipse([3, 5, 12, 14], fill=glass, outline=(190,55,90,255))
    d.ellipse([5, 7, 7, 9], fill=gl)                # shine
    return img
save(potion(), "potion.png")

def powerrune():
    img = new(16, 16); d = ImageDraw.Draw(img)
    core=(90, 150, 250, 255); coreh=(180,210,255,255); edge=(50,90,200,255)
    d.polygon([(8,1),(14,8),(8,15),(2,8)], fill=core, outline=edge)
    d.polygon([(8,4),(11,8),(8,12),(5,8)], fill=coreh)
    px(d, 8, 8, (255,255,255,255))
    return img
save(powerrune(), "powerrune.png")

def shard():
    img = new(16, 16); d = ImageDraw.Draw(img)
    gold=(250, 215, 70, 255); goldh=(255,245,170,255); edge=(190,150,30,255)
    d.polygon([(8,0),(12,6),(9,15),(7,15),(4,6)], fill=gold, outline=edge)
    d.line([(8,1),(8,14)], fill=goldh)
    px(d, 6, 5, goldh); px(d, 10, 8, edge)
    return img
save(shard(), "shard.png")

# ---------------------------------------------------------------------------
# TILES (24x24)
# ---------------------------------------------------------------------------
def wall():
    img = Image.new("RGBA", (24, 24), (52, 52, 74, 255)); d = ImageDraw.Draw(img)
    mortar=(34,34,50,255); hi=(70,70,96,255)
    # brick rows offset
    for ry, off in ((0,0),(8,12),(16,0)):
        d.line([(0,ry),(23,ry)], fill=mortar)
        for bx in range(off, 24, 12):
            d.line([(bx,ry),(bx,ry+8)], fill=mortar)
    for ry, off in ((0,0),(8,12),(16,0)):
        for bx in range(off+1, 24, 12):
            d.line([(bx,ry+1),(bx+9,ry+1)], fill=hi)
    return img
save(wall(), "wall.png")

def floor():
    img = Image.new("RGBA", (24, 24), (30, 30, 44, 255)); d = ImageDraw.Draw(img)
    line=(24,24,36,255); spec=(40,40,56,255)
    d.line([(0,0),(23,0)], fill=line); d.line([(0,0),(0,23)], fill=line)
    for (sx, sy) in ((5,7),(14,4),(18,16),(8,18),(11,12)):
        px(d, sx, sy, spec)
    return img
save(floor(), "floor.png")

def exit_tile():
    img = Image.new("RGBA", (24, 24), (40, 30, 22, 255)); d = ImageDraw.Draw(img)
    frame=(110, 80, 45, 255); door=(70, 50, 30, 255); glow=(90, 220, 130, 255)
    d.rectangle([3, 1, 20, 23], fill=door, outline=frame)
    d.rectangle([3, 1, 4, 23], fill=frame); d.rectangle([19, 1, 20, 23], fill=frame)
    d.arc([3, -8, 20, 8], start=0, end=180, fill=frame)
    d.ellipse([9, 9, 14, 16], fill=glow)            # rune sigil
    px(d, 11, 12, (255,255,255,255))
    return img
save(exit_tile(), "exit.png")

print("textures done")
