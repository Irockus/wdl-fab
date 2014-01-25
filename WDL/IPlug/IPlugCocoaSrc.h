/*
 *  IPlugCocoaSrc.h
 *  All mac objc++ dependant resources to build within each iplug plugin/app
 *  MUST be compiled OUSIDE the iPlug libs
 *  Used by the default IPlugCocoa.mm but could be included in your custom iplug mm files too.
 *  Created by Fabien on 1/24/14.
 *
 */

#include "../swell/swell-misc.mm"
#include "../swell/swell-menu.mm"
# include "../swell/swell-wnd.mm"
#ifndef SA_API
#else
# include "../swell/swell-dlg.mm"
#endif

#include "IGraphicsCocoa.mm"
#include "IGraphicsMac.mm"

#ifdef SA_API
# include "../swell/swellappmain.mm"
# include "main.mm"
#endif