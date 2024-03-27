project(BXML)
message(STATUS "Processing ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")

set(BXML_SOURCES
    bxml_api.cpp
    XmlWriter.cpp
    XmlStreamWriter.cpp
    bxml.cpp
)

set(BXML_HEADERS
    bxml_api.h
    XmlWriter.h
    XmlStreamWriter.h
    version.h
)

add_custom_target(BXML_VERSION
  ${CMAKE_COMMAND}
    -D SRC=${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
    -D DST=${CMAKE_CURRENT_SOURCE_DIR}/version.h
    -D GIT_EXECUTABLE=${GIT_EXECUTABLE}
    -D PROJECT_NAME=${PROJECT_NAME}
    -P ${BCOMPILER_SOURCE_DIR}/cmake/macros/GenerateVersionHeader.cmake
  DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
  BYPRODUCTS
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h
    )

include_directories(
    ${BCOMP_SOURCE_DIR}
    ${BCOMP_HELPER_SOURCE_DIR}
    ${RESOURCES_SOURCE_DIR}
    )

# add_library(BXML STATIC ${BXML_SOURCES} ${BXML_HEADERS})
add_executable(bxml ${BXML_SOURCES} ${BXML_HEADERS})
add_dependencies(bxml BXML_VERSION)

target_link_libraries(bxml
  -Bstatic BCOMP_HELPER_LIB BCOMP_LIB RESOURCES_LIB STUB_DECOMP_LIB
  -Bdynamic ${ICU_LIBRARIES} ${CMAKE_DL_LIBS} ${WINMM})
