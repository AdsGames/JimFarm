#include "sound_manager.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "tools.h"

// CTOR for sample wrapper
sample_wrapper::sample_wrapper( SAMPLE *sample_ptr, int vol, int pan, int freq, int freq_rand, bool loop){
  this -> sample_ptr = sample_ptr;
  this -> vol = vol;
  this -> pan = pan;
  this -> freq = freq;
  this -> freq_rand = freq_rand;
  this -> loop = loop;
}

std::vector<sample_wrapper*> sound_manager::sound_defs;

// Load sounds from file
void sound_manager::load( std::string newFile){
  rapidxml::xml_document<> doc;
  std::ifstream file;

  // Check exist
  file.open(newFile.c_str());

  // Create buffer
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string content(buffer.str());
  doc.parse<0>(&content[0]);

  // Get first node and def iterator
  rapidxml::xml_node<> *allSounds = doc.first_node();
  rapidxml::xml_node<> *cSound;

  // Loading
  std::cout << "   SOUNDS\n-------------\n";
  cSound = allSounds -> first_node("sound");

  // Load tiles
  for( ; cSound != NULL; cSound = cSound -> next_sibling()){
    // Read xml variables
    std::string file = "sfx/";
    file += cSound-> first_node("file") -> value();

    int volume = atoi(cSound-> first_node("volume") -> value());
    int panning = atoi(cSound-> first_node("panning") -> value());
    int frequency = atoi(cSound-> first_node("frequency") -> value());
    int frequency_rand = atoi(cSound-> first_node("frequency_rand") -> value());

    std::cout << "-> Loading sound:" << file << "  vol:" <<  volume << "  pan:" << panning << "  freq:" << frequency << "  freq rand:" << frequency_rand << "\n";

    SAMPLE *tempSample = load_sample_ex(file.c_str());
    sample_wrapper *tempWrapper = new sample_wrapper( tempSample, volume, panning, frequency, frequency_rand, false);
    sound_defs.push_back( tempWrapper);
  }

  std::cout << "\n\n";
}

// Play sample
void sound_manager::play( unsigned int sound_id){
  if( sound_id < sound_defs.size())
    play_sample( sound_defs.at(sound_id) -> sample_ptr, sound_defs.at(sound_id) -> vol,
                 sound_defs.at(sound_id) -> pan, sound_defs.at(sound_id) -> freq,
                 sound_defs.at(sound_id) -> loop);
}
