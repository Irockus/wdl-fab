#
# ICST DSP Library 1.2 cmakefile build
#

SET(DSP_SRC
 AudioAnalysis.cpp
 AudioFile.cpp
 AudioSynth.cpp
 BlkDsp.cpp
 Chart.cpp
 Neuro.cpp
 SpecMath.cpp
 fftoourad.cpp
 fftoouraf.cpp
)

SET(DSP_HEADERS
 AudioAnalysis.h
 AudioFile.h
 AudioSynth.h
 BlkDsp.h
 Chart.h
 Common.h
 CritSect.h
 MathDefs.h
 Neuro.h
 SpecMath.h
 SpecMathInline.h
 fftooura.h
)

if(NOT WIN32)
	set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
endif(WIN32)

add_library(icst_dsp ${DSP_SRC} ${DSP_HEADERS} )

