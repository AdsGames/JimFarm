/*
  Tile Type
  Allan Legemaate
  24/11/15
  Defenitions of the tiles ingame
*/
#ifndef TILE_TYPE_H
#define TILE_TYPE_H

#include <allegro.h>
#include <string>

class tile_type{
  public:
    tile_type( std::string newName, unsigned char newType, BITMAP *defaultImage, unsigned char newAttribute = 0, char newRandomness = 0);
    virtual ~tile_type();

    // Load image from file
    void setImages( std::string path1, std::string path2);

    // Get type
    char getType(){ return type; }

    // Get name
    std::string getName(){ return name; }

    // Get model
    unsigned char getRandomness(){ return randomness; }

    // Get type
    unsigned char getAttribute(){ return attribute; }

    // Tex
    BITMAP *getImage(){ return image[0]; }

    // Draw
    void draw( int x, int y, bool newTick);

    // Logic
    void logic();

  protected:
  private:
    unsigned char type;
    std::string name;
    BITMAP *image[2];
    unsigned char attribute;
    char randomness;
};

#endif // TILE_TYPE_H
