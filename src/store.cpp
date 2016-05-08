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
        if( customer_inventory != NULL)
            draw_sprite( tempBitmap, customer_inventory -> inventory_item -> image[0], 185, 90);

        if( selector_index == storeItems.size())
            draw_sprite( tempBitmap, indicator, 185, 90);
    }
}

void store::update(){
    if( open){
        if( key[KEY_LEFT] && selector_index > 0){
            selector_index--;
            rest( 100);
        }
        else if( key[KEY_RIGHT] && selector_index < storeItems.size()){
            selector_index++;
            rest( 100);
        }


        if( key[KEY_ENTER]){
            if( selector_index < storeItems.size()){
                if( customer_inventory -> inventory_item -> id == -1 && customer_inventory -> money >= storeItems.at(selector_index) -> value){
                    customer_inventory -> money -= storeItems.at(selector_index) -> value;
                    customer_inventory -> give_item( storeItems.at(selector_index) -> id);
                    storeItems.erase(storeItems.begin() + selector_index);
                }
            }
            else if( selector_index == storeItems.size() && customer_inventory -> inventory_item -> id != -1){
                customer_inventory -> money += customer_inventory -> inventory_item -> value;
                customer_inventory -> remove_item();
            }
            rest( 200);
        }
    }
}

void store::close_store(){
    open = false;
}

void store::open_store( character *new_inventory){
    open = true;
    customer_inventory = new_inventory;
}
