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


def deer(x, y, frame):
  ox, oy = cell(x, y)
  body = (150, 100, 60)
  light = (200, 160, 110)
  d.rectangle([ox + 3, oy + 6, ox + 11, oy + 10], fill=body)
  d.rectangle([ox + 11, oy + 3, ox + 14, oy + 7], fill=body)
  d.point((ox + 13, oy + 4), fill=(20, 20, 20))
  # Antlers
  d.line([(ox + 12, oy + 3), (ox + 11, oy + 0)], fill=light)
  d.line([(ox + 13, oy + 3), (ox + 14, oy + 0)], fill=light)
  d.rectangle([ox + 3, oy + 6, ox + 4, oy + 7], fill=(240, 240, 230))
  legs = (4, 6, 9, 11) if frame == 0 else (5, 5, 10, 10)
  for lx in legs:
    d.line([(ox + lx, oy + 11), (ox + lx, oy + 14)], fill=(100, 65, 40))


def rabbit(x, y, frame):
  ox, oy = cell(x, y)
  body = (170, 160, 150)
  dy = 0 if frame == 0 else -2
  d.ellipse([ox + 4, oy + 8 + dy, ox + 11, oy + 13 + dy], fill=body)
  d.ellipse([ox + 9, oy + 6 + dy, ox + 13, oy + 10 + dy], fill=body)
  d.line([(ox + 10, oy + 6 + dy), (ox + 10, oy + 2 + dy)], fill=body, width=2)
  d.line([(ox + 12, oy + 6 + dy), (ox + 13, oy + 2 + dy)], fill=body, width=2)
  d.point((ox + 12, oy + 8 + dy), fill=(20, 20, 20))
  d.ellipse([ox + 3, oy + 9 + dy, ox + 5, oy + 11 + dy], fill=(250, 250, 250))


