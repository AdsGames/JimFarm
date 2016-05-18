#include "tile_type_manager.h"

tile_type_manager::tile_type_manager(){
  //ctor
}

tile_type_manager::~tile_type_manager(){
  //dtor
}

// Load tiles
void tile_type_manager::load( std::string newFile){
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

  // Loading
  std::cout << "   TILES\n-------------\n";

  // Load tiles
  for(rapidxml::xml_node<> *cTile=allTiles-> first_node("tile"); cTile; cTile=cTile->next_sibling()){
    // Read xml variables
    // General
    int tileID = atoi(cTile-> first_attribute("id") -> value());
    std::string name = cTile-> first_node("name") -> value();
    std::string image_x = cTile-> first_node("images") -> first_node("image_x") -> value();
    std::string image_y = cTile-> first_node("images") -> first_node("image_y") -> value();
    std::string image_h = cTile-> first_node("images") -> first_node("image_h") -> value();
    std::string image_w = cTile-> first_node("images") -> first_node("image_w") -> value();
    int randomness = atoi(cTile-> first_node("random") -> value());

    std::string attrubite_string = cTile-> first_node("attrubite") -> value();
    int attrubite = NON_SOLID;

    // Get attrubite
    if( attrubite_string == "NON_SOLID")
      attrubite = NON_SOLID;
    else if( attrubite_string == "SOLID")
      attrubite = SOLID;

    // Draw to screen (debug)
    std::cout << "-> Loading Tile:" << name << "  ID:" <<  tileID << "  ATTRIBUTE:" << attrubite_string << "  RANDOMNESS:" << randomness << "\n";

    // Create tile, set variables and add it to the tile list
    //tile newTileType( 0, 0, image_x, image_y, tileID);

    // Add the tile
    //tile_defs.push_back( newTileType);
  }

  std::cout << "\n\n";
}

tile_type *tile_type_manager::getTileByType( int tileID){
  return &tile_defs.at( tileID);
}
