#include "app_main.h"
#include "app_resource.h"

#include "../resource.h"

#ifdef OS_WIN
#include "asio.h"

#include <windows.h>
#include <shlobj.h>
#include <sys/stat.h>

#define snprintf _snprintf
#endif

const int kNumIOVSOptions = 11;
const int kNumSIGVSOptions = 9;

const std::string kIOVSOptions[kNumIOVSOptions] = {"32", "64", "96", "128", "192", "256", "512", "1024", "2048", "4096", "8192" };
const std::string kSIGVSOptions[kNumSIGVSOptions] = {"16", "32", "64", "96", "128", "192", "256", "512", "1024" };

HWND gHWND=0;

WDL_DLGRET AppWrapper::MainDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  AppWrapper& i = AppWrapper::Instance();

  switch (uMsg)
  {
    case WM_INITDIALOG:
    {
      gHWND=hwndDlg;

      if (!i.AttachGUI()) Error("couldn't attach gui\n");

#ifdef _WIN32
      i.ClientResize(hwndDlg, GUI_WIDTH, GUI_HEIGHT);
      HINSTANCE hInst = GetModuleHandle(NULL);
      HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
      HICON hIconSm  = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0);
      if (hIcon) SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (LPARAM) hIcon);
      if (hIconSm) SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (LPARAM) hIconSm);

#else // OSX
      CenterWindow(hwndDlg);
#endif

      ShowWindow(hwndDlg,SW_SHOW);
      return 1;
    }
    case WM_DESTROY:
      gHWND = NULL;

#ifdef _WIN32
      PostQuitMessage(0);
#else
      SWELL_PostQuitMessage(hwndDlg);
#endif

      return 0;
    case WM_CLOSE:
      DestroyWindow(hwndDlg);
      return 0;
//    case WM_GETDLGCODE: {
//        LPARAM lres;
//        lres = CallWindowProc(/*TODO GET PROC */, hWnd, WM_GETDLGCODE, wParam, lParam);
//        if (lParam && ((MSG*)lParam)->message == WM_KEYDOWN  &&  wParam == VK_LEFT) {
//          lres |= DLGC_WANTMESSAGE;
//        }
//        return lres;
//      }
    case WM_COMMAND:
      switch (LOWORD(wParam))
      {
        case ID_QUIT:
          DestroyWindow(hwndDlg);
          return 0;
        case ID_ABOUT:
          if (!i.gPluginInstance->HostRequestingAboutBox())
          {
            char version[50];
            snprintf(version, sizeof(version), BUNDLE_MFR"\nBuilt on "__DATE__);
            MessageBox(hwndDlg,version, BUNDLE_NAME, MB_OK);
          }
          return 0;
        case ID_PREFERENCES:
        {
          INT_PTR ret = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_PREF), hwndDlg, PreferencesDlgProc);

          if(ret == IDOK)
          {
            i.UpdateINI();
          }

          return 0;
        }
      }
      return 0;
  }
  return 0;
}

