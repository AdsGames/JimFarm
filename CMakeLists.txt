cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(DOWNLOAD_EXTRACT_TIMESTAMP ON)

include(FetchContent)

project(JimFarm)

# Source code
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)

FetchContent_Declare(
  json 
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.11.3
)
FetchContent_Declare(
  sol2
  GIT_REPOSITORY https://github.com/ThePhD/sol2.git
  GIT_TAG        v3.3.0
)
FetchContent_Declare(
  asw
  GIT_REPOSITORY https://github.com/adsGames/asw.git
  GIT_TAG        v0.4.1
)

FetchContent_MakeAvailable(json asw sol2)

# Find libs
if(NOT EMSCRIPTEN)
  find_library(SDL_LIBRARY NAMES SDL2 REQUIRED)
  find_library(SDL_MIXER_LIBRARY NAMES SDL2_mixer REQUIRED)
  find_library(SDL_IMAGE_LIBRARY NAMES SDL2_image REQUIRED)
  find_library(SDL_TTF_LIBRARY NAMES SDL2_ttf REQUIRED)
  find_library(SDL_MAIN_LIBRARY NAMES SDL2main REQUIRED)
endif(NOT EMSCRIPTEN)

# Link Libs
# Emscripten support
if(EMSCRIPTEN)
  set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "index")
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  target_compile_options(
    ${PROJECT_NAME}
    PRIVATE
    -sUSE_SDL=2
    -sUSE_SDL_IMAGE=2
    -sUSE_SDL_TTF=2
    -sUSE_SDL_MIXER=2
    -sSDL2_IMAGE_FORMATS=["png"]
  )
  target_link_libraries(
    ${PROJECT_NAME}
    -sWASM=1
    -sUSE_SDL=2
    -sUSE_SDL_IMAGE=2
    -sUSE_SDL_TTF=2
    -sUSE_SDL_MIXER=2
    -sSDL2_IMAGE_FORMATS=["png"]
    -sDEMANGLE_SUPPORT=1
    -sTOTAL_MEMORY=512MB
    nlohmann_json::nlohmann_json
    asw
  )
  set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    LINK_FLAGS
    "--preload-file ${CMAKE_CURRENT_LIST_DIR}/assets@/assets --use-preload-plugins"
  )

# Run of the mill executable
else(EMSCRIPTEN)
  if(MINGW)
    target_link_libraries(${PROJECT_NAME} -lmingw32)
  endif(MINGW)

  target_link_libraries(
    ${PROJECT_NAME}
    -lm
    ${SDL_MAIN_LIBRARY}
    ${SDL_LIBRARY}
    ${SDL_MIXER_LIBRARY}
    ${SDL_IMAGE_LIBRARY}
    ${SDL_TTF_LIBRARY}
    nlohmann_json::nlohmann_json
    asw
  )
endif(EMSCRIPTEN)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/assets/ DESTINATION ${CMAKE_BINARY_DIR}/build/assets/)