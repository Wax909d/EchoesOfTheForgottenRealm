#!/usr/bin/env python3
"""Extra textures: 4-directional player, hearts, skeleton, ghost.
Run from project root: python3 tools/gen_extra.py  -> assets/textures/
"""
import os
from PIL import Image, ImageDraw

OUT = os.path.join(os.path.dirname(__file__), "..", "assets", "textures")
T = (0, 0, 0, 0)
def new(w, h): return Image.new("RGBA", (w, h), T)
def save(img, n): img.save(os.path.join(OUT, n)); print("wrote", n, img.size)
def px(d, x, y, c): d.point((x, y), fill=c)

skin   = (240, 205, 165, 255)
cloak  = (60, 180, 90, 255)
cloakd = (35, 120, 60, 255)
belt   = (90, 60, 30, 255)
eye    = (30, 40, 35, 255)

# ---------------------------------------------------------------------------
# PLAYER  4 directions x 2 frames  (16x16 cells -> 32 wide, 64 tall)
# Row order matches Direction enum mapping in code: 0 Down,1 Up,2 Left,3 Right
# ---------------------------------------------------------------------------
def legs(d, step, col=cloakd):
    if step == 0:
        d.rectangle([6, 12, 7, 14], fill=col); d.rectangle([9, 12, 10, 13], fill=col)
    else:
        d.rectangle([6, 12, 7, 13], fill=col); d.rectangle([9, 12, 10, 14], fill=col)

def p_down(step):
    img = new(16, 16); d = ImageDraw.Draw(img)
    d.rectangle([6, 2, 9, 5], fill=skin)
    d.rectangle([5, 1, 10, 1], fill=cloakd)
    d.rectangle([5, 2, 5, 4], fill=cloakd); d.rectangle([10, 2, 10, 4], fill=cloakd)
    px(d, 7, 4, eye); px(d, 9, 4, eye)
    d.rectangle([5, 6, 10, 11], fill=cloak)
    d.rectangle([5, 6, 5, 11], fill=cloakd); d.rectangle([10, 6, 10, 11], fill=cloakd)
    d.rectangle([5, 9, 10, 9], fill=belt)
    d.rectangle([4, 7, 4, 9], fill=skin); d.rectangle([11, 7, 11, 9], fill=skin)
    legs(d, step)
    return img

def p_up(step):
    img = new(16, 16); d = ImageDraw.Draw(img)
    # back of hood: all dark, small nape highlight
    d.rectangle([5, 1, 10, 5], fill=cloakd)
    d.rectangle([7, 5, 8, 5], fill=(50, 150, 80, 255))
    d.rectangle([5, 6, 10, 11], fill=cloak)
    d.rectangle([5, 6, 5, 11], fill=cloakd); d.rectangle([10, 6, 10, 11], fill=cloakd)
    d.rectangle([5, 9, 10, 9], fill=belt)
    d.rectangle([4, 7, 4, 9], fill=skin); d.rectangle([11, 7, 11, 9], fill=skin)
    legs(d, step)
    return img

def p_left(step):
    img = new(16, 16); d = ImageDraw.Draw(img)
    # head profile facing left
    d.rectangle([5, 2, 9, 5], fill=skin)
    d.rectangle([5, 1, 10, 1], fill=cloakd)
    d.rectangle([9, 2, 10, 5], fill=cloakd)         # hood at back (right)
    px(d, 6, 4, eye)                                # single eye toward left
    d.rectangle([5, 6, 10, 11], fill=cloak)
    d.rectangle([9, 6, 10, 11], fill=cloakd)
    d.rectangle([5, 9, 10, 9], fill=belt)
    d.rectangle([4, 7, 4, 9], fill=skin)            # leading arm
    # walking legs (front/back)
    if step == 0:
        d.rectangle([5, 12, 6, 14], fill=cloakd); d.rectangle([9, 12, 10, 13], fill=cloakd)
    else:
        d.rectangle([5, 12, 6, 13], fill=cloakd); d.rectangle([9, 12, 10, 14], fill=cloakd)
    return img

def p_right(step):
    return p_left(step).transpose(Image.FLIP_LEFT_RIGHT)

rows = [
    [p_down(0),  p_down(1)],
    [p_up(0),    p_up(1)],
    [p_left(0),  p_left(1)],
    [p_right(0), p_right(1)],
]
sheet = Image.new("RGBA", (32, 64), T)
for r, frames in enumerate(rows):
    for c, f in enumerate(frames):
        sheet.paste(f, (c * 16, r * 16))
