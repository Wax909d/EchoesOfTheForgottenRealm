#!/usr/bin/env python3
"""Remove // and /* */ comments from the C++ sources in src/, while leaving
string/char literals untouched. Also trims trailing whitespace and collapses
runs of blank lines. Run from project root: python3 tools/strip_comments.py
"""
import os, glob

def strip(code: str) -> str:
    out = []
    i, n = 0, len(code)
    state = "normal"   # normal | str | chr | line | block
    while i < n:
        c = code[i]
        nxt = code[i+1] if i+1 < n else ""
        if state == "normal":
            if c == '"':
                out.append(c); state = "str"; i += 1
            elif c == "'":
                out.append(c); state = "chr"; i += 1
            elif c == "/" and nxt == "/":
                state = "line"; i += 2
            elif c == "/" and nxt == "*":
                state = "block"; i += 2
            else:
                out.append(c); i += 1
        elif state == "str":
            out.append(c)
            if c == "\\":
                if i+1 < n: out.append(code[i+1]); i += 2
                else: i += 1
            else:
                if c == '"': state = "normal"
                i += 1
        elif state == "chr":
            out.append(c)
            if c == "\\":
                if i+1 < n: out.append(code[i+1]); i += 2
                else: i += 1
            else:
                if c == "'": state = "normal"
                i += 1
        elif state == "line":
            if c == "\n":
                out.append(c); state = "normal"
            i += 1
        elif state == "block":
            if c == "*" and nxt == "/":
                state = "normal"; i += 2
            else:
                if c == "\n": out.append(c)   # keep line breaks
                i += 1
    return "".join(out)

def tidy(text: str) -> str:
    lines = [ln.rstrip() for ln in text.split("\n")]
    result, blanks = [], 0
    for ln in lines:
        if ln == "":
            blanks += 1
            if blanks <= 1:
                result.append(ln)
        else:
            blanks = 0
            result.append(ln)
    while result and result[-1] == "":
        result.pop()
    return "\n".join(result) + "\n"

root = os.path.join(os.path.dirname(__file__), "..", "src")
files = sorted(glob.glob(os.path.join(root, "*.h")) +
               glob.glob(os.path.join(root, "*.cpp")))
for path in files:
    src = open(path).read()
    open(path, "w").write(tidy(strip(src)))
    print("stripped", os.path.basename(path))
print(f"done: {len(files)} files")
