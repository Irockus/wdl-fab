#ifndef _SWELL_INTERNAL_H_
#define _SWELL_INTERNAL_H_

#include "../ptrlist.h"

#include "swell-internal-structs.h" // exportable to a lib, no objc classname problem

#ifdef SWELL_TARGET_OSX

#define __SWELL_PREFIX_CLASSNAME3(a,b) a##b
#define __SWELL_PREFIX_CLASSNAME2(a,b) __SWELL_PREFIX_CLASSNAME3(a,b)
#define __SWELL_PREFIX_CLASSNAME(cname) __SWELL_PREFIX_CLASSNAME2(SWELL_APP_PREFIX,cname)

// this defines interfaces to internal swell classes
#define SWELL_hwndChild __SWELL_PREFIX_CLASSNAME(_hwnd)
#define SWELL_hwndCarbonHost __SWELL_PREFIX_CLASSNAME(_hwndcarbonhost)

#define SWELL_ModelessWindow __SWELL_PREFIX_CLASSNAME(_modelesswindow)
#define SWELL_ModalDialog __SWELL_PREFIX_CLASSNAME(_dialogbox)

#define SWELL_TextField __SWELL_PREFIX_CLASSNAME(_textfield)
#define SWELL_ListView __SWELL_PREFIX_CLASSNAME(_listview)
#define SWELL_TreeView __SWELL_PREFIX_CLASSNAME(_treeview)
#define SWELL_TabView __SWELL_PREFIX_CLASSNAME(_tabview)
#define SWELL_ProgressView  __SWELL_PREFIX_CLASSNAME(_progind)
#define SWELL_TextView  __SWELL_PREFIX_CLASSNAME(_textview)
#define SWELL_BoxView __SWELL_PREFIX_CLASSNAME(_box)
#define SWELL_Button __SWELL_PREFIX_CLASSNAME(_button)
#define SWELL_PopUpButton __SWELL_PREFIX_CLASSNAME(_pub)
#define SWELL_ComboBox __SWELL_PREFIX_CLASSNAME(_cbox)

#define SWELL_StatusCell __SWELL_PREFIX_CLASSNAME(_statuscell)
#define SWELL_ListViewCell __SWELL_PREFIX_CLASSNAME(_listviewcell)
#define SWELL_ODListViewCell __SWELL_PREFIX_CLASSNAME(_ODlistviewcell)
#define SWELL_ODButtonCell __SWELL_PREFIX_CLASSNAME(_ODbuttoncell)

#define SWELL_FocusRectWnd __SWELL_PREFIX_CLASSNAME(_drawfocusrectwnd)

#define SWELL_DataHold __SWELL_PREFIX_CLASSNAME(_sdh)
#define SWELL_ThreadTmp __SWELL_PREFIX_CLASSNAME(_thread)
#define SWELL_PopupMenuRecv __SWELL_PREFIX_CLASSNAME(_trackpopupmenurecv)

#define SWELL_TimerFuncTarget __SWELL_PREFIX_CLASSNAME(_tft)


#define SWELL_Menu __SWELL_PREFIX_CLASSNAME(_menu)

#ifdef __OBJC__

@interface SWELL_Menu : NSMenu
{
}
-(void)dealloc;
- (id)copyWithZone:(NSZone *)zone;
@end

@interface SWELL_DataHold : NSObject
{
  void *m_data;
}
-(id) initWithVal:(void *)val;
-(void *) getValue;
@end

@interface SWELL_TimerFuncTarget : NSObject
{
  TIMERPROC m_cb;
  HWND m_hwnd;
  UINT_PTR m_timerid;
}
-(id) initWithId:(UINT_PTR)tid hwnd:(HWND)h callback:(TIMERPROC)cb;
-(void)SWELL_Timer:(id)sender;
@end

@interface SWELL_TextField : NSTextField
- (void)setNeedsDisplay:(BOOL)flag;
- (void)setNeedsDisplayInRect:(NSRect)rect;
@end

@interface SWELL_TabView : NSTabView
{
  NSInteger m_tag;
  id m_dest;
}
@end

@interface SWELL_ProgressView : NSProgressIndicator
{
  NSInteger m_tag;
}
@end

