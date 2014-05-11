# VST3 SDK
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

SET(VST3_SRC
    "base/source/baseiids.cpp"
    "base/source/classfactory.cpp"
    "base/source/fatomic.cpp"
    "base/source/fbitset.cpp"
    "base/source/fbuffer.cpp"
    "base/source/fcpu.cpp"
    "base/source/fdebug.cpp"
    "base/source/fdynlib.cpp"
    "base/source/finitializer.cpp"
    "base/source/fmemory.cpp"
    "base/source/fobject.cpp"
    "base/source/fpoint.cpp"
    "base/source/frect.cpp"
    "base/source/fregion.cpp"
    "base/source/fstreamer.cpp"
    "base/source/fstring.cpp"
    "base/source/fthread.cpp"
    "base/source/timer.cpp"
    "base/source/updatehandler.cpp"
    "pluginterfaces/base/conststringtable.cpp"
    "pluginterfaces/base/funknown.cpp"
    "pluginterfaces/base/ustring.cpp"
    "public.sdk/source/vst3stdsdk.cpp"
    "public.sdk/source/common/memorystream.cpp"
    "public.sdk/source/common/pluginview.cpp"
    "public.sdk/source/main/pluginfactoryvst3.cpp"
)

SET(VST3_HEADERS
    "base/source/classfactory.h"
    "base/source/fatomic.h"
    "base/source/fbitset.h"
    "base/source/fbuffer.h"
    "base/source/fcleanup.h"
    "base/source/fcommandline.h"
    "base/source/fcontainer.h"
    "base/source/fcpu.h"
    "base/source/fdebug.h"
    "base/source/fdynlib.h"
    "base/source/finitializer.h"
    "base/source/fmemory.h"
    "base/source/fobject.h"
    "base/source/fpoint.h"
    "base/source/frect.h"
    "base/source/fregion.h"
    "base/source/fstdmethods.h"
    "base/source/fstreamer.h"
    "base/source/fstring.h"
    "base/source/fthread.h"
    "base/source/hexbinary.h"
    "base/source/tarray.h"
    "base/source/tassociation.h"
    "base/source/tbag.h"
    "base/source/tbinarytree.h"
    "base/source/tbtree.h"
    "base/source/tbtreedictionary.h"
    "base/source/tbtreeset.h"
    "base/source/tcontainer.h"
    "base/source/tdeque.h"
    "base/source/tdictionary.h"
    "base/source/tdlist.h"
    "base/source/thashset.h"
    "base/source/thashtable.h"
    "base/source/timer.h"
    "base/source/tlist.h"
    "base/source/tqueue.h"
    "base/source/tringbuffer.h"
    "base/source/tstack.h"
    "base/source/tvector.h"
    "base/source/updatehandler.h"
    "pluginterfaces/base/conststringtable.h"
    "pluginterfaces/base/falignpop.h"
    "pluginterfaces/base/falignpush.h"
    "pluginterfaces/base/fplatform.h"
    "pluginterfaces/base/fstrdefs.h"
    "pluginterfaces/base/ftypes.h"
    "pluginterfaces/base/funknown.h"
    "pluginterfaces/base/futils.h"
    "pluginterfaces/base/ibstream.h"
    "pluginterfaces/base/icloneable.h"
    "pluginterfaces/base/ierrorcontext.h"
    "pluginterfaces/base/ipersistent.h"
    "pluginterfaces/base/ipluginbase.h"
    "pluginterfaces/base/istringresult.h"
    "pluginterfaces/base/iupdatehandler.h"
    "pluginterfaces/base/keycodes.h"
    "pluginterfaces/base/ustring.h"
    "pluginterfaces/gui/iplugview.h"
    "public.sdk/source/common/memorystream.h"
    "public.sdk/source/common/pluginview.h"
    "public.sdk/source/main/pluginfactoryvst3.h"
)

if(WIN32)
	SET(VST3_SRC ${VST3_SRC} 
	"public.sdk/source/main/dllmain.cpp"
    "base/source/istreamwrapper.cpp"
	)
	SET(VST3_HEADERS ${VST3_HEADERS} 
    "base/source/istreamwrapper.h"
    )
elseif(APPLE)
	SET(VST3_SRC ${VST3_SRC} 
	"public.sdk/source/main/macexport.exp"
	"public.sdk/source/main/macmain.cpp"
	)
endif(WIN32)

if(WIN32)
   if(MSVC)
#      add_definitions(-DWIN32_LEAN_AND_MEAN)
      add_definitions(-D_CRT_SECURE_NO_WARNINGS)
   endif(MSVC)

else(WIN32)
	set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
endif(WIN32)

add_library(vst3 ${VST3_SRC} ${VST3_HEADERS} )

