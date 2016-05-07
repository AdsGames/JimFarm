#ifndef TILE_MAP_H
#define TILE_MAP_H

#include<allegro.h>
#include<vector>

#include "tile.h"
#include "tools.h"



class tile_map
{
    public:
        tile_map();
        virtual ~tile_map();

        // Constant
        static const int MAP_WIDTH = 30;
        static const int MAP_HEIGHT = 20;

        static const int VIEWPORT_WIDTH = 240;
        static const int VIEWPORT_HEIGHT = 160;

        void draw( BITMAP *tempBuffer);

        void replace_tile( int tileX, int tileY, int newID);
        bool place_item( int tileX, int tileY, int newItemID);

        char get_tile_at( int positionX, int positionY);
        char get_item_at( int positionX, int positionY);

        void generate_map();

        void load_images();

        void scroll( int player_x, int player_y);

        int x, y;

    protected:

    private:
        // Tiles
        std::vector<tile> map_tiles;

        BITMAP *tile_images[100];
        BITMAP *item_images[100];

        BITMAP *map_buffer;
};

#endif // TILE_MAP_H