@interface SWELL_ListViewCell : NSTextFieldCell
{
}
@end

@interface SWELL_StatusCell : NSTextFieldCell
{
  NSImage *status;
}
@end

@interface SWELL_TreeView : NSOutlineView
{
@public
  bool m_fakerightmouse;
  LONG style;
  WDL_PtrList<HTREEITEM__> *m_items;
  NSColor *m_fgColor;
  NSMutableArray *m_selColors;
}
@end

@interface SWELL_ListView : NSTableView
{
  int m_leftmousemovecnt;
  bool m_fakerightmouse;
@public
  LONG style;
  int ownermode_cnt;
  int m_start_item;
  int m_start_subitem;
  int m_start_item_clickmode;
  int m_lbMode;
  WDL_PtrList<SWELL_ListView_Row> *m_items;
  WDL_PtrList<NSTableColumn> *m_cols;
  WDL_PtrList<HGDIOBJ__> *m_status_imagelist;
  int m_status_imagelist_type;
  int m_fastClickMask;
  NSColor *m_fgColor;
  NSMutableArray *m_selColors;
}
-(LONG)getSwellStyle;
-(void)setSwellStyle:(LONG)st;
-(int)getSwellNotificationMode;
-(void)setSwellNotificationMode:(int)lbMode;
-(int)columnAtPoint:(NSPoint)pt;
-(int)getColumnPos:(int)idx; // get current position of column that was originally at idx
-(int)getColumnIdx:(int)pos; // get original index of column that is currently at position
@end

@interface SWELL_ODButtonCell : NSButtonCell
{
}
@end

@interface SWELL_ODListViewCell : NSCell
{
  SWELL_ListView *m_ownctl;
  int m_lastidx;
}
-(void)setOwnerControl:(SWELL_ListView *)t;
-(void)setItemIdx:(int)idx;
@end

@interface SWELL_Button : NSButton
{
  void *m_swellGDIimage;
  LONG_PTR m_userdata;
  int m_radioflags;
}
-(int)swellGetRadioFlags;
-(void)swellSetRadioFlags:(int)f;
-(LONG_PTR)getSwellUserData;
-(void)setSwellUserData:(LONG_PTR)val;
-(void)setSwellGDIImage:(void *)par;
-(void *)getSwellGDIImage;
@end

@interface SWELL_TextView : NSTextView
{
  NSInteger m_tag;
}
-(NSInteger) tag;
-(void) setTag:(NSInteger)tag;
@end

@interface SWELL_BoxView : NSBox
{
  NSInteger m_tag;
}
-(NSInteger) tag;
-(void) setTag:(NSInteger)tag;
@end

@interface SWELL_FocusRectWnd : NSView
{
}
@end

@interface SWELL_ThreadTmp : NSObject
{
@public
  void *a, *b;
}
-(void)bla:(id)obj;
@end

@interface SWELL_hwndChild : NSView // <NSDraggingSource>
{
@public
  BOOL m_enabled;
  DLGPROC m_dlgproc;
  WNDPROC m_wndproc;
  LONG_PTR m_userdata;
  LONG_PTR m_extradata[32];
  NSInteger m_tag;
  int m_isfakerightmouse;
  char m_hashaddestroy; // 2 = WM_DESTROY has finished completely
  HMENU m_menu;
  BOOL m_flip;
  bool m_supports_ddrop;
  bool m_paintctx_used;
  HDC m_paintctx_hdc;
  WindowPropRec *m_props;
  NSRect m_paintctx_rect;
  BOOL m_isopaque;
  char m_titlestr[1024];
  unsigned int m_create_windowflags;
  NSOpenGLContext *m_glctx;
  char m_isdirty; // &1=self needs redraw, &2=children may need redraw
  id m_lastTopLevelOwner; // save a copy of the owner, if any
  id m_access_cacheptrs[6];
}
- (id)initChild:(SWELL_DialogResourceIndex *)resstate Parent:(NSView *)parent dlgProc:(DLGPROC)dlgproc Param:(LPARAM)par;
- (LRESULT)onSwellMessage:(UINT)msg p1:(WPARAM)wParam p2:(LPARAM)lParam;
-(HANDLE)swellExtendedDragOp:(id <NSDraggingInfo>)sender retGlob:(BOOL)retG;
- (const char *)onSwellGetText;
-(void)onSwellSetText:(const char *)buf;
-(LONG)swellGetExtendedStyle;
-(void)swellSetExtendedStyle:(LONG)st;
-(HMENU)swellGetMenu;
-(BOOL)swellHasBeenDestroyed;
-(void)swellSetMenu:(HMENU)menu;
-(LONG_PTR)getSwellUserData;
-(void)setSwellUserData:(LONG_PTR)val;
-(void)setOpaque:(bool)isOpaque;
-(LPARAM)getSwellExtraData:(int)idx;
-(void)setSwellExtraData:(int)idx value:(LPARAM)val;
-(void)setSwellWindowProc:(WNDPROC)val;
-(WNDPROC)getSwellWindowProc;
-(void)setSwellDialogProc:(DLGPROC)val;
-(DLGPROC)getSwellDialogProc;