save(sheet, "player.png")

# ---------------------------------------------------------------------------
# HEARTS  3 frames (empty, half, full) 14x14 -> 42x14
# ---------------------------------------------------------------------------
def heart_shape(d, fill, outline):
    # two top lobes + body
    d.ellipse([1, 2, 6, 7], fill=fill, outline=outline)
    d.ellipse([7, 2, 12, 7], fill=fill, outline=outline)
    d.polygon([(1, 5), (12, 5), (7, 12), (6, 12)], fill=fill)
    d.polygon([(1, 5), (7, 11), (13, 5)], fill=fill)
    # outline lower
    d.line([(1, 5), (7, 12)], fill=outline)
    d.line([(13, 5), (7, 12)], fill=outline)

red   = (220, 50, 70, 255)
redhi = (255, 130, 150, 255)
gray  = (70, 70, 88, 255)
grayd = (45, 45, 60, 255)

def heart(kind):
    img = new(14, 14); d = ImageDraw.Draw(img)
    if kind == "empty":
        heart_shape(d, gray, grayd)
    elif kind == "half":
        heart_shape(d, gray, grayd)
        # paint left half red
        left = new(14, 14); ld = ImageDraw.Draw(left)
        heart_shape(ld, red, (150, 30, 45, 255))
        ld.rectangle([7, 0, 13, 13], fill=T)        # clear right half
        img.alpha_composite(left)
    else:  # full
        heart_shape(d, red, (150, 30, 45, 255))
        px(d, 3, 4, redhi); px(d, 4, 4, redhi)
    return img

hearts = Image.new("RGBA", (42, 14), T)
for i, k in enumerate(["empty", "half", "full"]):
    hearts.paste(heart(k), (i * 14, 0))
save(hearts, "heart.png")

# ---------------------------------------------------------------------------
# SKELETON  16x16 x2  - slow tanky chaser
# ---------------------------------------------------------------------------
def skeleton(step):
    img = new(16, 16); d = ImageDraw.Draw(img)
    bone = (225, 225, 210, 255); boned = (160, 160, 148, 255); dark = (40, 40, 45, 255)
    d.ellipse([5, 2, 10, 7], fill=bone, outline=boned)  # skull
    px(d, 6, 4, dark); px(d, 9, 4, dark)                # eye sockets
    px(d, 7, 6, dark); px(d, 8, 6, dark)                # teeth
    d.rectangle([7, 7, 8, 11], fill=bone)               # spine
    for ry in (8, 10):                                  # ribs
        d.line([(5, ry), (10, ry)], fill=bone)
    d.rectangle([4, 8, 4, 10], fill=bone); d.rectangle([11, 8, 11, 10], fill=bone)
    if step == 0:
        d.rectangle([5, 12, 6, 15], fill=bone); d.rectangle([9, 12, 10, 14], fill=bone)
    else:
        d.rectangle([5, 12, 6, 14], fill=bone); d.rectangle([9, 12, 10, 15], fill=bone)
    return img

save(Image.new("RGBA", (32, 16), T).copy() if False else
     (lambda s: (s.paste(skeleton(0), (0, 0)), s.paste(skeleton(1), (16, 0)), s)[-1])
     (Image.new("RGBA", (32, 16), T)), "skeleton.png")

# ---------------------------------------------------------------------------
# GHOST  16x16 x2 - translucent, drifts through walls
# ---------------------------------------------------------------------------
def ghost(step):
    img = new(16, 16); d = ImageDraw.Draw(img)
    body = (200, 215, 245, 220); bodd = (150, 165, 205, 220); eye = (40, 50, 80, 255)
    d.ellipse([3, 2, 12, 11], fill=body, outline=bodd)  # head
    d.rectangle([3, 7, 12, 12], fill=body)
    # wavy bottom
    if step == 0:
        d.polygon([(3, 12), (5, 14), (7, 12), (9, 14), (11, 12), (12, 13), (12, 12)],
                  fill=body)
    else:
        d.polygon([(3, 13), (5, 12), (7, 14), (9, 12), (11, 14), (12, 12)], fill=body)
    px(d, 6, 6, eye); px(d, 6, 7, eye)
    px(d, 10, 6, eye); px(d, 10, 7, eye)
    return img

gh = Image.new("RGBA", (32, 16), T)
gh.paste(ghost(0), (0, 0)); gh.paste(ghost(1), (16, 0))
save(gh, "ghost.png")

print("extra textures done")
