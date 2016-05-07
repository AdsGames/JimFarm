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
        const int MAP_WIDTH = 15;
        const int MAP_HEIGHT = 10;

        void draw( BITMAP *tempBuffer);

        void replace_tile( int tileX, int tileY, int newID);

        int get_tile_at( int positionX, int positionY);

        void generate_map();

        void load_images();

    protected:

    private:
        // Tiles
        std::vector<tile> map_tiles;

        BITMAP *tile_images[100];
};

#endif // TILE_MAP_H
