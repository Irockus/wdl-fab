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

MACRO(create_example name)
	set (PLUGNAME ${ARGV0})

	# Source code to build
	set(IPC_SRCS ${PLUGNAME}.cpp )
	if(WIN32)
	 set(IPC_SRCS ${IPC_SRCS} ${PLUGNAME}.rc)
	endif(WIN32)

	# Source code headers
	set(IPC_HEADERS ${IPC_HEADERS} resource.h ${PLUGNAME}.h  )
	 
	set (PLUGNAMEVST  ${PLUGNAME}VST)
#	set (PLUGNAMEVST3 ${PLUGNAME}VST3)
	set (PLUGNAMEAU   ${PLUGNAME}AU)
#	set (PLUGNAMEAAX  ${PLUGNAME}AAX)
#	set (PLUGNAMERTAS  ${PLUGNAME}RTAS)

	#Build Executable 
	add_executable(${PLUGNAME} ${IPC_SRCS} ${IPC_SA} ${IPC_HEADERS} ) 
	target_link_libraries(${PLUGNAME} wdl lice iplug jnet eel plush rtam png zlib ${IPLUG_LIBS}  ${OSX_FMWKS} )
	set_target_properties(${PLUGNAME}     PROPERTIES COMPILE_DEFINITIONS "SA_API=1")
	
	if( DEFINED PLUGNAMEVST )
		add_library(${PLUGNAMEVST} SHARED ${IPC_SRCS} ${IPC_VST} ${IPC_HEADERS} )
		target_link_libraries(${PLUGNAMEVST} wdl lice iplug jnet eel plush rtam png zlib ${IPLUG_LIBS}  ${OSX_FMWKS} )
		set_target_properties(${PLUGNAMEVST}  PROPERTIES COMPILE_DEFINITIONS "VST_API=1")
	endif()

	if( DEFINED PLUGNAMEVST3 )
		add_library(${PLUGNAMEVST3} SHARED ${IPC_SRCS} ${IPC_VST3} ${IPC_HEADERS} )
		target_link_libraries(${PLUGNAMEVST3} wdl lice iplug vst3 jnet eel plush rtam png zlib ${IPLUG_LIBS}  ${OSX_FMWKS} )
		set_target_properties(${PLUGNAMEVST3} PROPERTIES COMPILE_DEFINITIONS "VST3_API=1")
	endif()
	
	if(WIN32)
	   set_target_properties(${PLUGNAME} PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
	   set_target_properties(${PLUGNAME} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
	elseif(APPLE)
		if( DEFINED PLUGNAMEAU )
			add_library(${PLUGNAMEAU} SHARED ${IPC_SRCS} ${IPC_VST} ${IPC_HEADERS} )
			target_link_libraries(${PLUGNAMEAU} wdl lice iplug jnet eel plush rtam png zlib ${IPLUG_LIBS} ${OSX_FMWKS} )
			set_target_properties(${PLUGNAMEAU}  PROPERTIES COMPILE_DEFINITIONS "AU_API=1")
		endif()
	endif(WIN32)
endMACRO()

add_subdirectory(IPlugChunks)
add_subdirectory(IPlugControls)
add_subdirectory(IPlugConvoEngine)
add_subdirectory(IPlugDistortion)
add_subdirectory(IPlugEEL)
add_subdirectory(IPlugEffect)
add_subdirectory(IPlugGUIResize)
add_subdirectory(IPlugHostDetect)
add_subdirectory(IPlugMonoSynth)
add_subdirectory(IPlugMouseTest)
add_subdirectory(IPlugMultiChannel)
add_subdirectory(IPlugMultiTargets)
add_subdirectory(IPlugOpenGL)
add_subdirectory(IPlugPlush)
add_subdirectory(IPlugPolySynth)
add_subdirectory(IPlugResampler)
add_subdirectory(IPlugSideChain)
add_subdirectory(IPlugText)

