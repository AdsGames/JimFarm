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

/**
 * Wrapper for sample including all parameters
 * That would be passed to play_sample along with
 * Additional, frequency randomization
 */
class SampleWrapper {
  public:
    SampleWrapper (SAMPLE *sample_ptr = NULL, int vol = 255, int pan = 128, int freq = 1000, int freq_rand = 0, bool loop = false);

    SAMPLE *sample_ptr;
    int vol;
    int pan;
    int freq;
    int freq_rand;
    bool loop;
};

/**
 * Sound manager class loads, from xml, all samples
 * and allows play. Works with SampleWrapper.
 */
class SoundManager{
  public:
    SoundManager() {};
    ~SoundManager();

    // Load tile types
    static int load (std::string newFile);

    // Play sample
    static void play (unsigned int sound_id);

  private:
    // List of sounds
    static std::vector<SampleWrapper*> sound_defs;
};

#endif // SOUND_MANAGER_H
