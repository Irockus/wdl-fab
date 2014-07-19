# AAX SDK

SET(AAX_SRC
	Libs/AAXLibrary/source/AAX_CACFUnknown.cpp
	Libs/AAXLibrary/source/AAX_CAutoreleasePool.OSX.mm
	Libs/AAXLibrary/source/AAX_CAutoreleasePool.Win.cpp
	Libs/AAXLibrary/source/AAX_CChunkDataParser.cpp
	Libs/AAXLibrary/source/AAX_CEffectDirectData.cpp
	Libs/AAXLibrary/source/AAX_CEffectGUI_Icon.cpp
	Libs/AAXLibrary/source/AAX_CEffectGUI.cpp
	Libs/AAXLibrary/source/AAX_CEffectParameters.cpp
	Libs/AAXLibrary/source/AAX_CHostProcessor.cpp
	Libs/AAXLibrary/source/AAX_CHostServices.cpp
	Libs/AAXLibrary/source/AAX_CInstrumentParameters.cpp
	Libs/AAXLibrary/source/AAX_CMutex.cpp
	Libs/AAXLibrary/source/AAX_CommonConversions.cpp
	Libs/AAXLibrary/source/AAX_CPacketDispatcher.cpp
	Libs/AAXLibrary/source/AAX_CParameter.cpp
	Libs/AAXLibrary/source/AAX_CParameterManager.cpp
	Libs/AAXLibrary/source/AAX_CString.cpp
	Libs/AAXLibrary/source/AAX_CUIDs.cpp
	Libs/AAXLibrary/source/AAX_IEffectDirectData.cpp
	Libs/AAXLibrary/source/AAX_IEffectGUI_Icon.cpp
	Libs/AAXLibrary/source/AAX_IEffectGUI.cpp
	Libs/AAXLibrary/source/AAX_IEffectParameters.cpp
	Libs/AAXLibrary/source/AAX_IHostProcessor.cpp
	Libs/AAXLibrary/source/AAX_Init.cpp
	Libs/AAXLibrary/source/AAX_Properties.cpp
	Libs/AAXLibrary/source/AAX_SliderConversions.cpp
	Libs/AAXLibrary/source/AAX_StringUtilities.cpp
	Libs/AAXLibrary/source/AAX_VAutomationDelegate.cpp
	Libs/AAXLibrary/source/AAX_VCollection.cpp
	Libs/AAXLibrary/source/AAX_VComponentDescriptor.cpp
	Libs/AAXLibrary/source/AAX_VController.cpp
	Libs/AAXLibrary/source/AAX_VEffectDescriptor.cpp
	Libs/AAXLibrary/source/AAX_VHostProcessorDelegate.cpp
	Libs/AAXLibrary/source/AAX_VHostServices.cpp
	Libs/AAXLibrary/source/AAX_VPrivateDataAccess.cpp
	Libs/AAXLibrary/source/AAX_VPropertyMap.cpp
	Libs/AAXLibrary/source/AAX_VTransport.cpp
	Libs/AAXLibrary/source/AAX_VViewContainer.cpp
)

SET(AAX_HEADERS
	Libs/AAXLibrary/include/AAX_Atomic.h
	Libs/AAXLibrary/include/AAX_StringUtilities.h
	Libs/AAXLibrary/include/AAX_TimeUtilities.h
)

if(WIN32)
	SET(AAX_SRC ${AAX_SRC} 
	)
elseif(APPLE)
	SET(AAX_SRC ${AAX_SRC} 
	)
	set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
endif(WIN32)

add_library(AAX ${AAX_SRC} ${AAX_HEADERS} )

