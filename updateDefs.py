import sys
import re
import string
import json

tile_file_in = "assets/datatiles.json"
tile_file_out = "src/manager/tile_defs.h"

item_file_in = "assets/dataitems.json"
item_file_out = "src/manager/item_defs.h"

sound_file_in = "assets/datasounds.json"
sound_file_out = "src/manager/sound_defs.h"

interface_file_in = "assets/datainterfaces.json"
interface_file_out = "src/manager/interface_defs.h"

def parse_file(file_in, file_out, prefix):
  with open(file_in) as f:
    data = json.load(f)

    out_file = open(file_out,"w") #.H OUTPUT FILE NAME

    out_file.write("#define %s_NULL -1\n" % (prefix))
    for item in data:
      name = item["name"]
      name = name.upper()
      name = "_".join(name.split())
      
      id = ""
      id = item["id"]
      
      print(name, id)
      out_file.write("#define %s_%s %s\n" % (prefix, name, id))

    out_file.close()


print("Loading tiles from " + tile_file_in)
parse_file(tile_file_in, tile_file_out, "TILE")

print("Loading items from " + item_file_in)
parse_file(item_file_in, item_file_out, "ITEM")

print("Loading sounds from " + sound_file_in)
parse_file(sound_file_in, sound_file_out, "SOUND")

print("Loading interfaces from " + interface_file_in)
parse_file(interface_file_in, interface_file_out, "INTERFACE")
