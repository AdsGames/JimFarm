#! python
import sys;
import re;
import string;
import xml.etree.ElementTree as ET

# TILES
out_file = open("include/tile_defs.h","w"); #.H OUTPUT FILE NAME

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
out_file = open("include/item_defs.h","w"); #.H OUTPUT FILE NAME

tree = ET.parse('build/data/items.xml')
root = tree.getroot()
 
out_file.write( "#define ITEM_NULL -1\n")
for item in root.findall('item'):
  name = ""
  if( item.find('name').text != None):
    name = item.find('name').text
  
  name = name.upper()
  name = "_".join(name.split())
  
  id = ""
  if( item.get('id') != None):
    id = item.get('id')
  
  # print( name, id)
  out_file.write( "#define ITEM_%s %s\n" % (name, id))

out_file.close()