#! python
import sys;
import re;
import string;
import xml.etree.ElementTree as ET

# TILES
out_file = open("src/manager/tile_defs.h","w"); #.H OUTPUT FILE NAME

tree = ET.parse('build/data/tiles.xml')
root = tree.getroot()

out_file.write( "#define TILE_NULL -1\n")
for tile in root.findall('tile'):
  name = ""
  if( tile.find('name').text != None):
    name = tile.find('name').text
  
  name = name.upper()
  name = "_".join(name.split())
  
  id = ""
  if( tile.get('id') != None):
    id = tile.get('id')
  
  # print( name, id)
  out_file.write( "#define TILE_%s %s\n" % (name, id))

out_file.close()


# ITEMS
out_file = open("src/manager/item_defs.h","w"); #.H OUTPUT FILE NAME

tree = ET.parse('build/data/items.xml')
root = tree.getroot()
 
out_file.write( "#define ITEM_NULL -1\n")
id = 0
for item in root.findall('item'):
  name = ""
  if( item.find('name').text != None):
    name = item.find('name').text
  
  name = name.upper()
  name = "_".join(name.split())
  
  # print( name, id)
  out_file.write( "#define ITEM_%s %d\n" % (name, id))
  id += 1;

out_file.close()


# SOUNDS
out_file = open("src/manager/sound_defs.h","w"); #.H OUTPUT FILE NAME

tree = ET.parse('build/data/sounds.xml')
root = tree.getroot()
 
out_file.write( "#define SOUND_NULL -1\n")
id = 0
for sound in root.findall('sound'):
  name = ""
  if( sound.find('name').text != None):
    name = sound.find('name').text
  
  name = name.upper()
  name = "_".join(name.split())
  
  # print( name, id)
  out_file.write( "#define SOUND_%s %s\n" % (name, id))
  id += 1;

out_file.close()

# INTERFACES
out_file = open("src/manager/interface_defs.h","w"); #.H OUTPUT FILE NAME

tree = ET.parse('build/data/interfaces.xml')
root = tree.getroot()
 
out_file.write( "#define INTERFACE_NULL -1\n")
id = 0
for interface in root.findall('interface'):
  name = ""
  if( interface.find('name').text != None):
    name = interface.find('name').text
  
  name = name.upper()
  name = "_".join(name.split())
  
  # print( name, id)
  out_file.write( "#define INTERFACE_%s %s\n" % (name, id))
  id += 1;

out_file.close()