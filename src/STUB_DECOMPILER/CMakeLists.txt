project(STUB_DECOMPILER)
message(STATUS "Processing ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")

set(STUB_DECOMP_SOURCES stub.cpp)

include_directories(${BCOMP_SOURCE_DIR})

add_definitions(-D__BCOMP__ -fno-implicit-templates)

add_library(STUB_DECOMP_LIB STATIC ${STUB_DECOMP_SOURCES})
set_target_properties(STUB_DECOMP_LIB 
  PROPERTIES PREFIX "lib" OUTPUT_NAME "StubDecompiler")
  