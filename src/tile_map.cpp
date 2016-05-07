#include "tile_map.h"

tile_map::tile_map()
{
    x = 0;
    y = 0;
}

tile_map::~tile_map()
{
    //dtor
}

// Draw tiles
void tile_map::draw( BITMAP *tempBuffer){
    // Draw tiles
    for( unsigned int i = 0; i < map_tiles.size(); i++){
        map_tiles.at(i).draw( map_buffer);
    }

    blit( map_buffer, tempBuffer, x, y, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
}

// Generate map
void tile_map::generate_map(){
    for( int i = 0; i < MAP_WIDTH; i++){
        for( int t = 0; t < MAP_HEIGHT; t++){
            if( i < -54){
                tile newTile( i * 16, t * 16, tile_images[1], tile_images[1], 1);
                map_tiles.push_back( newTile);
            }
            //Well...............
            else if( i == 5 && t == 2){
              tile newTile( i * 16, t * 16, tile_images[3], tile_images[3], 3);
              map_tiles.push_back( newTile);
            }
            else{
              tile newTile( i * 16, t * 16, tile_images[0], tile_images[0], 0);
              map_tiles.push_back( newTile);
            }
        }
    }

    // Place items
    for( unsigned int i = 0; i < map_tiles.size(); i++){
        if( random( 0, 20) == 0){
            place_item( map_tiles.at( i).x, map_tiles.at( i).y, random( 0, 2));
        }
    }

    map_buffer = create_bitmap( MAP_WIDTH * 16, MAP_HEIGHT * 16);
}

// Load images
void tile_map::load_images(){
    if (!( tile_images[0] = load_bitmap("images/grass.png", NULL)))
        abort_on_error("Cannot find image images/grass.png\nPlease check your files and try again");

    if (!( tile_images[1] = load_bitmap("images/water.png", NULL)))
        abort_on_error("Cannot find image images/water.png\nPlease check your files and try again");

    if (!( tile_images[2] = load_bitmap("images/soil.png", NULL)))
        abort_on_error("Cannot find image images/soil.png\nPlease check your files and try again");

    if (!( tile_images[3] = load_bitmap("images/well.png", NULL)))
        abort_on_error("Cannot find image images/well.png\nPlease check your files and try again");


    if (!( item_images[0] = load_bitmap("images/hoe.png", NULL)))
        abort_on_error("Cannot find image images/hoe.png\nPlease check your files and try again");
    if (!( item_images[1] = load_bitmap("images/scythe.png", NULL)))
        abort_on_error("Cannot find image images/scythe.png\nPlease check your files and try again");
    if (!( item_images[2] = load_bitmap("images/seeds.png", NULL)))
        abort_on_error("Cannot find image images/seeds.png\nPlease check your files and try again");
}

// Replace tile on map
void tile_map::replace_tile( int tileX, int tileY, int newID){
    for( unsigned int i = 0; i < map_tiles.size(); i++){
        if( map_tiles.at(i).x == tileX && map_tiles.at(i).y == tileY){
            map_tiles.at(i).id = newID;
            map_tiles.at(i).image[0] = tile_images[newID];
        }
    }
}

// Place item on map
bool tile_map::place_item( int tileX, int tileY, int newItemID){
    for( unsigned int i = 0; i < map_tiles.size(); i++){
        if( map_tiles.at(i).x == tileX && map_tiles.at(i).y == tileY && map_tiles.at(i).item_id == -1){
            map_tiles.at(i).item_id = newItemID;
            map_tiles.at(i).item_image = item_images[newItemID];
            return true;
        }
    }
    return false;
}

// Get tile at position
char tile_map::get_tile_at( int positionX, int positionY){
    for( unsigned int i = 0; i < map_tiles.size(); i++){
        if( map_tiles.at(i).x == positionX && map_tiles.at(i).y == positionY){
            return map_tiles.at(i).id;
        }
    }
    return -1;
}

// Get item at position
char tile_map::get_item_at( int positionX, int positionY){
    for( unsigned int i = 0; i < map_tiles.size(); i++){
        if( map_tiles.at(i).x == positionX && map_tiles.at(i).y == positionY){
            return map_tiles.at(i).item_id;
        }
    }
    return -1;
}

// Scroll
void tile_map::scroll( int player_x, int player_y){
    // Scroll x
    if( (player_x + 64) - x > VIEWPORT_WIDTH && x + VIEWPORT_WIDTH < MAP_WIDTH * 16)
        x += 2;
    else if( (player_x - 64) - x < 0 && x > 0)
        x -= 2;

    // Scroll y
    if( (player_y + 48) - y > VIEWPORT_HEIGHT && y + VIEWPORT_HEIGHT < MAP_HEIGHT * 16)
        y += 2;
    else if( (player_y - 48) - y < 0 && y > 0)
        y -= 2;

}
