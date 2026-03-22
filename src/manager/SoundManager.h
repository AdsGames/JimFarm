/*
  Sound Manager
  Allan Legemaate
  06/12/17
  This loads all the types sounds to be called by item use etc...
*/

#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <asw/asw.h>
#include <map>
#include <string>
#include <vector>

/**
 * Wrapper for sample including all parameters
 * That would be passed to asw::sound::play along with
 * Additional, frequency randomization
 */
class SampleWrapper {
 public:
  SampleWrapper(asw::Sample sample_ptr = nullptr,
                float vol = 1.0f,
                float pan = 0.5f,
                int freq = 1000,
                int freq_rand = 0,
                bool loop = false);

  asw::Sample sample_ptr;
  float vol;
  float pan;
  int freq;
  int freq_rand;
  bool loop;
};

/**
 * Sound manager class loads, from xml, all samples
 * and allows play. Works with SampleWrapper.
 */
class SoundManager {
 public:
  // Load tile types
  static int load(const std::string& path);

  // Play sample
  static void play(unsigned int sound_id);

  // Play sample by name from sounds.json
  static void play(const std::string& name);

 private:
  // List of sounds
  static std::vector<SampleWrapper> sound_defs;
  static std::map<std::string, unsigned int> sound_names;
};

#endif  // SOUND_MANAGER_H
