#include "tile_map.h"

tile_map::tile_map()
{
    //ctor
}

tile_map::~tile_map()
{
    //dtor
}

// Draw tiles
void tile_map::draw( BITMAP *tempBuffer){
    // Draw tiles
    for( int i = 0; i < map_tiles.size(); i++){
        map_tiles.at(i).draw( tempBuffer);
    }
}

// Generate map
void tile_map::generate_map(){
    for( int i = 0; i < MAP_WIDTH; i++){
        for( int t = 0; t < MAP_HEIGHT; t++){
            if( i > 6){
                tile newTile( i * 16, t * 16, tile_images[1], tile_images[1], 1);
                map_tiles.push_back( newTile);
            }
            else{
                //Well...............
                if(i==5 && t==2){
                  tile newTile( i * 16, t * 16, tile_images[3], tile_images[3], 3);
                  map_tiles.push_back( newTile);

                }else{
                  tile newTile( i * 16, t * 16, tile_images[0], tile_images[0], 0);
                  map_tiles.push_back( newTile);
                }

            }
        }
    }
}

// Load images
void tile_map::load_images(){
    if (!( tile_images[0] = load_bitmap("images/grass.png", NULL)))
        abort_on_error("Cannot find image images/grass.png\nPlease check your files and try again");

    if (!( tile_images[1] = load_bitmap("images/water.png", NULL)))
        abort_on_error("Cannot find image images/water.png\nPlease check your files and try again");

    if (!( tile_images[2] = load_bitmap("images/tile1.png", NULL)))
        abort_on_error("Cannot find image images/tile2.png\nPlease check your files and try again");

    if (!( tile_images[3] = load_bitmap("images/well.png", NULL)))
        abort_on_error("Cannot find image images/well.png\nPlease check your files and try again");
}

// Replace tile on map
void tile_map::replace_tile( int tileX, int tileY, int newID){
    for( int i = 0; i < map_tiles.size(); i++){
        if( map_tiles.at(i).x == tileX && map_tiles.at(i).y == tileY){
            map_tiles.at(i).id = newID;
            map_tiles.at(i).image[0] = tile_images[newID];
        }
    }
}

// Get tile at position
int tile_map::get_tile_at( int positionX, int positionY){
    for( int i = 0; i < map_tiles.size(); i++){
        if( map_tiles.at(i).x == positionX && map_tiles.at(i).y == positionY){
            return map_tiles.at(i).id;
        }
    }
}
