cmake_minimum_required (VERSION 3.20)

add_library(ram SHARED ram.cpp)
target_include_directories(ram PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

include(FetchContent)
FetchContent_Declare(
  model_utils
  GIT_REPOSITORY https://github.com/MT-PROTEUS-PROJECT/model_utils
  GIT_TAG        e9febc0ee740e430d902356103021b935fd1e380
)
FetchContent_MakeAvailable(model_utils)

target_link_libraries(ram PRIVATE model_utils)

set_property(TARGET ram PROPERTY CXX_STANDARD 23)
