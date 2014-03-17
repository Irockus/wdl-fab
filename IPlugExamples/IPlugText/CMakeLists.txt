#
# IPlug Example Executable build
#

# Example plugin name
set (PLUGNAME IPlugText)

# Source code to build
set(IPC_SRCS
   ${PLUGNAME}.cpp
)
if(WIN32)
 set(IPC_SRCS ${IPC_SRCS} ${PLUGNAME}.rc)
endif(WIN32)


# Source code headers
set(IPC_HEADERS
   ${IPC_HEADERS}
   resource.h
   ${PLUGNAME}.h
   )
 
set (PLUGNAMEVST  ${PLUGNAME}VST)
set (PLUGNAMEVST3 ${PLUGNAME}VST3)
set (PLUGNAMEAU   ${PLUGNAME}AU)
set (PLUGNAMEAAX  ${PLUGNAME}AAX)
set (PLUGNAMERTAS  ${PLUGNAME}RTAS)

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

