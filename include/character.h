#ifndef CHARACTER_H
#define CHARACTER_H

#include <allegro.h>



class character
{
    public:
        character();
        virtual ~character();

        void setPosition( int newX, int newY){ x = newX; y = newY;}
        void setImage( BITMAP *newImage){ image = newImage;}

        void draw( BITMAP *tempBuffer);

        void update();
    protected:

    private:
        int x, y;
        BITMAP *image;

        char gameTick;
        char direction;
};

#endif // CHARACTER_H
