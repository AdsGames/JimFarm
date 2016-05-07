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

// Draw tiles
void tile_map::drawForeground( BITMAP *tempBuffer){
    rectfill( map_buffer, 0, 0, map_buffer -> w, map_buffer -> h, makecol( 255, 0, 255));

    // Draw items
    for( unsigned int i = 0; i < map_items.size(); i++){
        map_items.at(i).draw( map_buffer);
    }

    // Draw foreground
    for( unsigned int i = 0; i < map_tiles_foreground.size(); i++){
        map_tiles_foreground.at(i).draw( map_buffer);
    }

    masked_blit( map_buffer, tempBuffer, x, y, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
}

// Generate map
void tile_map::generate_map(){
    int tempMap[MAP_WIDTH][MAP_HEIGHT];
    int tempMapForeground[MAP_WIDTH][MAP_HEIGHT];

    // Generate map
    for( int i = 0; i < MAP_WIDTH; i++){
        for( int t = 0; t < MAP_HEIGHT; t++){
            tempMap[i][t] = 0;
            tempMapForeground[i][t] = 0;

            // Water
            if( i < -54){
                tempMapForeground[i][t] = 1;
            }
            //Well...............
            else if( i == 5 && t == 2){
              tempMapForeground[i][t] = 3;
            }
            // Dense grass
            else if( random( 0, 10) == 0){
                tempMapForeground[i][t] = 4;
            }
            // Trees
            else if( random( 0, 30) == 0){
                tempMapForeground[i][t] = 5;
            }
            // Bushes
            else if( random( 0, 20) == 0){
                tempMapForeground[i][t] = 6;
            }
        }
    }

    // Grow Grass
    for( int i = 0; i < MAP_WIDTH; i++){
        for( int t = 0; t < MAP_HEIGHT; t++){
            if( tempMapForeground[i][t] == 4){
                if( i > 0 && random( 0, 2) == 0){
                    tempMapForeground[i - 1][t] = 4;
                }
                if( i < MAP_WIDTH - 1 && random( 0, 2) == 0){
                    tempMapForeground[i + 1][t] = 4;
                }
                if( t > 0 && random( 0, 2) == 0){
                    tempMapForeground[i][t - 1] = 4;
                }
                if( t < MAP_HEIGHT - 1 && random( 0, 2) == 0){
                    tempMapForeground[i][t + 1] = 4;
                }
            }
        }
    }

    // Spread Trees
    for( int i = 0; i < MAP_WIDTH; i++){
        for( int t = 0; t < MAP_HEIGHT; t++){
            if( tempMapForeground[i][t] == 5){
                if( i > 0 && random( 0, 2) == 0){
                    tempMapForeground[i - 1][t] = 5;
                }
                if( i < MAP_WIDTH - 1 && random( 0, 2) == 0){
                    tempMapForeground[i + 1][t] = 5;
                }
                if( t > 0 && random( 0, 2) == 0){
                    tempMapForeground[i][t - 1] = 5;
                }
                if( t < MAP_HEIGHT - 1 && random( 0, 2) == 0){
                    tempMapForeground[i][t + 1] = 5;
                }
            }
        }
    }

    // Turn numbers into objects
    for( int i = 0; i < MAP_WIDTH; i++){
        for( int t = 0; t < MAP_HEIGHT; t++){
            tile newTile( i * 16, t * 16, tile_images[tempMap[i][t]], tile_images[tempMap[i][t]], tempMap[i][t]);
            map_tiles.push_back( newTile);

            if( tempMapForeground[i][t] != 0){
                tile newTile2( i * 16, t * 16, tile_images[tempMapForeground[i][t]], tile_images[tempMapForeground[i][t]], tempMapForeground[i][t]);
                map_tiles_foreground.push_back( newTile2);
            }
        }
    }

    // Place items
    for( int i = 0; i < MAP_WIDTH; i++){
        for( int t = 0; t < MAP_HEIGHT; t++){
            // Place items
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

    if (!( tile_images[4] = load_bitmap("images/dense_grass.png", NULL)))
        abort_on_error("Cannot find image images/dense_grass.png\nPlease check your files and try again");

    if (!( tile_images[5] = load_bitmap("images/tree.png", NULL)))
        abort_on_error("Cannot find image images/tree.png\nPlease check your files and try again");

    if (!( tile_images[6] = load_bitmap("images/bush.png", NULL)))
        abort_on_error("Cannot find image images/bush.png\nPlease check your files and try again");

    if (!( item_images[0] = load_bitmap("images/hoe.png", NULL)))
        abort_on_error("Cannot find image images/hoe.png\nPlease check your files and try again");
    item_names[0] = "hoe";

    if (!( item_images[1] = load_bitmap("images/scythe.png", NULL)))
        abort_on_error("Cannot find image images/scythe.png\nPlease check your files and try again");
    item_names[1] = "scythe";

    if (!( item_images[2] = load_bitmap("images/seeds.png", NULL)))
        abort_on_error("Cannot find image images/seeds.png\nPlease check your files and try again");
    item_names[2] = "seeds";
}

// Replace tile on map
void tile_map::replace_tile( int tileX, int tileY, int newID, bool foreground){
    if( !foreground){
        for( unsigned int i = 0; i < map_tiles.size(); i++){
            if( map_tiles.at(i).x == tileX && map_tiles.at(i).y == tileY){
                map_tiles.at(i).id = newID;
                map_tiles.at(i).image[0] = tile_images[newID];
            }
        }
    }
    else{
        for( unsigned int i = 0; i < map_tiles_foreground.size(); i++){
            if( map_tiles_foreground.at(i).x == tileX && map_tiles_foreground.at(i).y == tileY){
                map_tiles_foreground.at(i).id = newID;
                map_tiles_foreground.at(i).image[0] = tile_images[newID];
            }
        }
    }
}

// Place item on map
void tile_map::place_item( item newItem){
    map_items.push_back( newItem);
}

// Get tile at position
char tile_map::get_tile_at( int positionX, int positionY, bool foreground){
    if( !foreground){
        for( unsigned int i = 0; i < map_tiles.size(); i++){
            if( map_tiles.at(i).x == positionX && map_tiles.at(i).y == positionY){
                return map_tiles.at(i).id;
            }
        }
    }
    else{
        for( unsigned int i = 0; i < map_tiles_foreground.size(); i++){
            if( map_tiles_foreground.at(i).x == positionX && map_tiles_foreground.at(i).y == positionY){
                return map_tiles_foreground.at(i).id;
            }
        }
    }
    return -1;
}

// Check for solid tile
bool tile_map::is_solid_at( int positionX, int positionY){
    for( unsigned int i = 0; i < map_tiles_foreground.size(); i++){
        if( map_tiles_foreground.at(i).x == positionX && map_tiles_foreground.at(i).y == positionY){
            return map_tiles_foreground.at(i).solid;
        }
    }
    return false;
}


// Check if item exists
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

// Pick up item at position
void tile_map::remove_item_at( int positionX, int positionY){
    for( unsigned int i = 0; i < map_items.size(); i++){
        if( map_items.at(i).x == positionX && map_items.at(i).y == positionY){
            map_items.erase( map_items.begin() + i);
            break;
        }
    }
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
