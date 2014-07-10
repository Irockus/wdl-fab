#
# CMake config for xiph.org Vorbis library (http://www.xiph.org/downloads/)
#

SET(VORBIS_SRC
    lib/mdct.c
    lib/smallft.c
    lib/block.c
    lib/envelope.c
    lib/window.c
    lib/lsp.c
    lib/lpc.c
    lib/analysis.c
    lib/synthesis.c
    lib/psy.c
    lib/info.c
    lib/floor1.c
    lib/floor0.c
    lib/res0.c
    lib/mapping0.c
    lib/registry.c
    lib/codebook.c
    lib/sharedbook.c
    lib/lookup.c
    lib/bitrate.c
    lib/vorbisenc.c
    lib/vorbisfile.c
)

SET(VORBIS_HEADERS
	include/vorbis/codec.h
	include/vorbis/vorbisenc.h
	include/vorbis/vorbisfile.h
)

if(NOT WIN32)
	set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
endif()

add_library(vorbis ${VORBIS_SRC} ${VORBIS_HEADERS} )