def roof(x, y):
  ox, oy = cell(x, y)
  d.rectangle([ox, oy, ox + 15, oy + 15], fill=(130, 60, 45))
  for row in range(0, 16, 4):
    d.line([(ox, oy + row + 3), (ox + 15, oy + row + 3)], fill=(95, 40, 30))
    shift = 0 if (row // 4) % 2 == 0 else 4
    for col in range(shift, 16, 8):
      d.line([(ox + col, oy + row), (ox + col, oy + row + 3)],
             fill=(95, 40, 30))


def meat(x, y, cooked):
  ox, oy = cell(x, y)
  color = (150, 85, 45) if cooked else (200, 70, 80)
  d.ellipse([ox + 2, oy + 4, ox + 12, oy + 12], fill=color)
  d.line([(ox + 11, oy + 10), (ox + 14, oy + 13)], fill=(240, 235, 220),
         width=2)


def hide(x, y):
  ox, oy = cell(x, y)
  d.polygon([(ox + 2, oy + 3), (ox + 13, oy + 3), (ox + 14, oy + 13),
             (ox + 1, oy + 13)], fill=(170, 120, 70))
  d.point((ox + 5, oy + 7), fill=(210, 170, 120))
  d.point((ox + 10, oy + 9), fill=(210, 170, 120))


def mushroom(x, y):
  ox, oy = cell(x, y)
  d.rectangle([ox + 7, oy + 8, ox + 9, oy + 13], fill=(235, 225, 200))
  d.ellipse([ox + 3, oy + 3, ox + 13, oy + 10], fill=(200, 60, 50))
  d.point((ox + 6, oy + 5), fill=(250, 250, 250))
  d.point((ox + 10, oy + 6), fill=(250, 250, 250))


def cactus_fruit(x, y):
  ox, oy = cell(x, y)
  d.ellipse([ox + 4, oy + 4, ox + 12, oy + 13], fill=(220, 60, 120))
  d.point((ox + 7, oy + 7), fill=(250, 200, 220))
  d.line([(ox + 8, oy + 4), (ox + 8, oy + 2)], fill=(80, 150, 70))


def reed(x, y):
  ox, oy = cell(x, y)
  for rx in (5, 8, 11):
    d.line([(ox + rx, oy + 14), (ox + rx - 1, oy + 3)], fill=(170, 160, 90))
  d.rectangle([ox + 4, oy + 2, ox + 5, oy + 5], fill=(120, 80, 40))


def herb(x, y):
  ox, oy = cell(x, y)
  d.line([(ox + 8, oy + 14), (ox + 8, oy + 4)], fill=(60, 130, 60))
  for ly in (5, 8, 11):
    d.ellipse([ox + 3, oy + ly, ox + 7, oy + ly + 2], fill=(90, 180, 90))
    d.ellipse([ox + 9, oy + ly - 1, ox + 13, oy + ly + 1], fill=(90, 180, 90))


def clam(x, y, cooked):
  ox, oy = cell(x, y)
  color = (230, 200, 150) if cooked else (180, 170, 190)
  d.pieslice([ox + 2, oy + 3, ox + 14, oy + 15], 180, 360, fill=color)
  for rx in (5, 8, 11):
    d.line([(ox + 8, oy + 9), (ox + rx, oy + 4)], fill=(120, 110, 130))


def clay(x, y):
  ox, oy = cell(x, y)
  d.ellipse([ox + 2, oy + 5, ox + 14, oy + 13], fill=(170, 110, 80))
  d.ellipse([ox + 4, oy + 6, ox + 9, oy + 9], fill=(195, 135, 100))


def brick(x, y):
  ox, oy = cell(x, y)
  d.rectangle([ox + 2, oy + 5, ox + 14, oy + 11], fill=(180, 70, 50))
  d.line([(ox + 2, oy + 8), (ox + 14, oy + 8)], fill=(120, 45, 35))


def glass(x, y):
  ox, oy = cell(x, y)
  d.rectangle([ox + 3, oy + 3, ox + 12, oy + 12], fill=(170, 220, 240, 180),
              outline=(230, 250, 255))
  d.line([(ox + 5, oy + 10), (ox + 10, oy + 5)], fill=(255, 255, 255))


def ore(x, y):
  ox, oy = cell(x, y)
  d.polygon([(ox + 3, oy + 12), (ox + 5, oy + 5), (ox + 10, oy + 3),
             (ox + 13, oy + 9), (ox + 11, oy + 13)], fill=(225, 225, 225))
  d.point((ox + 7, oy + 7), fill=(255, 255, 255))
  d.point((ox + 10, oy + 10), fill=(255, 255, 255))


def ingot(x, y):
  ox, oy = cell(x, y)
  d.polygon([(ox + 2, oy + 12), (ox + 4, oy + 6), (ox + 13, oy + 6),
             (ox + 14, oy + 12)], fill=(230, 230, 230))
  d.line([(ox + 5, oy + 7), (ox + 12, oy + 7)], fill=(255, 255, 255))


def pickaxe(x, y):
  ox, oy = cell(x, y)
  d.line([(ox + 4, oy + 14), (ox + 10, oy + 4)], fill=(140, 90, 40), width=2)
  d.arc([ox + 2, oy + 1, ox + 15, oy + 11], 200, 340, fill=(200, 200, 205),
        width=2)


def bench(x, y, top, legs):
  ox, oy = cell(x, y)
  d.rectangle([ox + 1, oy + 5, ox + 14, oy + 8], fill=top)
  d.rectangle([ox + 2, oy + 9, ox + 3, oy + 14], fill=legs)
  d.rectangle([ox + 12, oy + 9, ox + 13, oy + 14], fill=legs)
  d.rectangle([ox + 4, oy + 3, ox + 7, oy + 5], fill=(150, 150, 160))
  d.line([(ox + 9, oy + 4), (ox + 12, oy + 2)], fill=(120, 80, 40))


def kiln_item(x, y):
  ox, oy = cell(x, y)
  d.rectangle([ox + 3, oy + 3, ox + 12, oy + 14], fill=(160, 90, 60))
  d.rectangle([ox + 6, oy + 9, ox + 9, oy + 13], fill=(40, 25, 20))
  d.point((ox + 7, oy + 11), fill=(250, 150, 40))


def bed_item(x, y):
  ox, oy = cell(x, y)
  d.rectangle([ox + 1, oy + 6, ox + 14, oy + 12], fill=(120, 80, 45))
  d.rectangle([ox + 2, oy + 6, ox + 13, oy + 9], fill=(200, 60, 60))
  d.rectangle([ox + 2, oy + 6, ox + 5, oy + 9], fill=(240, 240, 240))


def bed_tile(x, y):
  ox, oy = cell(x, y)
  d.rectangle([ox + 2, oy + 1, ox + 13, oy + 15], fill=(120, 80, 45))
  d.rectangle([ox + 3, oy + 2, ox + 12, oy + 5], fill=(240, 240, 240))
  d.rectangle([ox + 3, oy + 6, ox + 12, oy + 14], fill=(200, 60, 60))
  d.line([(ox + 3, oy + 7), (ox + 12, oy + 7)], fill=(230, 100, 100))


def door_item(x, y):
  ox, oy = cell(x, y)
  d.rectangle([ox + 4, oy + 1, ox + 11, oy + 14], fill=(140, 90, 50))
  d.point((ox + 10, oy + 8), fill=(230, 200, 80))


def window_item(x, y):
  ox, oy = cell(x, y)
  d.rectangle([ox + 3, oy + 3, ox + 12, oy + 12], fill=(170, 220, 240),
              outline=(130, 85, 45))
  d.line([(ox + 8, oy + 3), (ox + 8, oy + 12)], fill=(130, 85, 45))
  d.line([(ox + 3, oy + 8), (ox + 12, oy + 8)], fill=(130, 85, 45))


def floor(x, y, color, line, item):
  ox, oy = cell(x, y)
  if item:
    d.rectangle([ox + 2, oy + 4, ox + 13, oy + 12], fill=color)
    d.line([(ox + 2, oy + 8), (ox + 13, oy + 8)], fill=line)
    return
  d.rectangle([ox, oy, ox + 15, oy + 15], fill=color)
  for row in (3, 7, 11, 15):
    d.line([(ox, oy + row), (ox + 15, oy + row)], fill=line)
  for row, col in ((0, 6), (4, 11), (8, 3), (12, 9)):
    d.line([(ox + col, oy + row), (ox + col, oy + row + 3)], fill=line)


def sprinkler(x, y):
  ox, oy = cell(x, y)
  d.rectangle([ox + 7, oy + 6, ox + 8, oy + 14], fill=(200, 200, 205))
  d.ellipse([ox + 4, oy + 3, ox + 11, oy + 7], fill=(215, 130, 80))
  for dx, dy in ((-3, -1), (3, -1), (0, -3)):
    d.point((ox + 8 + dx, oy + 3 + dy), fill=(120, 170, 255))


def scarecrow_item(x, y):
  ox, oy = cell(x, y)
  d.line([(ox + 8, oy + 4), (ox + 8, oy + 15)], fill=(120, 80, 40), width=2)
  d.line([(ox + 2, oy + 7), (ox + 14, oy + 7)], fill=(120, 80, 40), width=2)
  d.ellipse([ox + 5, oy + 0, ox + 11, oy + 6], fill=(230, 200, 120))
  d.polygon([(ox + 4, oy + 1), (ox + 12, oy + 1), (ox + 8, oy - 1)],
            fill=(120, 80, 40))


def scarecrow_tile(x, y):
  ox, oy = cell(x, y)
  oy += 16
  d.line([(ox + 8, oy - 6), (ox + 8, oy + 15)], fill=(120, 80, 40), width=2)
  d.line([(ox + 1, oy + 1), (ox + 15, oy + 1)], fill=(120, 80, 40), width=2)
  d.rectangle([ox + 5, oy - 1, ox + 11, oy + 8], fill=(90, 120, 170))
  d.ellipse([ox + 5, oy - 10, ox + 11, oy - 3], fill=(230, 200, 120))
  d.rectangle([ox + 4, oy - 12, ox + 12, oy - 10], fill=(150, 110, 50))
  d.point((ox + 7, oy - 7), fill=(20, 20, 20))
  d.point((ox + 9, oy - 7), fill=(20, 20, 20))


def hopper(x, y):
  ox, oy = cell(x, y)
  d.polygon([(ox + 1, oy + 4), (ox + 14, oy + 4), (ox + 11, oy + 14),
             (ox + 4, oy + 14)], fill=(160, 110, 60))
  d.line([(ox + 1, oy + 4), (ox + 14, oy + 4)], fill=(215, 130, 80), width=2)
  d.ellipse([ox + 6, oy + 5, ox + 10, oy + 9], fill=(245, 240, 225))


def ore_rock(x, y, fleck):
  ox, oy = cell(x, y)
  d.polygon([(ox + 1, oy + 15), (ox + 2, oy + 6), (ox + 7, oy + 2),
             (ox + 13, oy + 4), (ox + 15, oy + 15)], fill=(120, 120, 125))
  for fx, fy in ((5, 8), (10, 6), (8, 11), (12, 12), (4, 12)):
    d.rectangle([ox + fx, oy + fy, ox + fx + 1, oy + fy + 1], fill=fleck)


def cactus(x, y):
  ox, oy = cell(x, y)
  green = (70, 150, 70)
  d.rounded_rectangle([ox + 6, oy + 4, ox + 10, oy + 31], 2, fill=green)
  d.rounded_rectangle([ox + 1, oy + 12, ox + 4, oy + 20], 2, fill=green)
  d.line([(ox + 4, oy + 19), (ox + 6, oy + 19)], fill=green, width=2)
  d.rounded_rectangle([ox + 12, oy + 9, ox + 15, oy + 17], 2, fill=green)
  d.line([(ox + 10, oy + 16), (ox + 12, oy + 16)], fill=green, width=2)
  d.ellipse([ox + 6, oy + 1, ox + 10, oy + 5], fill=(220, 60, 120))


def kiln_tile(x, y):
  ox, oy = cell(x, y)
  d.polygon([(ox + 2, oy + 31), (ox + 3, oy + 10), (ox + 8, oy + 4),
             (ox + 13, oy + 10), (ox + 14, oy + 31)], fill=(160, 90, 60))
  d.rectangle([ox + 6, oy + 0, ox + 10, oy + 5], fill=(120, 70, 50))
  d.pieslice([ox + 4, oy + 18, ox + 12, oy + 30], 180, 360, fill=(40, 25, 20))
  d.rectangle([ox + 4, oy + 24, ox + 12, oy + 30], fill=(40, 25, 20))
  d.ellipse([ox + 6, oy + 25, ox + 10, oy + 29], fill=(240, 130, 40))


def door_tile(x, y):
  ox, oy = cell(x, y)
  d.rectangle([ox + 1, oy + 4, ox + 14, oy + 31], fill=(140, 90, 50),
              outline=(100, 60, 30))
  d.line([(ox + 1, oy + 12), (ox + 14, oy + 12)], fill=(110, 70, 35))
  d.line([(ox + 1, oy + 22), (ox + 14, oy + 22)], fill=(110, 70, 35))
  d.ellipse([ox + 11, oy + 17, ox + 13, oy + 19], fill=(230, 200, 80))


def window_tile(x, y):
  ox, oy = cell(x, y)
  d.rectangle([ox, oy + 4, ox + 15, oy + 31], fill=(150, 105, 60))
  d.rectangle([ox + 3, oy + 8, ox + 12, oy + 20], fill=(170, 220, 240),
              outline=(100, 60, 30))
  d.line([(ox + 7, oy + 8), (ox + 7, oy + 20)], fill=(100, 60, 30))
  d.line([(ox + 4, oy + 17), (ox + 6, oy + 10)], fill=(255, 255, 255))


def brick_wall(x, y):
  ox, oy = cell(x, y)
  d.rectangle([ox, oy + 4, ox + 15, oy + 31], fill=(180, 70, 50))
  for row in range(4, 32, 4):
    d.line([(ox, oy + row), (ox + 15, oy + row)], fill=(120, 45, 35))
    shift = 0 if (row // 4) % 2 == 0 else 4
    for col in range(shift, 16, 8):
      d.line([(ox + col, oy + row), (ox + col, oy + row + 3)],
             fill=(120, 45, 35))


def reeds_tile(x, y):
  ox, oy = cell(x, y)
  for rx, top in ((3, 3), (6, 1), (9, 4), (12, 2)):
    d.line([(ox + rx, oy + 15), (ox + rx, oy + top)], fill=(120, 150, 70))
    d.rectangle([ox + rx - 1, oy + top, ox + rx, oy + top + 3],
                fill=(120, 80, 40))


def herb_tile(x, y):
  ox, oy = cell(x, y)
  for hx in (4, 11):
    d.line([(ox + hx, oy + 15), (ox + hx, oy + 7)], fill=(60, 130, 60))
    d.ellipse([ox + hx - 3, oy + 7, ox + hx, oy + 10], fill=(90, 180, 90))
    d.ellipse([ox + hx, oy + 9, ox + hx + 3, oy + 12], fill=(90, 180, 90))
  d.point((ox + 4, oy + 6), fill=(230, 230, 120))


def clam_tile(x, y):
  ox, oy = cell(x, y)
  d.pieslice([ox + 4, oy + 8, ox + 12, oy + 16], 180, 360,
             fill=(180, 170, 190))
  d.line([(ox + 8, oy + 12), (ox + 8, oy + 8)], fill=(120, 110, 130))


def clay_tile(x, y):
  ox, oy = cell(x, y)
  d.rectangle([ox, oy, ox + 15, oy + 15], fill=(165, 110, 80))
  for fx, fy in ((3, 3), (10, 5), (6, 11), (13, 12)):
    d.point((ox + fx, oy + fy), fill=(190, 135, 100))


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

# Row 1: animals, roof, food and materials
deer(0, 1, 0)
deer(1, 1, 1)
rabbit(2, 1, 0)
rabbit(3, 1, 1)
roof(4, 1)
meat(5, 1, False)
meat(6, 1, True)
hide(7, 1)
mushroom(8, 1)
cactus_fruit(9, 1)
reed(10, 1)
herb(11, 1)
clam(12, 1, False)
clam(13, 1, True)
clay(14, 1)
brick(15, 1)

# Row 2: materials, tools and placeable items
glass(0, 2)
ore(1, 2)
ingot(2, 2)
pickaxe(3, 2)
bench(4, 2, (160, 110, 60), (120, 80, 40))
bench(5, 2, (150, 150, 155), (110, 110, 115))
kiln_item(6, 2)
bed_item(7, 2)
door_item(8, 2)
window_item(9, 2)
floor(10, 2, (170, 120, 70), (120, 80, 45), True)
floor(11, 2, (180, 70, 50), (120, 45, 35), True)
sprinkler(12, 2)
scarecrow_item(13, 2)
hopper(14, 2)

# Row 3: one tile high tiles
mushroom(0, 3)
herb_tile(1, 3)
clam_tile(2, 3)
clay_tile(3, 3)
floor(4, 3, (170, 120, 70), (120, 80, 45), False)
floor(5, 3, (180, 70, 50), (120, 45, 35), False)
bench(6, 3, (160, 110, 60), (120, 80, 40))
bed_tile(7, 3)
sprinkler(8, 3)
hopper(9, 3)
ore_rock(10, 3, (215, 130, 80))
ore_rock(11, 3, (200, 205, 220))
bench(12, 3, (150, 150, 155), (110, 110, 115))
reeds_tile(13, 3)

# Rows 4-5: two tiles high tiles
cactus(0, 4)
kiln_tile(1, 4)
door_tile(2, 4)
window_tile(3, 4)
brick_wall(4, 4)
scarecrow_tile(5, 4)

img.save(OUT)
print("Wrote " + OUT)
