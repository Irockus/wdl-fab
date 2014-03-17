#TODO implement cmake portable builds for all examples
set(IPC_SA
    app_wrapper/app_dlg_main.cpp
	../../WDL/IPlug/IPlugStandalone.cpp
)

set(IPC_VST
	../../WDL/IPlug/IPlugVST.cpp
)

set(IPC_VST3
	../../WDL/IPlug/IPlugVST3.cpp
)

if(WIN32)
 set (IPLUG_LIBS ${IPLUG_LIBS} asio)
elseif(APPLE)
 
 set(IPC_SRCS ${IPC_SRCS} ../../WDL/IPlug/IPlugCocoa.mm)


 find_library(AUDIO_UNIT AudioUnit)
 if (NOT AUDIO_UNIT)
    message(FATAL_ERROR "AudioUnit not found")
 endif()
 
 find_library(CARBON Carbon)
 find_library(COCOA Cocoa)
 find_library(CORE_FND CoreFoundation)
 find_library(CORE_SVCS CoreServices)
 find_library(CORE_MIDI CoreMidi)
 find_library(CORE_AUDIO CoreAudio)
 find_library(APPKIT AppKit)

  set (OSX_FMWKS ${AUDIO_UNIT} ${COCOA} ${CARBON} ${CORE_FND} ${CORE_SVCS} ${CORE_MIDI} ${CORE_AUDIO} ${APPKIT} )

endif(WIN32)

add_subdirectory(IPlugControls)
add_subdirectory(IPlugText)

