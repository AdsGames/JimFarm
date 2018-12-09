#include "SoundManager.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

#include "Tools.h"

// CTOR for sample wrapper
SampleWrapper::SampleWrapper (SAMPLE *sample_ptr, int vol, int pan, int freq, int freq_rand, bool loop) {
  this -> sample_ptr = sample_ptr;
  this -> vol = vol;
  this -> pan = pan;
  this -> freq = freq;
  this -> freq_rand = freq_rand;
  this -> loop = loop;
}

// List of sounds
std::vector<SampleWrapper*> SoundManager::sound_defs;

/*
 * Delete samples from memory on destroy
 */
SoundManager::~SoundManager() {
  for (unsigned int i = 0; i < sound_defs.size(); i++)
    destroy_sample (sound_defs.at(i) -> sample_ptr);

  sound_defs.clear();
}

/*
 * Load sounds from file
 * Errors: 1 File Not Found,
 */
int SoundManager::load (std::string newFile) {
  // Open file or abort if it does not exist
  std::ifstream file(newFile.c_str());
  if (!file)
    return 1;

  // Create buffer
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string content(buffer.str());

  // Get first node
  rapidxml::xml_document<> doc;
  doc.parse<0>(&content[0]);
  rapidxml::xml_node<> *allSounds = doc.first_node();

  // Define iterator
  rapidxml::xml_node<> *cSound;
  cSound = allSounds -> first_node("sound");

  // Parse sounds
  for (; cSound != NULL; cSound = cSound -> next_sibling()) {
    std::string file = "sfx/";
    file += cSound-> first_node("file") -> value();

    int volume = atoi (cSound -> first_node("volume") -> value());
    int panning = atoi (cSound -> first_node("panning") -> value());
    int frequency = atoi (cSound -> first_node("frequency") -> value());
    int frequency_rand = atoi (cSound -> first_node("frequency_rand") -> value());

    SAMPLE *tempSample = load_sample_ex (file.c_str());
    SampleWrapper *tempWrapper = new SampleWrapper (tempSample, volume, panning, frequency, frequency_rand, false);
    sound_defs.push_back (tempWrapper);
  }

  // Close
  file.close();
  return 0;
}

// Play sample
void SoundManager::play (unsigned int sound_id) {
  if (sound_id < sound_defs.size()) {
    // Frequency randomization if requested
    int freq_modulator = random (-sound_defs.at(sound_id) -> freq_rand, sound_defs.at(sound_id) -> freq_rand);
    play_sample (sound_defs.at(sound_id) -> sample_ptr, sound_defs.at(sound_id) -> vol,
                 sound_defs.at(sound_id) -> pan, sound_defs.at(sound_id) -> freq + freq_modulator,
                 sound_defs.at(sound_id) -> loop);
  }
}
