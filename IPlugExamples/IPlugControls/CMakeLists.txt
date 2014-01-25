#
# IPlugControls executable build
#

set (PLUGNAME IPlugControls)

# Source code to build
set(IPC_SRCS
   IPlugControls.cpp
)

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
 set(IPC_SRCS ${IPC_SRCS} IPlugControls.rc)
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

# Source code headers
set(IPC_HEADERS
   ${IPC_HEADERS}
   resource.h
   IPlugControls.h
   )
 
set (PLUGNAMEVST  ${PLUGNAME}VST)
set (PLUGNAMEVST3 ${PLUGNAME}VST3)
set (PLUGNAMEAU   ${PLUGNAME}AU)
set (PLUGNAMEAAX  ${PLUGNAME}AAX)
set (PLUGNAMERTAS  ${PLUGNAME}RTAS)

#Linking against a custom library
#target_link_libraries(${PLUGNAME} wdl lice)
#set_target_properties(${PLUGNAME} PROPERTIES LINK_SEARCH_END_STATIC 1)

#Build Executable 
add_executable(${PLUGNAME} ${IPC_SRCS} ${IPC_SA} ${IPC_HEADERS} ) 
target_link_libraries(${PLUGNAME} wdl lice iplug jnet rtam png zlib ${IPLUG_LIBS}  ${OSX_FMWKS} )

add_library(${PLUGNAMEVST} SHARED ${IPC_SRCS} ${IPC_VST} ${IPC_HEADERS} )
target_link_libraries(${PLUGNAMEVST} wdl lice iplug jnet rtam png zlib ${IPLUG_LIBS}  ${OSX_FMWKS} )

add_library(${PLUGNAMEVST3} SHARED ${IPC_SRCS} ${IPC_VST3} ${IPC_HEADERS} )
target_link_libraries(${PLUGNAMEVST3} wdl lice iplug vst3 jnet rtam png zlib ${IPLUG_LIBS}  ${OSX_FMWKS} )

set_target_properties(${PLUGNAME}     PROPERTIES COMPILE_DEFINITIONS "SA_API=1")
set_target_properties(${PLUGNAMEVST}  PROPERTIES COMPILE_DEFINITIONS "VST_API=1")
set_target_properties(${PLUGNAMEVST3} PROPERTIES COMPILE_DEFINITIONS "VST3_API=1")

if(WIN32)
   set_target_properties(${PLUGNAME} PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
   set_target_properties(${PLUGNAME} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
elseif(APPLE)
   add_library(${PLUGNAMEAU} SHARED ${IPC_SRCS} ${IPC_VST} ${IPC_HEADERS} )
   target_link_libraries(${PLUGNAMEAU} wdl lice iplug jnet rtam png zlib ${IPLUG_LIBS} ${OSX_FMWKS} )
   set_target_properties(${PLUGNAMEAU}  PROPERTIES COMPILE_DEFINITIONS "AU_API=1")
endif(WIN32)

