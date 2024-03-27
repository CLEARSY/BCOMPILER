project(RESOURCES)

set(RESOURCES_SOURCES
  p_api.cpp
  p_comm.cpp
  p_file.cpp
  p_item.cpp
  p_line.cpp
  p_mana.cpp
  p_mark.cpp
  p_string.cpp
  p_resour.cpp
  p_synt.cpp
  p_tool.cpp
  p_trace.cpp
  )

set(RESOURCES_HEADERS
  p_api.h
  p_comm.h
  p_file.h
  p_head.h
  p_item.h
  p_line.h
  p_mana.h
  p_mark.h
  p_port.h
  p_resour.h
  p_string.h
  p_synt.h
  p_tool.h
  p_trace.h
  version.h
  )

add_custom_target(RESOURCES_VERSION
  ${CMAKE_COMMAND}
    -D SRC=${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
    -D DST=${CMAKE_CURRENT_SOURCE_DIR}/version.h
    -D GIT_EXECUTABLE=${GIT_EXECUTABLE}
    -P ${BCOMPILER_SOURCE_DIR}/cmake/macros/GenerateVersionHeader.cmake
  DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
  BYPRODUCTS
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h
    )

include_directories(${BCOMP_SOURCE_DIR})

add_library(RESOURCES_LIB STATIC ${RESOURCES_SOURCES} ${RESOURCES_HEADERS})
set_target_properties(RESOURCES_LIB PROPERTIES PREFIX "lib")
set_target_properties(RESOURCES_LIB PROPERTIES OUTPUT_NAME "Resources")

add_dependencies(RESOURCES_LIB RESOURCES_VERSION)

add_definitions(-D__RESOURCE__)
