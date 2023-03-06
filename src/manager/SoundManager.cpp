#include "SoundManager.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

#include "../utility/Tools.h"

// CTOR for sample wrapper
SampleWrapper::SampleWrapper(asw::Sample sample_ptr,
                             int vol,
                             int pan,
                             int freq,
                             int freq_rand,
                             bool loop)
    : sample_ptr(sample_ptr),
      vol(vol),
      pan(pan),
      freq(freq),
      freq_rand(freq_rand),
      loop(loop) {}

// List of sounds
std::vector<SampleWrapper> SoundManager::sound_defs;

/*
 * Load sounds from file
 * Errors: 1 File Not Found,
 */
int SoundManager::load(const std::string& path) {
  // Open file or abort if it does not exist
  std::ifstream file(path);

  if (!file.is_open()) {
    return 1;
  }

  // Create buffer
  nlohmann::json doc = nlohmann::json::parse(file);

  // Parse data
  for (auto const& sound : doc) {
    std::string file = "assets/sfx/";
    file += sound["file"];

    int volume = sound["volume"];
    int panning = sound["panning"];
    int frequency = sound["frequency"];
    int frequency_rand = sound["frequency_rand"];

    asw::Sample tempSample = asw::assets::loadSample(file);
    auto tempWrapper = SampleWrapper(tempSample, volume, panning, frequency,
                                     frequency_rand, false);
    sound_defs.push_back(tempWrapper);
  }

  // Close
  file.close();
  return 0;
}

// Play sample
void SoundManager::play(unsigned int sound_id) {
  if (sound_id >= sound_defs.size()) {
    return;
  }

  const auto& sound = sound_defs.at(sound_id);

  // Frequency randomization if requested
  int freq_modulator = random(-sound.freq_rand, sound.freq_rand);

  asw::sound::play(sound.sample_ptr, sound.vol, sound.pan,
                   //  sound.freq + freq_modulator,
                   sound.loop);
}
