cmake_minimum_required (VERSION 3.20)

add_library(ram SHARED ram.cpp)
target_include_directories(ram PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

include(FetchContent)
FetchContent_Declare(
  model_utils
  GIT_REPOSITORY https://github.com/MT-PROTEUS-PROJECT/model_utils
  GIT_TAG        cb9fe66c89cade316b8a6ed0afadf9c05b901702
)
FetchContent_MakeAvailable(model_utils)

target_link_libraries(ram PRIVATE model_utils)

set_property(TARGET ram PROPERTY CXX_STANDARD 23)
