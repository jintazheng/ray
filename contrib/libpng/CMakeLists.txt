project(libpng C)

SET(LIB_NAME libpng)

INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/zlib)

FILE(GLOB_RECURSE HEADER_LIST *.h)
AUX_SOURCE_DIRECTORY(. SOURCE_LIST)

SOURCE_GROUP("libpng" FILES ${HEADER_LIST})
SOURCE_GROUP("libpng" FILES ${SOURCE_LIST})

ADD_LIBRARY(${LIB_NAME} SHARED ${HEADER_LIST} ${SOURCE_LIST})
TARGET_LINK_LIBRARIES (${LIB_NAME} zlib)

IF(BUILD_PLATFORM_ANDROID)
    TARGET_LINK_LIBRARIES (${LIB_NAME} m)
ENDIF()

IF(MINGW OR UNIX)
   SET_TARGET_PROPERTIES(libpng PROPERTIES OUTPUT_NAME png)
ENDIF()