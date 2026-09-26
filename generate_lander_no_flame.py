from pathlib import Path

from PIL import Image


ROOT = Path(__file__).resolve().parent
SOURCE = ROOT / "98.png"
OUTPUT = ROOT / "lander_frames_no_flame.inc"
CROPS = (
    ("00", 0, 0, 2, 2),
    ("15", 3, 0, 6, 3),
    ("30", 7, 0, 10, 3),
    ("45", 11, 0, 14, 3),
    ("60", 16, 0, 19, 3),
    ("75", 21, 0, 24, 3),
    ("90", 27, 0, 29, 2),
)


def pack_rows(image):
    width, height = image.size
    pixels = image.convert("RGB")
    rows = []

    for y in range(height):
        row = []
        for byte_x in range(0, width, 8):
            value = 0
            for bit in range(8):
                color = pixels.getpixel((byte_x + bit, y))
                if color == (255, 255, 255):
                    value |= 0x80 >> bit
                elif color != (0, 0, 0):
                    raise ValueError(f"Unexpected pixel color: {color}")
            row.append(value)
        rows.append(row)

    return rows


def main():
    source = Image.open(SOURCE).convert("RGB")
    crops = {}
    for name, left, top, right, bottom in CROPS:
        bounds = (left * 8, top * 8, (right + 1) * 8, (bottom + 1) * 8)
        crops[name] = source.crop(bounds)

    transpose = Image.Transpose
    quadrants = (
        (0, lambda image: image),
        (1, lambda image: image.transpose(transpose.FLIP_TOP_BOTTOM)),
        (2, lambda image: image.transpose(transpose.ROTATE_180)),
        (3, lambda image: image.transpose(transpose.FLIP_LEFT_RIGHT)),
    )

    lines = ["#asm"]
    for quadrant, mirror in quadrants:
        for name, _, _, _, _ in CROPS:
            image = mirror(crops[name])
            width, height = image.size
            lines.append(f"._lander_no_flame_q{quadrant}_{name}")
            lines.append(f" defb {width},{height}")
            for row in pack_rows(image):
                lines.append(" defb " + ",".join(map(str, row)))
    lines.append("#endasm")
    OUTPUT.write_text("\n".join(lines) + "\n", encoding="ascii")
    print(f"Wrote {OUTPUT.name}: {len(quadrants) * len(CROPS)} frames")


if __name__ == "__main__":
    main()