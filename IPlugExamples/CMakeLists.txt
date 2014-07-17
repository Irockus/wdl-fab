set(IPC_SA   app_wrapper/app_dlg_main.cpp ../../WDL/IPlug/IPlugStandalone.cpp)
set(IPC_AU   ../../WDL/IPlug/IPlugAU.cpp   ../../WDL/IPlug/dfx/dfx-au-utilities.c)
set(IPC_VST	 ../../WDL/IPlug/IPlugVST.cpp)
set(IPC_VST3 ../../WDL/IPlug/IPlugVST3.cpp)
set(IPC_RTAS ../../WDL/IPlug/IPlugRTAS.cpp)
set(IPC_AAX  ../../WDL/IPlug/IPlugAAX.cpp)

if(WIN32)
 set (IPLUG_LIBS ${IPLUG_LIBS} asio)
elseif(APPLE)

 find_library(AUDIO_UNIT AudioUnit)
 find_library(AUDIO_TOOLBOX AudioToolbox)
 if (NOT AUDIO_UNIT)
    message(FATAL_ERROR "AudioUnit not found")
 endif()
 if (NOT AUDIO_TOOLBOX)
    message(FATAL_ERROR "AudioToolbox not found")
 endif()
 
 find_library(CARBON Carbon)
 find_library(COCOA Cocoa)
 find_library(CORE_FND CoreFoundation)
 find_library(CORE_SVCS CoreServices)
 find_library(CORE_MIDI CoreMidi)
 find_library(CORE_AUDIO CoreAudio)
 find_library(APPKIT AppKit)

 set (OSX_FMWKS ${AUDIO_UNIT} ${AUDIO_TOOLBOX} ${CORE_MIDI} ${CORE_AUDIO} 
 	${COCOA} ${CARBON} ${CORE_FND} ${CORE_SVCS} ${APPKIT} )


endif(WIN32)

