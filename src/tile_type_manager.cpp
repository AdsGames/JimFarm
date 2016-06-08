#include "tile_type_manager.h"

std::vector<tile_type> tile_type_manager::tile_defs;
std::vector<tile_type> tile_type_manager::item_defs;

BITMAP *tile_type_manager::sprite_sheet_tiles = NULL;
BITMAP *tile_type_manager::sprite_sheet_items = NULL;

tile_type_manager::tile_type_manager(){
  //ctor
}

tile_type_manager::~tile_type_manager(){
  //dtor
}

std::string tile_type_manager::load_script( std::string newFile){
  std::string line, lines;
  std::ifstream myfile (newFile.c_str());

  if (myfile.is_open()){
    while ( getline (myfile,line) ){
      lines += line + "\n";
    }
    myfile.close();
    return lines;
  }

  else
    std::cout << "\n ERROR: Unable to open file " + newFile + "\n\n";

  return "";
}

// Load tiles
void tile_type_manager::load( std::string newFile, bool items){
  // Load biomes from file
  rapidxml::xml_document<> doc;
  std::ifstream file;

  // Check exist
  file.open(newFile.c_str());

  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string content(buffer.str());
  doc.parse<0>(&content[0]);

  rapidxml::xml_node<> *allTiles = doc.first_node();
  rapidxml::xml_node<> *cTile;
  // Loading
  if( !items){
    std::cout << "   TILES\n-------------\n";
    cTile = allTiles-> first_node("tile");
  }
  else{
    std::cout << "   ITEMS\n-------------\n";
    cTile = allTiles-> first_node("item");
  }

  // Load tiles
  for(cTile; cTile; cTile = cTile->next_sibling()){
    // Read xml variables
    // General
    int tileID = atoi(cTile-> first_attribute("id") -> value());
    std::string name = cTile-> first_node("name") -> value();
    int image_x = atoi(cTile-> first_node("images") -> first_node("image_x") -> value());
    int image_y = atoi(cTile-> first_node("images") -> first_node("image_y") -> value());

    if( !items){
      int image_h = atoi(cTile-> first_node("images") -> first_node("image_h") -> value());
      int image_w = atoi(cTile-> first_node("images") -> first_node("image_w") -> value());
      std::string script = cTile-> first_node("script") -> value();
      int randomness = atoi(cTile-> first_node("random") -> value());
      std::string attrubite_string = cTile-> first_node("attrubite") -> value();
      int attrubite = NON_SOLID;

      // Get attrubite
      if( attrubite_string == "NON_SOLID")
        attrubite = NON_SOLID;
      else if( attrubite_string == "SOLID")
        attrubite = SOLID;

      // Process any scripts
      if( script != ""){
        script = load_script( script);
      }

      // Draw to screen (debug)
      std::cout << "-> Loading Tile:" << name << "  ID:" <<  tileID << "  ATTRIBUTE:" << attrubite_string << "  RANDOMNESS:" << randomness
                << "  X:" << image_x << "  Y:" << image_y << "  H:" << image_h << "  W:" << image_w << " Script:" << script << "\n";

      // Create tile, set variables and add it to the tile list
      tile_type newTileType( image_x, image_y, image_w, image_h, tileID, name, attrubite, 0, script);
      newTileType.setSpriteSheet( sprite_sheet_tiles);

      // Add the tile
      tile_defs.push_back( newTileType);
    }
    else{
      int value = atoi(cTile-> first_node("value") -> value());

      // Draw to screen (debug)
      std::cout << "-> Loading Items:" << name << "  ID:" <<  tileID << "  X:" << image_x << "  Y:" << image_y << "  value:" << value << "\n";

      // Create item, set variables and add it to the item list
      tile_type newTileType( image_x, image_y, 1, 1, tileID, name, 0, (unsigned char)value);
      newTileType.setSpriteSheet( sprite_sheet_items);

      // Add the tile
      item_defs.push_back( newTileType);
    }
  }

  std::cout << "\n\n";
}

tile_type *tile_type_manager::getTileByID( int tileID){
  for( unsigned int i = 0; i < tile_defs.size(); i++){
    if( tile_defs.at( i).getID() == tileID){
      return &tile_defs.at( i);
    }
  }
  return &tile_defs.at( 0);
}

tile_type *tile_type_manager::getItemByID( int tileID){
  for( unsigned int i = 0; i < item_defs.size(); i++){
    if( item_defs.at( i).getID() == tileID){
      return &item_defs.at( i);
    }
  }
  return &item_defs.at( 0);
}
