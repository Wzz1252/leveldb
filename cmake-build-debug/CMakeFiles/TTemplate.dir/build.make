# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/torment/Workspace/CLionProjects/leveldb

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/torment/Workspace/CLionProjects/leveldb/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/ttemplate.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ttemplate.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ttemplate.dir/flags.make

CMakeFiles/ttemplate.dir/util/testutil.cc.o: CMakeFiles/ttemplate.dir/flags.make
CMakeFiles/ttemplate.dir/util/testutil.cc.o: ../util/testutil.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/torment/Workspace/CLionProjects/leveldb/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/ttemplate.dir/util/testutil.cc.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ttemplate.dir/util/testutil.cc.o -c /Users/torment/Workspace/CLionProjects/leveldb/util/testutil.cc

CMakeFiles/ttemplate.dir/util/testutil.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ttemplate.dir/util/testutil.cc.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/torment/Workspace/CLionProjects/leveldb/util/testutil.cc > CMakeFiles/ttemplate.dir/util/testutil.cc.i

CMakeFiles/ttemplate.dir/util/testutil.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ttemplate.dir/util/testutil.cc.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/torment/Workspace/CLionProjects/leveldb/util/testutil.cc -o CMakeFiles/ttemplate.dir/util/testutil.cc.s

CMakeFiles/ttemplate.dir/test/class/ttemplate.cpp.o: CMakeFiles/ttemplate.dir/flags.make
CMakeFiles/ttemplate.dir/test/class/ttemplate.cpp.o: ../test/class/ttemplate.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/torment/Workspace/CLionProjects/leveldb/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/ttemplate.dir/test/class/ttemplate.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ttemplate.dir/test/class/ttemplate.cpp.o -c /Users/torment/Workspace/CLionProjects/leveldb/test/class/ttemplate.cpp

CMakeFiles/ttemplate.dir/test/class/ttemplate.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ttemplate.dir/test/class/ttemplate.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/torment/Workspace/CLionProjects/leveldb/test/class/ttemplate.cpp > CMakeFiles/ttemplate.dir/test/class/ttemplate.cpp.i

CMakeFiles/ttemplate.dir/test/class/ttemplate.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ttemplate.dir/test/class/ttemplate.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/torment/Workspace/CLionProjects/leveldb/test/class/ttemplate.cpp -o CMakeFiles/ttemplate.dir/test/class/ttemplate.cpp.s

# Object files for target ttemplate
ttemplate_OBJECTS = \
"CMakeFiles/ttemplate.dir/util/testutil.cc.o" \
"CMakeFiles/ttemplate.dir/test/class/ttemplate.cpp.o"

# External object files for target ttemplate
ttemplate_EXTERNAL_OBJECTS =

ttemplate: CMakeFiles/ttemplate.dir/util/testutil.cc.o
ttemplate: CMakeFiles/ttemplate.dir/test/class/ttemplate.cpp.o
ttemplate: CMakeFiles/ttemplate.dir/build.make
ttemplate: libleveldb.a
ttemplate: lib/libgmockd.a
ttemplate: lib/libgtestd.a
ttemplate: CMakeFiles/ttemplate.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/torment/Workspace/CLionProjects/leveldb/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable ttemplate"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ttemplate.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ttemplate.dir/build: ttemplate

.PHONY : CMakeFiles/ttemplate.dir/build

CMakeFiles/ttemplate.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ttemplate.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ttemplate.dir/clean

CMakeFiles/ttemplate.dir/depend:
	cd /Users/torment/Workspace/CLionProjects/leveldb/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/torment/Workspace/CLionProjects/leveldb /Users/torment/Workspace/CLionProjects/leveldb /Users/torment/Workspace/CLionProjects/leveldb/cmake-build-debug /Users/torment/Workspace/CLionProjects/leveldb/cmake-build-debug /Users/torment/Workspace/CLionProjects/leveldb/cmake-build-debug/CMakeFiles/ttemplate.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ttemplate.dir/depend
