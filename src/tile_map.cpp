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

// Manually load new file
void tile_map::load_map( std::string fileName){
    //Change size
    std::string fileLoad = fileName + ".txt";
    std::ifstream findSize(fileLoad.c_str());
    MAP_WIDTH = 0;
    MAP_HEIGHT = 0;
    int data;
    while (findSize >> data) {
        if(MAP_HEIGHT == 0){
            MAP_WIDTH++;
        }
        if(findSize.peek() == '\n'){
            MAP_HEIGHT++;
        }
    }

    //Setup Map
    if(fileName != "blank"){
        map_tiles.clear();
        map_tiles_foreground.clear();

        fileLoad = fileName + ".txt";
        std::ifstream read(fileLoad.c_str());

        for( int t = 0; t < MAP_HEIGHT; t++){
            for (int i = 0; i < MAP_WIDTH; i++){
                int newTileType;
                read >> newTileType;
                std::cout << newTileType;
                // Set tile type
                tile newTile( i * 16, t * 16, tile_images[newTileType], tile_images[newTileType], newTileType);
                map_tiles.push_back( newTile);
            }
        }
        read.close();

        fileLoad = fileName + "_back.txt";
        std::ifstream read2(fileLoad.c_str());

        for( int t = 0; t < MAP_HEIGHT; t++){
            for (int i = 0; i < MAP_WIDTH; i++){
                int newTileType;
                read2 >> newTileType;
                std::cout << newTileType;
                // Set tile type
                if( newTileType != 0){
                    tile newTile( i * 16, t * 16, tile_images[newTileType], tile_images[newTileType], newTileType);
                    map_tiles_foreground.push_back( newTile);
                }
            }
        }
        read2.close();
    }
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

    load_map( "data/map");

    /*// Generate map
    for( int i = 0; i < MAP_WIDTH; i++){
        for( int t = 0; t < MAP_HEIGHT; t++){
            tempMap[i][t] = 0;
            tempMapForeground[i][t] = 0;

            // Water
            if( random( 0, 150) == 0){
                tempMapForeground[i][t] = 1;
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

    // More Pond
    for( int j = 0; j < 3; j++){
        for( int i = 0; i < MAP_WIDTH; i++){
            for( int t = 0; t < MAP_HEIGHT; t++){
                if( tempMapForeground[i][t] == 1){
                    if( i > 0 && random( 0, 1) == 0){
                        tempMapForeground[i - 1][t] = 1;
                    }
                    if( i < MAP_WIDTH - 1 && random( 0, 1) == 0){
                        tempMapForeground[i + 1][t] = 1;
                    }
                    if( t > 0 && random( 0, 1) == 0){
                        tempMapForeground[i][t - 1] = 1;
                    }
                    if( t < MAP_HEIGHT - 1 && random( 0, 1) == 0){
                        tempMapForeground[i][t + 1] = 1;
                    }
                }
            }
        }
    }*/

    //Place foreground objects
    /*tempMapForeground[5][3] = 3;
    tempMapForeground[5][4] = 0;
    tempMapForeground[10][7] = 50;
    tempMapForeground[11][7] = 51;
    tempMapForeground[12][7] = 52;
    tempMapForeground[14][9] = 54;

    //Place background items
    tempMap[5][4] = 7;

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
    }*/

    // Place items
    for( int i = 0; i < MAP_WIDTH; i++){
        for( int t = 0; t < MAP_HEIGHT; t++){
            // Place items
            // Goods
            if( random( 0, 50) == 0){
                char temp_item_id = random( 0, 5);
                item newItem( i * 16, t * 16, item_images[temp_item_id], item_images[temp_item_id], temp_item_id, item_names[temp_item_id]);
                place_item( newItem);
            }
            remove_item_at(6*16,5*16);
            item newItem( 6 * 16, 5 * 16, item_images[3], item_images[3], 3, item_names[3]);
            place_item( newItem);


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

    //tile_images[6] did 9/11
    if (!( tile_images[6] = load_bitmap("images/bush.png", NULL)))
        abort_on_error("Cannot find image images/bush.png\nPlease check your files and try again");

    if (!( tile_images[7] = load_bitmap("images/well_path.png", NULL)))
        abort_on_error("Cannot find image images/well_path.png\nPlease check your files and try again");


    if (!( tile_images[8] = load_bitmap("images/crop_2_1.png", NULL)))
        abort_on_error("Cannot find image images/crop_2_1.png\nPlease check your files and try again");

    if (!( tile_images[9] = load_bitmap("images/crop_2_2.png", NULL)))
        abort_on_error("Cannot find image images/crop_2_2.png\nPlease check your files and try again");

    if (!( tile_images[10] = load_bitmap("images/crop_2_3.png", NULL)))
        abort_on_error("Cannot find image images/crop_2_3.png\nPlease check your files and try again");

    if (!( tile_images[11] = load_bitmap("images/stump.png", NULL)))
        abort_on_error("Cannot find image images/stump.png\nPlease check your files and try again");

    // What's the difference between item_images[0] and Allan? Nothing.
    if (!( tile_images[50] = load_bitmap("images/coop_1.png", NULL)))
        abort_on_error("Cannot find image images/coop_1.png\nPlease check your files and try again");

    if (!( tile_images[51] = load_bitmap("images/coop_2.png", NULL)))
        abort_on_error("Cannot find image images/coop_2.png\nPlease check your files and try again");

    if (!( tile_images[52] = load_bitmap("images/coop_3.png", NULL)))
        abort_on_error("Cannot find image images/coop_3.png\nPlease check your files and try again");

    if (!( tile_images[53] = load_bitmap("images/coop_fence.png", NULL)))
        abort_on_error("Cannot find image images/coop_fence.png\nPlease check your files and try again");

    if (!( tile_images[54] = load_bitmap("images/store.png", NULL)))
        abort_on_error("Cannot find image images/store.png\nPlease check your files and try again");

    // What's the difference between item_images[0] and Allan? Everything!
    if (!( item_images[0] = load_bitmap("images/hoe.png", NULL)))
        abort_on_error("Cannot find image images/hoe.png\nPlease check your files and try again");
    item_names[0] = "hoe";

    if (!( item_images[1] = load_bitmap("images/scythe.png", NULL)))
        abort_on_error("Cannot find image images/scythe.png\nPlease check your files and try again");
    item_names[1] = "scythe";

    if (!( item_images[2] = load_bitmap("images/seeds.png", NULL)))
        abort_on_error("Cannot find image images/seeds.png\nPlease check your files and try again");
    item_names[2] = "seeds";

    if (!( item_images[3] = load_bitmap("images/watering_can.png", NULL)))
        abort_on_error("Cannot find image images/watering_can.png\nPlease check your files and try again");
    item_names[3] = "watering can";

    if (!( item_images[4] = load_bitmap("images/axe.png", NULL)))
        abort_on_error("Cannot find image images/axe.png\nPlease check your files and try again");
    item_names[4] = "axe";

    if (!( item_images[5] = load_bitmap("images/shovel.png", NULL)))
        abort_on_error("Cannot find image images/shovel.png\nPlease check your files and try again");
    item_names[5] = "shovel";
}

// Replace tile on map
void tile_map::replace_tile( int tileX, int tileY, int newID, bool foreground){
    if( !foreground){
        for( unsigned int i = 0; i < map_tiles.size(); i++){
            if( map_tiles.at(i).x == tileX && map_tiles.at(i).y == tileY){
                if( newID == -1){
                    map_tiles.erase( map_tiles.begin() + i);
                }
                else{
                    map_tiles.at(i).id = newID;
                    map_tiles.at(i).image[0] = tile_images[newID];
                }
            }
        }
    }
    else{
        for( unsigned int i = 0; i < map_tiles_foreground.size(); i++){
            if( map_tiles_foreground.at(i).x == tileX && map_tiles_foreground.at(i).y == tileY){
                if( newID == -1){
                    map_tiles_foreground.erase( map_tiles_foreground.begin() + i);
                }
                else{
                    map_tiles_foreground.at(i).id = newID;
                    map_tiles_foreground.at(i).image[0] = tile_images[newID];
                }
            }
        }
    }
}

// Place item on map
void tile_map::place_item( item newItem){
    map_items.push_back( newItem);
}

// Get tile at position
int tile_map::get_tile_at( int positionX, int positionY, bool foreground){
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
