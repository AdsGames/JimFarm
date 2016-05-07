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

    // Draw items
    for( unsigned int i = 0; i < map_items.size(); i++){
        map_items.at(i).draw( map_buffer);
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

    for( int i = 0; i < MAP_WIDTH; i++){
        for( int t = 0; t < MAP_HEIGHT; t++){
            // Place items

            // Grass
            if( random( 0, 4) == 0){
                char temp_item_id = 3;
                item newItem( i * 16, t * 16, item_images[temp_item_id], item_images[temp_item_id], temp_item_id, item_names[temp_item_id]);
                place_item( newItem);
            }
            // Goods
            if( random( 0, 100) == 0){
                char temp_item_id = random( 0, 2);
                item newItem( i * 16, t * 16, item_images[temp_item_id], item_images[temp_item_id], temp_item_id, item_names[temp_item_id]);
                place_item( newItem);
            }

            map_buffer = create_bitmap( MAP_WIDTH * 16, MAP_HEIGHT * 16);
        }
    }
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
    item_names[0] = "hoe";

    if (!( item_images[1] = load_bitmap("images/scythe.png", NULL)))
        abort_on_error("Cannot find image images/scythe.png\nPlease check your files and try again");
    item_names[1] = "scythe";

    if (!( item_images[2] = load_bitmap("images/seeds.png", NULL)))
        abort_on_error("Cannot find image images/seeds.png\nPlease check your files and try again");
    item_names[2] = "seeds";

    if (!( item_images[3] = load_bitmap("images/dense_grass.png", NULL)))
        abort_on_error("Cannot find image images/dense_grass.png\nPlease check your files and try again");
    item_names[3] = "dense grass";
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
void tile_map::place_item( item newItem){
    map_items.push_back( newItem);
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

// CHeck if item exists
bool tile_map::is_item_at( int positionX, int positionY){
    for( unsigned int i = 0; i < map_items.size(); i++){
        if( map_items.at(i).x == positionX && map_items.at(i).y == positionY){
            return true;
        }
    }
    return false;
}

// Get item at position
item *tile_map::get_item_at( int positionX, int positionY){
    for( unsigned int i = 0; i < map_items.size(); i++){
        if( map_items.at(i).x == positionX && map_items.at(i).y == positionY){
            return &map_items.at(i);
        }
    }
    return NULL;
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
