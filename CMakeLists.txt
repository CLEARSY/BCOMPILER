cmake_minimum_required(VERSION 3.7)
project(BCOMPILER)

set(CMAKE_CXX_STANDARD 11)

# recherche des bibliothèques statiques
if(NOT ICU_FOUND)
  find_package(ICU REQUIRED COMPONENTS dt in uc data)
endif()

# recherche de git pour récupérer tag et sha1
if(NOT GIT_EXECUTABLE)
  find_package(Git REQUIRED)
endif()

if(WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	set(WINMM -lwinmm)
else()
	set(WINMM "")
endif()

if(CMAKE_COMPILER_IS_GNU_CXX)
    add_definitions(-fno-implicit-templates)
endif(CMAKE_COMPILER_IS_GNU_CXX)

add_subdirectory(src/BCOMP)
add_subdirectory(src/STUB_DECOMPILER)
add_subdirectory(src/BCOMP_HELPER)
add_subdirectory(src/RESOURCES)
add_subdirectory(src/BXML)
