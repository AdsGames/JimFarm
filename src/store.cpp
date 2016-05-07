#include "store.h"


store::store()
{
    open = false;
}

store::~store()
{
    //dtor
}

void store::load_images(){
    if (!( image = load_bitmap("images/GUI_STORE.png", NULL)))
        abort_on_error("Cannot find image images/GUI_STORE.png\nPlease check your files and try again");

    if (!( indicator = load_bitmap("images/indicator.png", NULL)))
        abort_on_error("Cannot find image images/indicator.png\nPlease check your files and try again");
}

void store::draw( BITMAP *tempBitmap){
    if( open){
        draw_sprite( tempBitmap, image, 0, 0);

        for( int i = 0; i < storeItems.size(); i++){
            if( storeItems.at(i) -> image[0] != NULL)
                draw_sprite( tempBitmap, storeItems.at(i) -> image[0], 25 + i * 19, 90);
                if( selector_index == i)
                    draw_sprite( tempBitmap, indicator, 25 + i * 19, 90);
        }
    }
}

void store::update(){
    if( key[KEY_LEFT] && selector_index > 0)
        selector_index--;
    else if( key[KEY_RIGHT] && selector_index < 7)
        selector_index++;
}

void store::toggle(){
    open =! open;
}