#ifdef OS_WIN
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nShowCmd)
{
  AppWrapper& i = AppWrapper::Instance();
  HINSTANCE& gHINST = i.gHINST;

  // first check to make sure this is the only instance running
  // http://www.bcbjournal.org/articles/vol3/9911/Single-instance_applications.htm
  try
  {
    // Try to open the mutex.
    HANDLE hMutex = OpenMutex(
                      MUTEX_ALL_ACCESS, 0, BUNDLE_NAME);

    // If hMutex is 0 then the mutex doesn't exist.
    if (!hMutex)
      hMutex = CreateMutex(0, 0, BUNDLE_NAME);
    else
    {
      // This is a second instance. Bring the
      // original instance to the top.
      HWND hWnd = FindWindow(0, BUNDLE_NAME);
      SetForegroundWindow(hWnd);

      return 0;
    }

    i.gHINST = hInstance;

    InitCommonControls();
    i.gScrollMessage = RegisterWindowMessage("MSWHEEL_ROLLMSG");

    i.gState = new AppState();
    i.gTempState = new AppState();
    i.gActiveState = new AppState();

    if (SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, i.gINIPath) != S_OK)
    {
      DBGMSG("could not retrieve the user's application data directory!\n");

      //TODO error msg?
      return 1;
    }

    char * & gINIPath = i.gINIPath;
    AppState* gState = i.gState;

    snprintf(gINIPath, MAX_APP_INIPATH, "%s\\%s", gINIPath, BUNDLE_NAME); // Add the app name to the path

    struct stat st;
    if (stat(gINIPath, &st) == 0) // if directory exists
    {
      snprintf(gINIPath, MAX_APP_INIPATH, "%s\\%s", gINIPath, "settings.ini"); // add file name to path

      if (stat(gINIPath, &st) == 0) // if settings file exists read values into state
      {
        gState->mAudioDriverType = GetPrivateProfileInt("audio", "driver", 0, gINIPath);

        GetPrivateProfileString("audio", "indev", DEFAULT_INPUT_DEV, gState->mAudioInDev, 100, gINIPath);
        GetPrivateProfileString("audio", "outdev", DEFAULT_OUTPUT_DEV, gState->mAudioOutDev, 100, gINIPath);

        //audio
        gState->mAudioInChanL = GetPrivateProfileInt("audio", "in1", 1, gINIPath); // 1 is first audio input
        gState->mAudioInChanR = GetPrivateProfileInt("audio", "in2", 2, gINIPath);
        gState->mAudioOutChanL = GetPrivateProfileInt("audio", "out1", 1, gINIPath); // 1 is first audio output
        gState->mAudioOutChanR = GetPrivateProfileInt("audio", "out2", 2, gINIPath);
        gState->mAudioInIsMono = GetPrivateProfileInt("audio", "monoinput", 0, gINIPath);

        GetPrivateProfileString("audio", "iovs", "512", gState->mAudioIOVS, 100, gINIPath);
        GetPrivateProfileString("audio", "sigvs", "32", gState->mAudioSigVS, 100, gINIPath);
        GetPrivateProfileString("audio", "sr", "44100", gState->mAudioSR, 100, gINIPath);

        //midi
        GetPrivateProfileString("midi", "indev", "no input", gState->mMidiInDev, 100, gINIPath);
        GetPrivateProfileString("midi", "outdev", "no output", gState->mMidiOutDev, 100, gINIPath);

        gState->mMidiInChan = GetPrivateProfileInt("midi", "inchan", 0, gINIPath); // 0 is any
        gState->mMidiOutChan = GetPrivateProfileInt("midi", "outchan", 0, gINIPath); // 1 is first chan

        i.UpdateINI(); // this will write over any invalid values in the file
      }
      else // settings file doesn't exist, so populate with default values
      {
        i.UpdateINI();
      }
    }
    else
    {
      // folder doesn't exist - make folder and make file
      CreateDirectory(gINIPath, NULL);
      sprintf(gINIPath, "%s%s", gINIPath, "settings.ini"); // add file name to path
      i.UpdateINI(); // will write file if doesn't exist
    }

    i.Init();

    CreateDialog(gHINST, MAKEINTRESOURCE(IDD_DIALOG_MAIN), GetDesktopWindow(), AppWrapper::MainDlgProc);

    for (;;)
    {
      MSG msg = { 0, };
      int vvv = GetMessage(&msg, NULL, 0, 0);
      if (!vvv)  break;

      if (vvv<0)
      {
        Sleep(10);
        continue;
      }
      if (!msg.hwnd)
      {
        DispatchMessage(&msg);
        continue;
      }

      if (gHWND && IsDialogMessage(gHWND, &msg)) continue;

      // default processing for other dialogs
      HWND hWndParent = NULL;
      HWND temphwnd = msg.hwnd;
      do
      {
        if (GetClassLong(temphwnd, GCW_ATOM) == (INT)32770)
        {
          hWndParent = temphwnd;
          if (!(GetWindowLong(temphwnd, GWL_STYLE)&WS_CHILD)) break; // not a child, exit
        }
      }
      while (temphwnd = GetParent(temphwnd));

      if (hWndParent && IsDialogMessage(hWndParent, &msg)) continue;

      TranslateMessage(&msg);
      DispatchMessage(&msg);

    }

    // in case gHWND didnt get destroyed -- this corresponds to SWELLAPP_DESTROY roughly
    if (gHWND) DestroyWindow(gHWND);

    i.Cleanup();

    ReleaseMutex(hMutex);
  }
  catch (...)
  {
    //TODO proper error catching
    DBGMSG("another instance running");
  }
  return 0;
}

