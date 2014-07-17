cmake_minimum_required (VERSION 2.8)

if(APPLE)

	# set the base sdk version:
	set(BUILD_SDK_VER 10.6)
	set(BUILD_SDK_MAXVER 1060)
	
	set(CMAKE_OSX_SYSROOT "macosx${BUILD_SDK_VER}")
	set(CMAKE_OSX_DEPLOYMENT_TARGET "${BUILD_SDK_VER}" )
  
#	find_library( COREFOUNDATION_FRAMEWORK NAMES CoreFoundation)
endif(APPLE)


set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
#set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} $ENV{VORBISDIR}/include )

# simple, unified lib & bin dir for all libs and exe
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# find something portable

# find_package(bzip2 required)
# find_package(png required)
# find_package(jpeg required)
# find_package(zlib required)
# find_package(OpenGL REQUIRED)
# find_package(GLU REQUIRED)
# find_package(SDL REQUIRED)

# vorbis / ogg audio encoding decoding libs
set(VORBIS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/contribs/vorbis" )
set(OGG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/contribs/ogg" )
set(VORBIS_INCLUDE_DIR "${VORBIS_DIR}/include" "${VORBIS_DIR}/lib" )
set(OGG_INCLUDE_DIR "${OGG_DIR}/include" )
set(OGG_LIBRARY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libogg.lib" )
set(VORBIS_LIBRARY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libvorbis.lib" )
	
if (WIN32)
	add_definitions(-DOS_WIN)
	add_definitions(-D__WINDOWS_ASIO__)
	add_definitions(-D__WINDOWS_DS__)
	
else(WIN32)
	add_definitions(
		-DOS_OSX
		-D__MAC_OS_X_VERSION_MAX_ALLOWED=${BUILD_SDK_MAXVER}
		-DMAC_OS_X_VERSION_MAX_ALLOWED=${BUILD_SDK_MAXVER}
		)
#	find_package(Vorbis REQUIRED)
endif(WIN32)

macro(configure_msvc_runtime)
  if(MSVC)
    # Default to statically-linked runtime.
    if("${MSVC_RUNTIME}" STREQUAL "")
      set(MSVC_RUNTIME "static")
    endif()
    
    # Set compiler options.
    set(variables
      CMAKE_C_FLAGS_DEBUG
      CMAKE_C_FLAGS_MINSIZEREL
      CMAKE_C_FLAGS_RELEASE
      CMAKE_C_FLAGS_RELWITHDEBINFO
      CMAKE_CXX_FLAGS_DEBUG
      CMAKE_CXX_FLAGS_MINSIZEREL
      CMAKE_CXX_FLAGS_RELEASE
      CMAKE_CXX_FLAGS_RELWITHDEBINFO
    )
    
    if(${MSVC_RUNTIME} STREQUAL "static")
      message(STATUS
        "MSVC -> forcing use of statically-linked runtime."
      )
      foreach(variable ${variables})
        if(${variable} MATCHES "/MD")
          string(REGEX REPLACE "/MD" "/MT" ${variable} "${${variable}}")
        endif()
      endforeach()
    else()
      message(STATUS
        "MSVC -> forcing use of dynamically-linked runtime."
      )
      foreach(variable ${variables})
        if(${variable} MATCHES "/MT")
          string(REGEX REPLACE "/MT" "/MD" ${variable} "${${variable}}")
        endif()
      endforeach()
    endif()
    
  else(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")
    set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
    set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG")
    
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse3 -mssse3")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wreturn-type -Wunused-variable -Wno-deprecated-declarations  -Wno-write-strings -Wno-reorder -msse3 -mssse3")

  endif()
endmacro()

IF(MSVC)
  ADD_DEFINITIONS(-DMSVC_RUNTIME=static)
  ADD_DEFINITIONS(/arch:SSE2)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS)
  ADD_DEFINITIONS(/wd4244 /wd4996 /wd4018 /wd4305)
  
ELSEIF(APPLE)
  SET(CMAKE_OSX_ARCHITECTURES "x86_64;i386")
  ADD_DEFINITIONS(-D__MACOSX_CORE__ -DWDL_FFT_REALSIZE=8 -DSWELL_FORCE_COCOA_TEXT -DSWELL_CLEANUP_ON_UNLOAD)
ENDIF(MSVC)

configure_msvc_runtime()
set(variables
  CMAKE_C_FLAGS_DEBUG
  CMAKE_C_FLAGS_MINSIZEREL
  CMAKE_C_FLAGS_RELEASE
  CMAKE_C_FLAGS_RELWITHDEBINFO
  CMAKE_CXX_FLAGS_DEBUG
  CMAKE_CXX_FLAGS_MINSIZEREL
  CMAKE_CXX_FLAGS_RELEASE
  CMAKE_CXX_FLAGS_RELWITHDEBINFO
)
#display build flags
message(STATUS "Initial build flags:")
foreach(variable ${variables})
  message(STATUS "  '${variable}': ${${variable}}")
endforeach()
message(STATUS "")

include_directories(
	# ${OPENAL_INCLUDES}
	# ${BZIP2_INCLUDE_DIR}
	# ${JPEG_INCLUDE_DIR}
	# ${PNG_INCLUDE_DIR}
	# ${ZLIB_INCLUDE_DIR}
	# ${OPENGL_INCLUDE_DIR}
	# ${GLU_INCLUDE_DIR}
	# ${SDL_INCLUDE_DIR}
	${VORBIS_INCLUDE_DIR}
	${OGG_INCLUDE_DIR}
	${CMAKE_SOURCE_DIR}/Source
	${CMAKE_CURRENT_SOURCE_DIR}/WDL/IPlug	
	${CMAKE_CURRENT_SOURCE_DIR}/WDL	
	${CMAKE_CURRENT_SOURCE_DIR}/WDL/rtaudiomidi	
	${CMAKE_CURRENT_SOURCE_DIR}/ASIO_SDK
	${CMAKE_CURRENT_SOURCE_DIR}/VST_SDK
	${CMAKE_CURRENT_SOURCE_DIR}/VST3_SDK
	)

IF (APPLE)
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/WDL/swell	)
ENDIF(APPLE)

set(IPLUG_LIBS 
	# ${OPENAL_LIBRARY} 
	# ${PNG_LIBRARY} 
	# ${JPEG_LIBRARY} 
	# ${BZIP2_LIBRARIES} 
	# ${ZLIB_LIBRARIES} 
	# ${SDL_LIBRARY} 
	# ${GLU_LIBRARY} 
	# ${OPENGL_LIBRARY} 
#	${OGG_LIBRARY}
#	${VORBIS_LIBRARY} 
#	${VORBISFILE_LIBRARY} 
)


# Version number
set (IPLUG_VERSION_MAJOR 0)
set (IPLUG_VERSION_MINOR 9)
set (IPLUG_VERSION_PATCH "0")
set(IPLUG_VERSION "${IPLUG_VERSION_MAJOR}.${IPLUG_VERSION_MINOR}")
set(IPLUG_VERSION_FULL "${IPLUG_VERSION}.${IPLUG_VERSION_PATCH}")

# SET
	# IPlugAAX.cpp
	# IPlugAU.cpp
	# IPlugRTAS.cpp
	# IPlugStandalone.cpp
	# IPlugStructs.cpp
	# IPlugVST.cpp
	# IPlugVST3.cpp

#Prefer static linking for plugins
set(BUILD_SHARED_LIBS OFF)  

if(WIN32)
   if(MSVC)
#      add_definitions(-DWIN32_LEAN_AND_MEAN)
      add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	  SET(IPLUG_LIBS ${IPLUG_LIBS} Winmm wsock32 Wininet)
   endif(MSVC)
endif(WIN32)

# IPlug build flags:
set (HAVE_DIGIDESIGN_AXX 	OFF)
set (HAVE_DIGIDESIGN_PT9 	OFF)
set (HAVE_VST				ON)
set (HAVE_VST3				ON)

if(APPLE)
	set (HAVE_AUDIOUNITS	ON)
else(APPLE)
	set (HAVE_AUDIOUNITS	OFF)
endif(APPLE)

# Subdirectories

#	ASIO SDK
if(WIN32)
	add_subdirectory(ASIO_SDK)
endif(WIN32)

#	Digidesign Protools related plugin development: 
if(HAVE_DIGIDESIGN_AXX)
	add_subdirectory(AAX_SDK)
endif(HAVE_DIGIDESIGN_AXX)
if(HAVE_DIGIDESIGN_PT9)
	add_subdirectory(PT9_SDK) # rtas sdk
endif(HAVE_DIGIDESIGN_PT9)

#	VST Development
if(HAVE_VST3)
	add_subdirectory(VST3_SDK)
endif(HAVE_VST3)
if(HAVE_VST)
	add_subdirectory(VST_SDK)
endif(HAVE_VST)

# for future Xcode 4+ builds if needed
if(HAVE_AUDIOUNITS)
	#add_subdirectory(CA_SDK) 
endif(HAVE_AUDIOUNITS)


#	Common
add_subdirectory(WDL) # wdl lice iplug libs
add_subdirectory(contribs)
add_subdirectory(IPlugExamples)

# Your Custom Project below :
if(IS_DIRECTORY ./IPlugProjects)
  add_subdirectory(IPlugProjects)
endif()

