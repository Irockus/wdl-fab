#-------------------------------------------------------------------------------------
# Useful, reusable macros definitions for iPlug cmake
#   Created by fabien on 2014/07/18
#   For bug reports please contact: fabien@onepost.net
#-------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------

if (APPLE)
	# 0. Init set all sdks off
	set(HAVE_SDK106  OFF)
	set(HAVE_SDK107  OFF)
	set(HAVE_SDK108  OFF)
	set(HAVE_SDK109  OFF)
	set(HAVE_SDK1010 OFF)

	# 1. Set to ON your preferred only the SDKs you have as listed below:
	#   By default it assumes 106 is available and that you also have 10.8 and 10.9
	set(HAVE_SDK106  ON)
	#set(HAVE_SDK107  ON)
	#set(HAVE_SDK108  ON)
	#set(HAVE_SDK109  ON)
	#set(HAVE_SDK1010 ON)

 	# 2. Set the base sdk version to the MINIMUM SDK found upper
 	#    in order to maximize plugin compatibility:
 	
 	if(HAVE_SDK106)
		set(BUILD_SDK_VER 10.6)
		set(BUILD_SDK_MAXVER 1060)
	elseif(HAVE_SDK107)
		set(BUILD_SDK_VER 10.7)
		set(BUILD_SDK_MAXVER 1070)
	elseif(HAVE_SDK108)
		set(BUILD_SDK_VER 10.8)
		set(BUILD_SDK_MAXVER 1080)
	elseif(HAVE_SDK109)
		set(BUILD_SDK_VER 10.9)
		set(BUILD_SDK_MAXVER 1090)
	elseif(HAVE_SDK1010)
		set(BUILD_SDK_VER 10.10)
		set(BUILD_SDK_MAXVER 10100)
	endif() 	

endif(APPLE)