#else

extern HMENU SWELL_app_stocksysmenu;
const char *homeDir;

INT_PTR SWELLAppMain(int msg, INT_PTR parm1, INT_PTR parm2)
{
  AppWrapper& i = AppWrapper::Instance();
  char * &gINIPath = i.gINIPath;
  AppState*& gState = i.gState;
  switch (msg)
  {
    case SWELLAPP_ONLOAD:

      i.gState = new AppState();
      i.gTempState = new AppState();
      i.gActiveState = new AppState();

      homeDir = getenv("HOME");
      sprintf(gINIPath, "%s/Library/Application Support/%s/", homeDir, BUNDLE_NAME);

      struct stat st;
      if (stat(gINIPath, &st) == 0) // if directory exists
      {
        sprintf(gINIPath, "%s%s", gINIPath, "settings.ini"); // add file name to path

        if (stat(gINIPath, &st) == 0) // if settings file exists read values into state
        {
          gState->mAudioDriverType = GetPrivateProfileInt("audio", "driver", 0, gINIPath);

          GetPrivateProfileString("audio", "indev", "Built-in Input", gState->mAudioInDev, 100, gINIPath);
          GetPrivateProfileString("audio", "outdev", "Built-in Output", gState->mAudioOutDev, 100, gINIPath);

          //audio
          gState->mAudioInChanL = GetPrivateProfileInt("audio", "in1", 1, gINIPath); // 1 is first audio input
          gState->mAudioInChanR = GetPrivateProfileInt("audio", "in2", 2, gINIPath);
          gState->mAudioOutChanL = GetPrivateProfileInt("audio", "out1", 1, gINIPath); // 1 is first audio output
          gState->mAudioOutChanR = GetPrivateProfileInt("audio", "out2", 2, gINIPath);
          gState->mAudioInIsMono = GetPrivateProfileInt("audio", "monoinput", 0, gINIPath);

          GetPrivateProfileString("audio", "iovs", "512", gState->mAudioIOVS, 100, gINIPath);
          GetPrivateProfileString("audio", "sigvs", "32", gState->mAudioSigVS, 100, gINIPath);
          GetPrivateProfileString("audio", "sr", "44100", gState->mAudioSR, 100, gINIPath);

          //midi
          GetPrivateProfileString("midi", "indev", "no input", gState->mMidiInDev, 100, gINIPath);
          GetPrivateProfileString("midi", "outdev", "no output", gState->mMidiOutDev, 100, gINIPath);

          gState->mMidiInChan = GetPrivateProfileInt("midi", "inchan", 0, gINIPath); // 0 is any
          gState->mMidiOutChan = GetPrivateProfileInt("midi", "outchan", 0, gINIPath); // 1 is first chan

          i.UpdateINI(); // this will write over any invalid values in the file
        }
        else // settings file doesn't exist, so populate with default values
        {
          i.UpdateINI();
        }

      }
      else   // folder doesn't exist - make folder and make file
      {
        // http://blog.tremend.ro/2008/10/06/create-directories-in-c-using-mkdir-with-proper-permissions/

        mode_t process_mask = umask(0);
        int result_code = mkdir(gINIPath, S_IRWXU | S_IRWXG | S_IRWXO);
        umask(process_mask);

        if (result_code) return 1;
        else
        {
          sprintf(gINIPath, "%s%s", gINIPath, "settings.ini"); // add file name to path
          i.UpdateINI(); // will write file if doesn't exist
        }
      }
      break;
#pragma mark loaded
    case SWELLAPP_LOADED:
    {
      i.Init();

      HMENU menu = SWELL_GetCurrentMenu();

      if (menu)
      {
        // other windows will get the stock (bundle) menus
        //SWELL_SetDefaultModalWindowMenu(menu);
        //SWELL_SetDefaultWindowMenu(menu);

        // work on a new menu
        menu = SWELL_DuplicateMenu(menu);
        HMENU src = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU1));
        int x;
        for (x = 0; x<GetMenuItemCount(src) - 1; x++)
        {
          HMENU sm = GetSubMenu(src, x);
          if (sm)
          {
            char str[1024];
            MENUITEMINFO mii = { sizeof(mii), MIIM_TYPE, };
            mii.dwTypeData = str;
            mii.cch = sizeof(str);
            str[0] = 0;
            GetMenuItemInfo(src, x, TRUE, &mii);
            MENUITEMINFO mi = { sizeof(mi), MIIM_STATE | MIIM_SUBMENU | MIIM_TYPE, MFT_STRING, 0, 0, SWELL_DuplicateMenu(sm), NULL, NULL, 0, str };
            InsertMenuItem(menu, x + 1, TRUE, &mi);
          }
        }
      }

      if (menu)
      {
        HMENU sm = GetSubMenu(menu, 1);
        DeleteMenu(sm, ID_QUIT, MF_BYCOMMAND); // remove QUIT from our file menu, since it is in the system menu on OSX
        DeleteMenu(sm, ID_PREFERENCES, MF_BYCOMMAND); // remove PREFERENCES from the file menu, since it is in the system menu on OSX

        // remove any trailing separators
        int a = GetMenuItemCount(sm);
        while (a > 0 && GetMenuItemID(sm, a - 1) == 0) DeleteMenu(sm, --a, MF_BYPOSITION);

        DeleteMenu(menu, 1, MF_BYPOSITION); // delete file menu
      }

      // if we want to set any default modifiers for items in the menus, we can use:
      // SetMenuItemModifier(menu,commandID,MF_BYCOMMAND,'A',FCONTROL) etc.

      HWND hwnd = CreateDialog(i.gHINST, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, AppWrapper::MainDlgProc);
      if (menu)
      {
        SetMenu(hwnd, menu); // set the menu for the dialog to our menu (on Windows that menu is set from the .rc, but on SWELL
        SWELL_SetDefaultModalWindowMenu(menu); // other windows will get the stock (bundle) menus
      }
      // we need to set it manually (and obviously we've edited the menu anyway)
    }

    if (!i.AttachGUI()) DBGMSG("couldn't attach gui\n"); //todo error

    break;
    case SWELLAPP_ONCOMMAND:
      // this is to catch commands coming from the system menu etc
      if (gHWND && (parm1 & 0xffff)) SendMessage(gHWND, WM_COMMAND, parm1 & 0xffff, 0);
      break;
