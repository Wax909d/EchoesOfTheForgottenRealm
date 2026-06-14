#!/usr/bin/env python3
"""Synthesize chiptune-style sound effects and a looping music track.
Pure standard library (wave + math). Output -> assets/audio/*.wav
"""
import os, math, struct, wave, random

OUT = os.path.join(os.path.dirname(__file__), "..", "assets", "audio")
os.makedirs(OUT, exist_ok=True)
SR = 44100

def write_wav(name, samples):
    path = os.path.join(OUT, name)
    with wave.open(path, "w") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(SR)
        frames = bytearray()
        for s in samples:
            s = max(-1.0, min(1.0, s))
            frames += struct.pack("<h", int(s * 32767))
        w.writeframes(bytes(frames))
    print("wrote", name, f"{len(samples)/SR:.2f}s")

def square(t, f):
    return 1.0 if math.sin(2*math.pi*f*t) >= 0 else -1.0

def tri(t, f):
    x = (f * t) % 1.0
    return 4*abs(x-0.5) - 1.0

def env(i, n, attack=0.01, release=0.2):
    t = i / SR
    dur = n / SR
    a = min(1.0, t/attack) if attack > 0 else 1.0
    r = min(1.0, (dur - t)/release) if release > 0 else 1.0
    return max(0.0, a) * max(0.0, r)

def tone(freq, dur, vol=0.4, wave_fn=square, attack=0.005, release=0.06):
    n = int(SR * dur)
    out = []
    for i in range(n):
        t = i / SR
        out.append(wave_fn(t, freq) * vol * env(i, n, attack, release))
    return out

def noise(dur, vol=0.4, release=0.1):
    n = int(SR * dur)
    out = []
    for i in range(n):
        out.append((random.uniform(-1, 1)) * vol * env(i, n, 0.001, release))
    return out

# --- hit: short bright blip down ------------------------------------------
hit = []
for f, dl in [(880, 0.04), (660, 0.05)]:
    hit += tone(f, dl, vol=0.35, wave_fn=square)
write_wav("hit.wav", hit)

# --- swing: quick airy whoosh ---------------------------------------------
swing = []
for f, dl in [(680, 0.03), (420, 0.04)]:
    swing += tone(f, dl, vol=0.16, wave_fn=tri, release=0.03)
swing = [a + b for a, b in zip(swing + [0]*999, noise(len(swing)/SR, vol=0.07))][:len(swing)]
write_wav("swing.wav", swing)

# --- pickup: rising sparkle arpeggio --------------------------------------
pick = []
for f in [523, 659, 784, 1047]:
    pick += tone(f, 0.06, vol=0.3, wave_fn=tri, release=0.04)
write_wav("pickup.wav", pick)

# --- hurt: low buzzy descend + noise --------------------------------------
hurt = []
for f, dl in [(220, 0.07), (160, 0.09)]:
    hurt += tone(f, dl, vol=0.4, wave_fn=square)
hurt = [a + b for a, b in zip(hurt + [0]*999, noise(len(hurt)/SR, vol=0.15))][:len(hurt)]
write_wav("hurt.wav", hurt)

# --- evil laugh: menacing "muahaha" the boss plays on a kill --------------
def evil_laugh():
    dur = 1.7
    n = int(SR * dur)
    out = []
    base = 175.0
    for i in range(n):
        t = i / SR
        # pitch glides down + slow vibrato for a sinister wobble
        f = base * (1.0 - 0.18 * t / dur) + 6.0 * math.sin(2 * math.pi * 5.0 * t)
        carrier = 0.6 * square(t, f) + 0.4 * tri(t, f * 0.5)   # + sub octave
        # "ha-ha-ha" syllable pulse (~6.5 Hz, sharpened)
        pulse = max(0.0, math.sin(2 * math.pi * 6.5 * t)) ** 3
        breath = random.uniform(-1, 1) * 0.18 * pulse
        env_o = min(1.0, t / 0.04) * max(0.0, (dur - t) / 0.45)
        out.append((carrier * pulse + breath) * 0.5 * env_o)
    return out

write_wav("evil_laugh.wav", evil_laugh())

# --- music: slow atmospheric dungeon theme (looping) ----------------------
# Three soft voices: a melancholic minor melody, a low bass root, and a quiet
# fifth "pad" underneath. Slow tempo + soft envelopes give an eerie, spacious
# dungeon mood instead of a marchy chiptune.
random.seed(5)
A = 220.0
def n(semi):
    return A * (2 ** (semi / 12.0))

note_dur = 0.46
melody = [ 7, 8, 7, 5,   3, 5, 7, 3,    8, 7, 5, 3,   2, 3, 5, 7,
          12,10, 8, 7,   5, 7, 8, 5,    3, 2, 0, 2,   3, 5, 3, 0]
roots  = [-5,-5,-5,-5,  -7,-7,-7,-7,   -5,-5,-5,-5, -10,-10,-10,-10,
          -5,-5,-5,-5,  -7,-7,-7,-7,   -5,-5,-5,-5, -12,-12,-12,-12]

music = []
for i in range(len(melody)):
    nm  = int(note_dur * SR)
    mel = tone(n(melody[i]),          note_dur, vol=0.15, wave_fn=tri,
               attack=0.05, release=0.16)
    bass= tone(n(roots[i]) / 2.0,     note_dur, vol=0.16, wave_fn=tri,
               attack=0.08, release=0.20)
    pad = tone(n(roots[i] + 7) / 2.0, note_dur, vol=0.05, wave_fn=tri,
               attack=0.12, release=0.22)
    for j in range(nm):
        s = (mel[j] if j < len(mel) else 0) \
          + (bass[j] if j < len(bass) else 0) \
          + (pad[j]  if j < len(pad)  else 0)
        music.append(s * 0.85)
write_wav("music.wav", music)

print("audio done")
