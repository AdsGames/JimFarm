project (JimFarm)
cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)

if(EMSCRIPTEN)
  set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE /home/allan/github/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake)
  set(DVCPKG_TARGET_TRIPLET wasm32-emscripten)
endif(EMSCRIPTEN)

# Source code
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/lib/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h ${CMAKE_CURRENT_SOURCE_DIR}/lib/*.h)

add_executable (${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)

target_include_directories(
  ${PROJECT_NAME} PRIVATE
  lib/asw
)


# Find libs
if(NOT EMSCRIPTEN)
  find_package(SDL2 CONFIG REQUIRED) 
  target_link_libraries(${PROJECT_NAME} PRIVATE $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>)

  find_package(sdl2-gfx CONFIG REQUIRED)
  target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2_gfx)

  find_package(SDL2_image CONFIG REQUIRED)
  target_link_libraries(${PROJECT_NAME} PRIVATE $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)

  find_package(SDL2_mixer CONFIG REQUIRED)
  target_link_libraries(${PROJECT_NAME} PRIVATE $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>)

  find_package(SDL2_ttf CONFIG REQUIRED)
  target_link_libraries(${PROJECT_NAME} PRIVATE $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>)
endif(NOT EMSCRIPTEN)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

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
    -sUSE_SDL_GFX=2 
    -sSDL2_IMAGE_FORMATS=["png"]
  )
  target_link_libraries(
    ${PROJECT_NAME} 
    -sWASM=1 
    -sUSE_SDL=2 
    -sUSE_SDL_IMAGE=2 
    -sUSE_SDL_TTF=2 
    -sUSE_SDL_MIXER=2 
    -sUSE_SDL_GFX=2 
    -sSDL2_IMAGE_FORMATS=["png"] 
    -sDEMANGLE_SUPPORT=1
    -sTOTAL_MEMORY=512MB
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
    target_link_libraries(${PROJECT_NAME} PRIVATE -lmingw32)
  endif(MINGW)
  target_link_libraries(${PROJECT_NAME} PRIVATE -lm)
endif(EMSCRIPTEN)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/assets/ DESTINATION ${CMAKE_BINARY_DIR}/build/assets/)