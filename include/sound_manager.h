/*
  Sound Manager
  Allan Legemaate
  06/12/17
  This loads all the types sounds to be called by item use etc...
*/

#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <allegro.h>
#include <string>
#include <vector>

class sample_wrapper{
  public:
    sample_wrapper( SAMPLE *sample_ptr = NULL, int vol = 255, int pan = 128, int freq = 1000, int freq_rand = 0, bool loop = false);

    SAMPLE *sample_ptr;
    int vol;
    int pan;
    int freq;
    int freq_rand;
    bool loop;
};

class sound_manager{
  public:
    sound_manager() {};
    virtual ~sound_manager() {};

    // Load tile types
    static void load( std::string newFile);

    // Play sample
    static void play( unsigned int sound_id);

  protected:
  private:
    static std::vector<sample_wrapper*> sound_defs;
};

#endif // SOUND_MANAGER_H