- (NSArray*) namesOfPromisedFilesDroppedAtDestination:(NSURL*)droplocation;

-(void) getSwellPaintInfo:(PAINTSTRUCT *)ps;
- (int)swellCapChangeNotify;
-(unsigned int)swellCreateWindowFlags;

-(bool)swellCanPostMessage;
-(int)swellEnumProps:(PROPENUMPROCEX)proc lp:(LPARAM)lParam;
-(void *)swellGetProp:(const char *)name wantRemove:(BOOL)rem;
-(int)swellSetProp:(const char *)name value:(void *)val ;


// NSAccessibility

#if 0
// TODO: Investigate if those below should be implemented one day:

// attribute methods
- (NSArray *)accessibilityAttributeNames;
- (id)accessibilityAttributeValue:(NSString *)attribute;
- (BOOL)accessibilityIsAttributeSettable:(NSString *)attribute;
- (void)accessibilitySetValue:(id)value forAttribute:(NSString *)attribute;

// parameterized attribute methods
- (NSArray *)accessibilityParameterizedAttributeNames;
- (id)accessibilityAttributeValue:(NSString *)attribute forParameter:(id)parameter;

// action methods
- (NSArray *)accessibilityActionNames;
- (NSString *)accessibilityActionDescription:(NSString *)action;
- (void)accessibilityPerformAction:(NSString *)action;

#endif

// Return YES if the UIElement doesn't show up to the outside world - i.e. its parent should return the UIElement's children as its own - cutting the UIElement out. E.g. NSControls are ignored when they are single-celled.
- (BOOL)accessibilityIsIgnored;

// Returns the deepest descendant of the UIElement hierarchy that contains the point. You can assume the point has already been determined to lie within the receiver. Override this method to do deeper hit testing within a UIElement - e.g. a NSMatrix would test its cells. The point is bottom-left relative screen coordinates.
- (id)accessibilityHitTest:(NSPoint)point;

// Returns the UI Element that has the focus. You can assume that the search for the focus has already been narrowed down to the reciever. Override this method to do a deeper search with a UIElement - e.g. a NSMatrix would determine if one of its cells has the focus.
- (id)accessibilityFocusedUIElement;

@end

@interface SWELL_ModelessWindow : NSWindow
{
@public
  NSSize lastFrameSize;
  id m_owner;
  OwnedWindowListRec *m_ownedwnds;
  BOOL m_enabled;
  int m_wantraiseamt;
  bool  m_wantInitialKeyWindowOnShow;
}
- (id)initModeless:(SWELL_DialogResourceIndex *)resstate Parent:(HWND)parent dlgProc:(DLGPROC)dlgproc Param:(LPARAM)par outputHwnd:(HWND *)hwndOut forceStyles:(unsigned int)smask;
- (id)initModelessForChild:(HWND)child owner:(HWND)owner styleMask:(unsigned int)smask;
- (void)swellDestroyAllOwnedWindows;
- (void)swellRemoveOwnedWindow:(NSWindow *)wnd;
- (void)swellSetOwner:(id)owner;
- (id)swellGetOwner;
- (void **)swellGetOwnerWindowHead;
-(void)swellDoDestroyStuff;
-(void)swellResetOwnedWindowLevels;
@end

