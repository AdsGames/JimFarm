#include "SoundManager.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

#include "../utility/Tools.h"

// CTOR for sample wrapper
SampleWrapper::SampleWrapper(SAMPLE* sample_ptr,
                             int vol,
                             int pan,
                             int freq,
                             int freq_rand,
                             bool loop) {
  this->sample_ptr = sample_ptr;
  this->vol = vol;
  this->pan = pan;
  this->freq = freq;
  this->freq_rand = freq_rand;
  this->loop = loop;
}

// List of sounds
std::vector<SampleWrapper*> SoundManager::sound_defs;

/*
 * Delete samples from memory on destroy
 */
SoundManager::~SoundManager() {
  for (auto const& sound : sound_defs) {
    destroy_sample(sound->sample_ptr);
  }

  sound_defs.clear();
}

/*
 * Load sounds from file
 * Errors: 1 File Not Found,
 */
int SoundManager::load(std::string path) {
  // Open file or abort if it does not exist
  std::ifstream file(path);
  if (!file.is_open()) {
    return 1;
  }

  // Create buffer
  nlohmann::json doc = nlohmann::json::parse(file);

  // Parse data
  for (auto const& sound : doc) {
    std::string file = "sfx/";
    file += sound["file"];

    int volume = sound["volume"];
    int panning = sound["panning"];
    int frequency = sound["frequency"];
    int frequency_rand = sound["frequency_rand"];

    SAMPLE* tempSample = load_sample_ex(file.c_str());
    SampleWrapper* tempWrapper = new SampleWrapper(
        tempSample, volume, panning, frequency, frequency_rand, false);
    sound_defs.push_back(tempWrapper);
  }

  // Close
  file.close();
  return 0;
}

// Play sample
void SoundManager::play(unsigned int sound_id) {
  if (sound_id < sound_defs.size()) {
    // Frequency randomization if requested
    int freq_modulator = random(-sound_defs.at(sound_id)->freq_rand,
                                sound_defs.at(sound_id)->freq_rand);
    play_sample(sound_defs.at(sound_id)->sample_ptr,
                sound_defs.at(sound_id)->vol, sound_defs.at(sound_id)->pan,
                sound_defs.at(sound_id)->freq + freq_modulator,
                sound_defs.at(sound_id)->loop);
  }
}
