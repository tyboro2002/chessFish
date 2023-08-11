# CMakeList.txt : CMake project for chessFish3, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (chessFish3 "chessFish3.cpp" "chessFish3.h" "game.cpp" "game.h" "engine.cpp" "engine.h" "tests.cpp" "tests.h" "time.h" "time.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET chessFish3 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
