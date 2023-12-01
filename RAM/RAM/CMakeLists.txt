cmake_minimum_required (VERSION 3.19)

add_library(RAM SHARED ram.cpp)
target_include_directories(RAM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

include(FetchContent)
FetchContent_Declare(
  model_utils
  GIT_REPOSITORY https://github.com/MT-PROTEUS-PROJECT/model_utils
  GIT_TAG        a7aeb233cc748c281d5087d79d43957640630c04
)
FetchContent_MakeAvailable(model_utils)

target_link_libraries(RAM PRIVATE model_utils)

set_property(TARGET RAM PROPERTY CXX_STANDARD 20)