#-------------------------------------------------------------------------------------
MACRO(set_bundle a_project a_pname an_ext)
  if(APPLE)
	set_target_properties( ${ARGV0} PROPERTIES
    	BUNDLE 1
    	BUNDLE_EXTENSION ${ARGV2} 
    	XCODE_ATTRIBUTE_WRAPPER_EXTENSION ${ARGV2}  #sets the extension to .${an_ext}
    	XCODE_ATTRIBUTE_MACH_O_TYPE mh_bundle
#    	XCODE_ATTRIBUTE_INFOPLIST_FILE ${ARGV1}
    	MACOSX_BUNDLE_INFO_PLIST ${ARGV1}
    )
    set_source_files_properties( ${ARGV1} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    add_custom_command(
    	TARGET ${ARGV0}
    	POST_BUILD
    	COMMAND cp
    	ARGS "${PROJECT_BINARY_DIR}/../../WDL/IPlug/PkgInfo" "$<TARGET_FILE_DIR:${ARGV0}>/.."
  	)
#	message(STATUS "Post Cmd: cp ${PROJECT_BINARY_DIR}/../../WDL/IPlug/PkgInfo $<TARGET_FILE_DIR:${ARGV0}>/..")
	message(STATUS "bundle plist is ${ARGV1}")
  else()
	message(STATUS "set_bundle(${ARGV0} ${ARGV1} ${ARGV2})")

	endif()
endMACRO()

#-------------------------------------------------------------------------------------
MACRO(set_bundle_exp a_project a_pname an_ext)
	set_bundle(${ARGV0} ${ARGV1} ${ARGV2} )
	set_target_properties( ${ARGV0} PROPERTIES
	    LINK_FLAGS "-Wl,-exported_symbols_list,\"${CMAKE_CURRENT_SOURCE_DIR}/${PLUGNAME}.exp\"")
endMACRO()

#-------------------------------------------------------------------------------------
MACRO(set_plug_props a_project compile_defs)
	set_target_properties( ${ARGV0} PROPERTIES
        COMPILE_DEFINITIONS "${ARGV1}"
		OUTPUT_NAME ${PLUGNAME}
		)
	if(APPLE)
		set_target_properties( ${ARGV0} PROPERTIES
			XCODE_ATTRIBUTE_CLANG_LINK_OBJC_RUNTIME "NO"
			)
	endif()
	target_link_libraries(${ARGV0} wdl lice iplug jnet eel plush rtam png zlib ogg vorbis icst_dsp ${IPLUG_LIBS}  ${OSX_FMWKS} )

endMACRO()

#-------------------------------------------------------------------------------------
# Create a Multi Target Example Project ...
#-------------------------------------------------------------------------------------
MACRO(create_example name)
	set (PLUGNAME ${ARGV0})
	set (PN_SRC ${PLUGNAME}_SRC )
	set (PN_HDR ${PLUGNAME}_HDR )
	set (PN_RSC ${PLUGNAME}_RSC )
	set (PN_IMG ${PLUGNAME}_IMG )

	set (PLUGNAMESA   ${PLUGNAME})
	set (PLUGNAMEVST  ${PLUGNAME}VST)
#	set (PLUGNAMEVST3 ${PLUGNAME}VST3)
	set (PLUGNAMEAU   ${PLUGNAME}AU)
#	set (PLUGNAMEAAX  ${PLUGNAME}AAX)
#	set (PLUGNAMERTAS  ${PLUGNAME}RTAS)

	PROJECT ( ${PLUGNAME} )

	set(BINARY_NAME "${PLUGNAME}" )

	# Source code to build
	set(${PN_SRC} ${${PN_SRC}}   ${PLUGNAME}.cpp )
	if(WIN32)
	  set(${PN_SRC} ${${PN_SRC}} ${PLUGNAME}.rc)
	else(WIN32)
	  set(${PN_SRC} 
	  ${${PN_SRC}}  
	  ../../WDL/IPlug/IPlugCocoa.mm 
	  ../../common.xcconfig 
	  ${PLUGNAME}.xcconfig
	  )
	  ADD_DEFINITIONS(
    	-D__MACOSX_CORE__ 
    	-DSWELL_FORCE_COCOA_TEXT 
    	-DCOCOA_PREFIX=v${PLUGNAME} 
    	-DSWELL_APP_PREFIX=Swell_v${PLUGNAME}
    	)
	endif(WIN32)

	# Source code headers
	set(${PN_HDR} ${${PN_HDR}} resource.h ${PLUGNAME}.h  )

	# Add Images resources for all OSX builds
	if(APPLE)
      set(GUI_TYPE MACOSX_BUNDLE)
      source_group(Resources\\img ".*png")
      set( ${PN_SRC} ${${PN_SRC}}  ${${PN_IMG}} )
      set_source_files_properties(${${PN_IMG}} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    else()
      SET(GUI_TYPE WIN32)
	endif()
	
	#-------------------------------------------------------------------------------------
	# Add  Executable 
	#-------------------------------------------------------------------------------------
	if ( DEFINED PLUGNAMESA )
		if(APPLE)
		  set(APPICON_PATH resources/${PLUGNAME}.icns  )
	  
		  set(PLIST_PATHSA   ${CMAKE_CURRENT_SOURCE_DIR}/resources/${PLUGNAME}-OSXAPP-Info.plist )
		  set(PLIST_PATHVST  ${CMAKE_CURRENT_SOURCE_DIR}/resources/${PLUGNAME}-VST2-Info.plist )
		  set(PLIST_PATHVST3 ${CMAKE_CURRENT_SOURCE_DIR}/resources/${PLUGNAME}-VST3-Info.plist )
		  set(PLIST_PATHAU   ${CMAKE_CURRENT_SOURCE_DIR}/resources/${PLUGNAME}-AU-Info.plist )
		  set(PLIST_PATHRTAS ${CMAKE_CURRENT_SOURCE_DIR}/resources/${PLUGNAME}-RTAS-Info.plist )
		  set(PLIST_PATHAAX  ${CMAKE_CURRENT_SOURCE_DIR}/resources/${PLUGNAME}-AAX-Info.plist )

		# Resources
		  set( ${PN_RSC}
			${CMAKE_CURRENT_SOURCE_DIR}/resources/English.lproj/MainMenu.xib 
			${CMAKE_CURRENT_SOURCE_DIR}/resources/English.lproj/InfoPlist.strings
			${APPICON_PATH}
			${PLIST_PATHSA}
			)
		
			SET_SOURCE_FILES_PROPERTIES( ${${PN_RSC}} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
		endif()

	
		add_executable(${PLUGNAMESA} ${GUI_TYPE} ${${PN_SRC}} ${${PN_RSC}} ${APPICON_PATH} ${${PN_HDR}} ${IPC_SA}  ) 
		set_plug_props(${PLUGNAMESA} "SA_API=1")
	
		if(WIN32)
			 set_target_properties(${PLUGNAMESA} PROPERTIES 
			 	LINK_FLAGS_DEBUG 	"/SUBSYSTEM:WINDOWS"
				LINK_FLAGS_RELEASE 	"/SUBSYSTEM:WINDOWS")
		elseif(APPLE)
		  SET_TARGET_PROPERTIES(${PLUGNAMESA} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PLIST_PATHSA} )
		endif()
	endif()
  
	#-------------------------------------------------------------------------------------
	# Add  AudioUnit Plugin
	#-------------------------------------------------------------------------------------
	if(APPLE)
		if( DEFINED PLUGNAMEAU )
			add_library(${PLUGNAMEAU} MODULE 
				${${PN_SRC}} ${${PN_HDR}} ${PLIST_PATHAU}  ${IPC_AU} 
				"${CMAKE_CURRENT_SOURCE_DIR}/${PLUGNAME}.exp"
				)
	     	set_bundle_exp(${PLUGNAMEAU} "${PLIST_PATHAU}" "component" )
			set_plug_props(${PLUGNAMEAU} "AU_API=1")
		endif()
	endif()

	#-------------------------------------------------------------------------------------
	# Add  VST Plugin 
	#-------------------------------------------------------------------------------------
	if( DEFINED PLUGNAMEVST )
		add_library(${PLUGNAMEVST}  MODULE ${${PN_SRC}} ${${PN_HDR}} ${IPC_VST} )
        set_bundle(${PLUGNAMEVST} "${PLIST_PATHVST}" "vst" )
		set_plug_props(${PLUGNAMEVST} "VST_API=1 VST_FORCE_DEPRECATED")
	endif()

	#-------------------------------------------------------------------------------------
	# Add  VST3 Plugin 
	#-------------------------------------------------------------------------------------
	if( DEFINED PLUGNAMEVST3 )
		add_library(${PLUGNAMEVST3} MODULE ${${PN_SRC}} ${${PN_HDR}} ${IPC_VST3} )
  		set_bundle(${PLUGNAMEVST3} "${PLIST_PATHVST3}" "vst3" )
		set_plug_props(${PLUGNAMEVST3} "VST3_API=1")
		target_link_libraries(${PLUGNAMEVST3} vst3)
	endif()
	
	#-------------------------------------------------------------------------------------
	# Add  RTAS Plugin 
	#-------------------------------------------------------------------------------------
	if( DEFINED PLUGNAMERTAS )
		add_library(${PLUGNAMERTAS} MODULE ${${PN_SRC}} ${${PN_HDR}} ${IPC_RTAS} )
		set_bundle(${PLUGNAMERTAS} "${PLIST_PATHRTAS}" "dpm" )
		set_plug_props(${PLUGNAMERTAS} "RTAS_API=1")
	endif()
	
	#-------------------------------------------------------------------------------------
	# Add  AAX Plugin 
	#-------------------------------------------------------------------------------------
	if( DEFINED PLUGNAMEAAX )
		add_library(${PLUGNAMEAAX} MODULE ${${PN_SRC}} ${${PN_HDR}} ${IPC_AAX} )
		set_bundle(${PLUGNAMEAAX} "${PLIST_PATHAAX}" "aax" )
		set_plug_props(${PLUGNAMEAAX} "AAX_API=1")
	endif()
	
	#-------------------------------------------------------------------------------------
endMACRO()

add_subdirectory(IPlugControls)
add_subdirectory(IPlugConvoEngine)
add_subdirectory(IPlugEffect)
add_subdirectory(IPlugPolySynth)
add_subdirectory(IPlugResampler)
if(WIN32)
add_subdirectory(IPlugChunks)
add_subdirectory(IPlugDistortion)
add_subdirectory(IPlugEEL)
add_subdirectory(IPlugGUIResize)
add_subdirectory(IPlugHostDetect)
add_subdirectory(IPlugMonoSynth)
add_subdirectory(IPlugMouseTest)
add_subdirectory(IPlugMultiChannel)
add_subdirectory(IPlugMultiTargets)
add_subdirectory(IPlugOpenGL)
add_subdirectory(IPlugPlush)
add_subdirectory(IPlugSideChain)
add_subdirectory(IPlugText)
endif()
