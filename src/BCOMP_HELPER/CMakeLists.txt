project(BCOMP_HELPER)

set(BCOMP_HELPER_SOURCES
  BCOMPLoader.cpp
  MessageHandler.cpp
  NodeVisitor.cpp)

set(BCOMP_HELPER_HEADERS
  BCOMPLoader.h
  MessageHandler.h
  NodeVisitor.h
)

include_directories(${BCOMP_SOURCE_DIR})

add_library(BCOMP_HELPER_LIB
  STATIC ${BCOMP_HELPER_SOURCES} ${BCOMP_HELPER_HEADERS})
set_target_properties(BCOMP_HELPER_LIB PROPERTIES PREFIX "lib")
set_target_properties(BCOMP_HELPER_LIB PROPERTIES OUTPUT_NAME "BCompHelper")