@interface SWELL_ModalDialog : NSPanel
{
  NSSize lastFrameSize;
  id m_owner;
  OwnedWindowListRec *m_ownedwnds;

  int m_rv;
  bool m_hasrv;
  BOOL m_enabled;
}
- (id)initDialogBox:(SWELL_DialogResourceIndex *)resstate Parent:(HWND)parent dlgProc:(DLGPROC)dlgproc Param:(LPARAM)par;
- (void)swellDestroyAllOwnedWindows;
- (void)swellRemoveOwnedWindow:(NSWindow *)wnd;
- (void)swellSetOwner:(id)owner;
- (id)swellGetOwner;
- (void **)swellGetOwnerWindowHead;
-(void)swellDoDestroyStuff;

-(void)swellSetModalRetVal:(int)r;
-(int)swellGetModalRetVal;
-(bool)swellHasModalRetVal;
@end

@interface SWELL_hwndCarbonHost : SWELL_hwndChild
#ifdef MAC_OS_X_VERSION_10_7
  <NSWindowDelegate>
#endif
{
@public
  NSWindow *m_cwnd;

  bool m_whileresizing;
  void* m_wndhandler;   // won't compile if declared EventHandlerRef, wtf
  void* m_ctlhandler;   // not sure if these need to be separate but cant hurt
  bool m_wantallkeys;
}
-(BOOL)swellIsCarbonHostingView;
-(void)swellDoRepos;
@end

@interface SWELL_PopupMenuRecv : NSObject
{
  int m_act;
  HWND cbwnd;
}
-(id) initWithWnd:(HWND)wnd;
-(void) onSwellCommand:(id)sender;
-(int) isCommand;
- (void)menuNeedsUpdate:(NSMenu *)menu;

@end

@interface SWELL_PopUpButton : NSPopUpButton
{
  LONG m_style;
}
-(void)setSwellStyle:(LONG)style;
-(LONG)getSwellStyle;
@end

@interface SWELL_ComboBox : NSComboBox
{
@public
  LONG m_style;
  WDL_PtrList<char> *m_ids;
}
-(id)init;
-(void)dealloc;
-(void)setSwellStyle:(LONG)style;
-(LONG)getSwellStyle;
@end

// some extras so we can call functions available only on some OSX versions without warnings, and with the correct types
#define SWELL_DelegateExtensions __SWELL_PREFIX_CLASSNAME(_delext)
#define SWELL_ViewExtensions __SWELL_PREFIX_CLASSNAME(_viewext)
#define SWELL_AppExtensions __SWELL_PREFIX_CLASSNAME(_appext)
#define SWELL_WindowExtensions __SWELL_PREFIX_CLASSNAME(_wndext)
#define SWELL_TableColumnExtensions __SWELL_PREFIX_CLASSNAME(_tcolext)

@interface SWELL_WindowExtensions : NSWindow
-(void)setCollectionBehavior:(NSUInteger)a;
@end
@interface SWELL_ViewExtensions : NSView
-(void)_recursiveDisplayRectIfNeededIgnoringOpacity:(NSRect)rect isVisibleRect:(BOOL)vr rectIsVisibleRectForView:(NSView*)v topView:(NSView *)v2;
@end

@interface SWELL_DelegateExtensions : NSObject
-(bool)swellPostMessage:(HWND)dest msg:(int)message wp:(WPARAM)wParam lp:(LPARAM)lParam;
-(void)swellPostMessageClearQ:(HWND)dest;
-(void)swellPostMessageTick:(id)sender;
@end

@interface SWELL_AppExtensions : NSApplication
-(NSUInteger)presentationOptions;
-(void)setPresentationOptions:(NSUInteger)o;
@end
@interface SWELL_TableColumnExtensions : NSTableColumn
-(BOOL)isHidden;
-(void)setHidden:(BOOL)h;
@end

#endif // __OBJC__

#endif // ifdef SWELL_TARGET_OSX

#endif // ifndef _SWELL_INTERNAL_H_
