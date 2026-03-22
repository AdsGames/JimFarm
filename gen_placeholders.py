# Generates assets/images/placeholders.png
# Simple 16x16 placeholder sprites until real art exists.
# Cell (x, y) is referenced from tiles.json / items.json with "sheet": "placeholders"

from PIL import Image, ImageDraw

CELL = 16
OUT = "assets/images/placeholders.png"

img = Image.new("RGBA", (256, 256), (0, 0, 0, 0))
d = ImageDraw.Draw(img)


def cell(x, y):
  return x * CELL, y * CELL


def campfire(x, y, lit):
  ox, oy = cell(x, y)
  # Logs
  d.line([(ox + 3, oy + 13), (ox + 12, oy + 10)], fill=(110, 70, 30), width=2)
  d.line([(ox + 3, oy + 10), (ox + 12, oy + 13)], fill=(90, 55, 25), width=2)
  # Stones
  for sx in (1, 5, 10, 14):
    d.ellipse([ox + sx - 1, oy + 12, ox + sx + 1, oy + 14], fill=(120, 120, 120))
  if lit:
    d.polygon([(ox + 8, oy + 2), (ox + 12, oy + 11), (ox + 4, oy + 11)],
              fill=(240, 120, 20))
    d.polygon([(ox + 8, oy + 5), (ox + 10, oy + 11), (ox + 6, oy + 11)],
              fill=(255, 220, 60))
  else:
    d.ellipse([ox + 6, oy + 9, ox + 10, oy + 12], fill=(60, 60, 60))


def wolf(x, y, frame):
  ox, oy = cell(x, y)
  body = (110, 110, 120)
  dark = (70, 70, 80)
  # Body
  d.rectangle([ox + 3, oy + 7, ox + 12, oy + 11], fill=body)
  # Head
  d.rectangle([ox + 11, oy + 4, ox + 15, oy + 8], fill=body)
  d.polygon([(ox + 11, oy + 4), (ox + 12, oy + 2), (ox + 13, oy + 4)], fill=dark)
  d.point((ox + 14, oy + 5), fill=(255, 60, 60))
  # Tail
  d.line([(ox + 3, oy + 8), (ox + 0, oy + 5)], fill=dark, width=2)
  # Legs
  legs = (4, 7, 9, 11) if frame == 0 else (5, 6, 10, 12)
  for lx in legs:
    d.line([(ox + lx, oy + 12), (ox + lx, oy + 14)], fill=dark)


def spear(x, y):
  ox, oy = cell(x, y)
  d.line([(ox + 3, oy + 13), (ox + 11, oy + 5)], fill=(140, 90, 40), width=1)
  d.polygon([(ox + 10, oy + 4), (ox + 14, oy + 2), (ox + 12, oy + 6)],
            fill=(180, 180, 190))


def pelt(x, y):
  ox, oy = cell(x, y)
  d.polygon([(ox + 3, oy + 4), (ox + 13, oy + 4), (ox + 14, oy + 12),
             (ox + 8, oy + 14), (ox + 2, oy + 12)], fill=(130, 130, 140))
  d.line([(ox + 5, oy + 7), (ox + 11, oy + 7)], fill=(95, 95, 105))


def coat(x, y):
  ox, oy = cell(x, y)
  d.rectangle([ox + 4, oy + 3, ox + 11, oy + 14], fill=(150, 90, 50))
  d.rectangle([ox + 1, oy + 4, ox + 3, oy + 11], fill=(150, 90, 50))
  d.rectangle([ox + 12, oy + 4, ox + 14, oy + 11], fill=(150, 90, 50))
  d.rectangle([ox + 5, oy + 2, ox + 10, oy + 4], fill=(200, 200, 200))
  d.line([(ox + 8, oy + 5), (ox + 8, oy + 14)], fill=(90, 50, 25))


def cooked_egg(x, y):
  ox, oy = cell(x, y)
  d.ellipse([ox + 2, oy + 4, ox + 14, oy + 13], fill=(250, 250, 245))
  d.ellipse([ox + 6, oy + 6, ox + 10, oy + 10], fill=(250, 190, 30))


def stew(x, y):
  ox, oy = cell(x, y)
  d.ellipse([ox + 2, oy + 6, ox + 14, oy + 14], fill=(120, 80, 50))
  d.ellipse([ox + 3, oy + 6, ox + 13, oy + 10], fill=(200, 90, 40))
  d.point((ox + 6, oy + 8), fill=(240, 140, 40))
  d.point((ox + 10, oy + 7), fill=(90, 160, 60))


def campfire_item(x, y):
  ox, oy = cell(x, y)
  d.line([(ox + 2, oy + 12), (ox + 13, oy + 6)], fill=(110, 70, 30), width=3)
  d.line([(ox + 2, oy + 6), (ox + 13, oy + 12)], fill=(90, 55, 25), width=3)


def gate(x, y):
  ox, oy = cell(x, y)
  for px in (2, 13):
    d.rectangle([ox + px, oy + 3, ox + px + 1, oy + 14], fill=(120, 80, 40))
  for py in (5, 10):
    d.rectangle([ox + 3, oy + py, ox + 12, oy + py + 1], fill=(160, 110, 60))


def bandage(x, y):
  ox, oy = cell(x, y)
  d.rectangle([ox + 2, oy + 5, ox + 13, oy + 10], fill=(240, 235, 220))
  d.rectangle([ox + 7, oy + 4, ox + 8, oy + 11], fill=(200, 60, 60))
  d.rectangle([ox + 5, oy + 7, ox + 10, oy + 8], fill=(200, 60, 60))


def structure_part(x, y):
  # Intentionally empty, draws nothing
  pass


def cooked_carrot(x, y):
  ox, oy = cell(x, y)
  d.polygon([(ox + 3, oy + 4), (ox + 13, oy + 9), (ox + 4, oy + 8)],
            fill=(200, 100, 30))
  d.line([(ox + 4, oy + 6), (ox + 10, oy + 8)], fill=(120, 60, 20))


campfire(0, 0, True)
campfire(1, 0, False)
wolf(2, 0, 0)
wolf(3, 0, 1)
spear(4, 0)
pelt(5, 0)
coat(6, 0)
cooked_egg(7, 0)
stew(8, 0)
campfire_item(9, 0)
gate(10, 0)
bandage(11, 0)
structure_part(12, 0)
cooked_carrot(13, 0)

img.save(OUT)
print("Wrote " + OUT)
