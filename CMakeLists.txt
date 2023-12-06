# CMake settings for DelayedMove

# CMake minimum version
cmake_minimum_required(VERSION 3.0)

# project name and languages
project(DelayedMove CXX)

##############################################################################

# Unicode support
add_definitions(-DUNICODE -D_UNICODE)

# DelayedMove.exe
add_executable(DelayedMove DelayedMove.cpp)
target_link_libraries(DelayedMove PRIVATE shlwapi)

##############################################################################
