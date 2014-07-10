#
# CMake config for xiph.org Ogg library (http://www.xiph.org/downloads/)
#

SET(OGG_SRC
	src/bitwise.c
	src/framing.c
 )

SET(OGG_HEADERS
	include/ogg/ogg.h
	include/ogg/os_types.h
)

if(NOT WIN32)
	set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
endif()

add_library(ogg ${OGG_SRC} ${OGG_HEADERS} )