#pragma mark destroy
    case SWELLAPP_DESTROY:

      if (gHWND) DestroyWindow(gHWND);
      i.Cleanup();
      break;
    case SWELLAPP_PROCESSMESSAGE: // can hook keyboard input here
      // parm1 = (MSG*), should we want it -- look in swell.h to see what the return values refer to
      break;
  }
  return 0;
}

#endif


#ifndef OS_WIN
#include "swell-dlggen.h"

#define SET_IDD_SCALE 1.
#define SET_IDD_STYLE SWELL_DLG_WS_FLIPPED|SWELL_DLG_WS_NOAUTOSIZE

SWELL_DEFINE_DIALOG_RESOURCE_BEGIN(IDD_DIALOG_MAIN, SET_IDD_STYLE, BUNDLE_NAME, GUI_WIDTH, GUI_HEIGHT, SET_IDD_SCALE)
BEGIN
//   EDITTEXT        IDC_EDIT1,59,50,145,14,ES_AUTOHSCROLL
//   LTEXT           "Enter some text here:",IDC_STATIC,59,39,73,8
END
SWELL_DEFINE_DIALOG_RESOURCE_END(IDD_DIALOG_MAIN)

SWELL_DEFINE_DIALOG_RESOURCE_BEGIN(IDD_DIALOG_PREF, SET_IDD_STYLE, "Preferences", 320, 420, SET_IDD_SCALE)
BEGIN
GROUPBOX        "Audio Settings", IDC_STATIC, 5, 10, 300, 230

LTEXT           "Driver Type", IDC_STATIC, 20, 32, 60, 20
COMBOBOX        IDC_COMBO_AUDIO_DRIVER, 20, 50, 150, 100, CBS_DROPDOWNLIST

