"""Draws the MouseMend icon: a computer mouse with a plaster across it.
Writes src/MouseMend.ico (16 to 256 px, for the exe) and docs/logo512.png.
Drawn at 4x and scaled down, so the edges are smooth.

    python tools/make_icon.py
"""
import os

from PIL import Image, ImageDraw

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
S = 2048                      # working size, scaled down at the end
BODY = (16, 185, 129)         # emerald
BODY_DARK = (5, 150, 105)
WHEEL = (255, 255, 255)
TAPE = (250, 214, 165)
TAPE_EDGE = (226, 176, 120)
DOT = (214, 160, 104)


def mouse_layer():
    img = Image.new("RGBA", (S, S), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    # body: a tall rounded shape with a slightly narrower top
    left, top, right, bottom = int(S * .24), int(S * .08), int(S * .76), int(S * .92)
    r = int((right - left) * .5)
    d.rounded_rectangle((left, top, right, bottom), radius=r, fill=BODY)
    # lower half a shade darker: reads as a body, not a pill
    shade = Image.new("RGBA", (S, S), (0, 0, 0, 0))
    ds = ImageDraw.Draw(shade)
    ds.rounded_rectangle((left, top, right, bottom), radius=r, fill=BODY_DARK)
    mask = Image.new("L", (S, S), 0)
    ImageDraw.Draw(mask).rectangle((0, int(S * .55), S, S), fill=255)
    img.paste(shade, (0, 0), Image.composite(shade.split()[3], Image.new("L", (S, S), 0), mask))
    # the split between the buttons, and the wheel
    cx = S // 2
    d.line((cx, top + int(S * .03), cx, int(S * .40)), fill=(255, 255, 255, 150), width=int(S * .018))
    ww, wh = int(S * .075), int(S * .17)
    d.rounded_rectangle((cx - ww // 2, int(S * .20), cx + ww // 2, int(S * .20) + wh), radius=ww // 2, fill=WHEEL)
    return img


def tape_layer():
    w, h = int(S * .62), int(S * .19)
    t = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    d = ImageDraw.Draw(t)
    d.rounded_rectangle((0, 0, w - 1, h - 1), radius=h // 2, fill=TAPE, outline=TAPE_EDGE, width=int(S * .008))
    # the pad in the middle, with its dots
    pw = int(w * .34)
    d.rounded_rectangle(((w - pw) // 2, int(h * .18), (w + pw) // 2, int(h * .82)), radius=int(h * .16), fill=(242, 196, 140))
    for i in range(3):
        for j in range(2):
            x = (w - pw) // 2 + int(pw * (.25 + .25 * i))
            y = int(h * (.36 + .28 * j))
            rr = int(S * .012)
            d.ellipse((x - rr, y - rr, x + rr, y + rr), fill=DOT)
    return t.rotate(32, resample=Image.BICUBIC, expand=True)


def icon():
    img = mouse_layer()
    tape = tape_layer()
    img.alpha_composite(tape, (int(S * .47) - tape.width // 2, int(S * .62) - tape.height // 2))
    return img


def main():
    big = icon()
    os.makedirs(os.path.join(ROOT, "docs"), exist_ok=True)
    big.resize((512, 512), Image.LANCZOS).save(os.path.join(ROOT, "docs", "logo512.png"))
    sizes = [(16, 16), (20, 20), (24, 24), (32, 32), (40, 40), (48, 48), (64, 64), (128, 128), (256, 256)]
    base = big.resize((256, 256), Image.LANCZOS)
    base.save(os.path.join(ROOT, "src", "MouseMend.ico"), sizes=sizes)
    print("wrote src/MouseMend.ico and docs/logo512.png")


main()
