cmake_minimum_required(VERSION 3.24)

project(JimFarm VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/target)
set(SANITIZE  OFF)

include(cmake/get_cpm.cmake)
CPMAddPackage("gh:adsgames/asw@0.12.3")
CPMAddPackage("gh:nlohmann/json@3.11.3")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
target_link_libraries(${PROJECT_NAME} PRIVATE 
  asw
  nlohmann_json::nlohmann_json
)

if (SANITIZE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
  set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")
endif()

asw_add_web_target(${PROJECT_NAME}
  TITLE "Jim Farm"
  BACKGROUND "#000000"
  ASSETS ${CMAKE_CURRENT_LIST_DIR}/assets
)

if(EMSCRIPTEN)
  target_link_options(${PROJECT_NAME} PRIVATE
    -sMAXIMUM_MEMORY=1gb
    --use-preload-plugins
  )
endif()

file(COPY ${CMAKE_CURRENT_LIST_DIR}/assets/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)