LTEXT           "Input Device", IDC_STATIC, 20, 75, 80, 20
COMBOBOX        IDC_COMBO_AUDIO_IN_DEV, 20, 90, 150, 100, CBS_DROPDOWNLIST

LTEXT           "Output Device", IDC_STATIC, 20, 115, 80, 20
COMBOBOX        IDC_COMBO_AUDIO_OUT_DEV, 20, 130, 150, 100, CBS_DROPDOWNLIST

LTEXT           "In 1 (L)", IDC_STATIC, 20, 155, 90, 20
COMBOBOX        IDC_COMBO_AUDIO_IN_L, 20, 170, 46, 100, CBS_DROPDOWNLIST

LTEXT           "In 2 (R)", IDC_STATIC, 75, 155, 90, 20
COMBOBOX        IDC_COMBO_AUDIO_IN_R, 75, 170, 46, 100, CBS_DROPDOWNLIST

CHECKBOX        "Mono", IDC_CB_MONO_INPUT, 125, 128, 56, 100, 0

LTEXT           "Out 1 (L)", IDC_STATIC, 20, 195, 60, 20
COMBOBOX        IDC_COMBO_AUDIO_OUT_L, 20, 210, 46, 100, CBS_DROPDOWNLIST

LTEXT           "Out 2 (R)", IDC_STATIC, 75, 195, 60, 20
COMBOBOX        IDC_COMBO_AUDIO_OUT_R, 75, 210, 46, 100, CBS_DROPDOWNLIST

LTEXT           "IO Vector Size", IDC_STATIC, 200, 32, 80, 20
COMBOBOX        IDC_COMBO_AUDIO_IOVS, 200, 50, 90, 100, CBS_DROPDOWNLIST

LTEXT           "Signal Vector Size", IDC_STATIC, 200, 75, 100, 20
COMBOBOX        IDC_COMBO_AUDIO_SIGVS, 200, 90, 90, 100, CBS_DROPDOWNLIST

LTEXT           "Sampling Rate", IDC_STATIC, 200, 115, 80, 20
COMBOBOX        IDC_COMBO_AUDIO_SR, 200, 130, 90, 100, CBS_DROPDOWNLIST

PUSHBUTTON      "Audio Midi Setup...", IDC_BUTTON_ASIO, 180, 170, 110, 40

GROUPBOX        "MIDI Settings", IDC_STATIC, 5, 255, 300, 120

LTEXT           "Input Device", IDC_STATIC, 20, 275, 100, 20
COMBOBOX        IDC_COMBO_MIDI_IN_DEV, 20, 293, 150, 100, CBS_DROPDOWNLIST

LTEXT           "Output Device", IDC_STATIC, 20, 320, 100, 20
COMBOBOX        IDC_COMBO_MIDI_OUT_DEV, 20, 338, 150, 100, CBS_DROPDOWNLIST

LTEXT           "Input Channel", IDC_STATIC, 200, 275, 100, 20
COMBOBOX        IDC_COMBO_MIDI_IN_CHAN, 200, 293, 90, 100, CBS_DROPDOWNLIST

LTEXT           "Output Channel", IDC_STATIC, 200, 320, 100, 20
COMBOBOX        IDC_COMBO_MIDI_OUT_CHAN, 200, 338, 90, 100, CBS_DROPDOWNLIST

DEFPUSHBUTTON   "OK", IDOK, 192, 383, 50, 20
PUSHBUTTON      "Apply", IDAPPLY, 132, 383, 50, 20
PUSHBUTTON      "Cancel", IDCANCEL, 252, 383, 50, 20
END
SWELL_DEFINE_DIALOG_RESOURCE_END(IDD_DIALOG_PREF)

#include "swell-menugen.h"

SWELL_DEFINE_MENU_RESOURCE_BEGIN(IDR_MENU1)
POPUP "&File"
BEGIN
// MENUITEM SEPARATOR
MENUITEM "Preferences...", ID_PREFERENCES
MENUITEM "&Quit", ID_QUIT
END
POPUP "&Help"
BEGIN
MENUITEM "&About", ID_ABOUT
END
SWELL_DEFINE_MENU_RESOURCE_END(IDR_MENU1)

#endif