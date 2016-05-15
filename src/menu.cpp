#include "menu.h"


menu::menu()
{
    state = MAIN_MENU;
}

menu::~menu()
{
    //dtor
}

void menu::load_data(){
    if (!( menu_image = load_bitmap("images/title_screen.png", NULL)))
        abort_on_error("Cannot find image images/title_screen.png\nPlease check your files and try again");

    if (!( help_image = load_bitmap("images/help.png", NULL)))
        abort_on_error("Cannot find image images/help.png\nPlease check your files and try again");

    if (!( story_image = load_bitmap("images/story.png", NULL)))
        abort_on_error("Cannot find image images/story.png\nPlease check your files and try again");


}


void menu::draw( BITMAP *tempBitmap){
    if(state==MAIN_MENU){
        draw_sprite( tempBitmap, menu_image, 0, 0);
    }
}

void menu::update(){


}
