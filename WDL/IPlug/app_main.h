#ifndef _IPLUGAPP_APP_MAIN_H_
#define _IPLUGAPP_APP_MAIN_H_

#include "IPlugOSDetect.h"

/*

 Standalone osx/win app wrapper for iPlug, using SWELL
 Oli Larkin 2012

 Fabien : Refactored as a reusable class wrapper to make it reusable in the IPlug framework

 Notes:

 App settings are stored in a .ini file. The location is as follows:

 Windows7: C:\Users\USERNAME\AppData\Local\<IPlugName>\settings.ini
 Windows XP/Vista: C:\Documents and Settings\USERNAME\Local Settings\Application Data\<IPlugName>\settings.ini
 OSX: /Users/USERNAME/Library/Application\ Support/<IPlugName>/settings.ini

*/

#ifdef OS_WIN
#include <windows.h>
#include <commctrl.h>

#define DEFAULT_INPUT_DEV "Default Device"
#define DEFAULT_OUTPUT_DEV "Default Device"

#define DAC_DS 0
#define DAC_ASIO 1
#elif defined OS_OSX
#include "swell.h"
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )

#define DEFAULT_INPUT_DEV "Built-in Input"
#define DEFAULT_OUTPUT_DEV "Built-in Output"

#define DAC_COREAUDIO 0
//  #define DAC_JACK 1
#endif

#include <../wdltypes.h>
#include "../rtaudiomidi/RtAudio.h"
#include "../rtaudiomidi/RtMidi.h"
#include <IPlugStandAlone.h>

#include <string>
#include <vector>

typedef unsigned short UInt16;

#define MAX_APP_INIPATH 261

/// Application state data structure used by the AppWrapper singleton instance.
struct AppState
{
  UInt16 mAudioDriverType;   ///< on osx core audio 0 or jack 1, on windows DS 0 or ASIO 1

  // strings
  char mAudioInDev[100];  ///< Audio input dev name
  char mAudioOutDev[100]; ///< Audio output dev name
  char mAudioSR[100]; ///< Audio Sample rate string (i.e. "44100")
  char mAudioIOVS[100]; ///< IO buffer size in samples
  char mAudioSigVS[100]; ///< Sig buffer size

  UInt16 mAudioInChanL;  ///< Left Midi in channel number
  UInt16 mAudioInChanR;  ///< Right Midi in channel number
  UInt16 mAudioOutChanL; ///< Left Midi out channel number
  UInt16 mAudioOutChanR; ///< Right Midi out channel number
  UInt16 mAudioInIsMono; ///< Mono audio in flag

  // strings containing the names of the midi devices
  char mMidiInDev[100];		///< Midi input dev name
  char mMidiOutDev[100];	///< Midi output dev name

  UInt16 mMidiInChan; ///< Midi input channel
  UInt16 mMidiOutChan; ///< Midi output channel

  AppState():
    mAudioDriverType(0), // DS / CoreAudio by default
    mAudioInChanL(1),
    mAudioInChanR(2),
    mAudioOutChanL(1),
    mAudioOutChanR(2),
    mMidiInChan(0),
    mMidiOutChan(0)
  {
    strcpy(mAudioInDev, DEFAULT_INPUT_DEV);
    strcpy(mAudioOutDev, DEFAULT_OUTPUT_DEV);
    strcpy(mAudioSR, "44100");
    strcpy(mAudioIOVS, "512");
    strcpy(mAudioSigVS, "32");

    strcpy(mMidiInDev, "off");
    strcpy(mMidiOutDev, "off");
  }
};

/**
 * Encapsulate all the wrapping the API needed to be called by
 * the WinMain entry point and also of the various windows and audio/midi procs
 */
class AppWrapper
{
  static AppWrapper* _instance;

#ifdef WIN32
  friend int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nShowCmd);
#else
  friend INT_PTR SWELLAppMain(int msg, INT_PTR parm1, INT_PTR parm2);
#endif

  static  WDL_DLGRET MainDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
  static  WDL_DLGRET PreferencesDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
  static void MIDICallback(double deltatime, std::vector< unsigned char > *message, void *userData);
  static int AudioCallback(void *outputBuffer,
                           void *inputBuffer,
                           unsigned int nFrames,
                           double streamTime,
                           RtAudioStreamStatus status,
                           void *userData);

protected:
  HINSTANCE gHINST;
  IPlugStandalone* gPluginInstance; // The iplug plugin instance
  UINT gScrollMessage;

  RtAudio* gDAC;
  RtMidiIn *gMidiIn;
  RtMidiOut *gMidiOut;

  AppState *gState;
  AppState *gTempState; // The state is copied here when the pref dialog is opened, and restored if cancel is pressed
  AppState *gActiveState; // When the audio driver is started the current state is copied here so that if OK is pressed after APPLY nothing is changed

  unsigned int gSigVS;
  unsigned int gBufIndex; // index for signal vector, loops from 0 to gSigVS
  char *gINIPath; // path of ini file

  std::vector<unsigned int> gAudioInputDevs;
  std::vector<unsigned int> gAudioOutputDevs;
  std::vector<std::string> gMIDIInputDevNames;
  std::vector<std::string> gMIDIOutputDevNames;
  std::vector<std::string> gAudioIDDevNames;

public:
  /// Singleton Design Pattern Impl for the App Wrapper
  static AppWrapper& Instance()	{ return *(_instance ? _instance : (_instance = new AppWrapper())); }

  ~AppWrapper();

  void ProbeAudioIO();
  void ProbeMidiIO();

  static bool AudioSettingsInStateAreEqual(AppState* os, AppState* ns);
  bool TryToChangeAudioDriverType();
  bool TryToChangeAudio();
  std::string GetAudioDeviceName(int idx);
  int GetAudioDeviceID(char* deviceNameToTest);

  bool ChooseMidiInput(const char* pPortName);
  bool ChooseMidiOutput(const char* pPortName);
  static bool MIDISettingsInStateAreEqual(AppState* os, AppState* ns);
  unsigned int GetMIDIInPortNumber(const char* nameToTest);
  unsigned int GetMIDIOutPortNumber(const char* nameToTest);

  static void Error(const char *msg);

  void Init();
  void Cleanup();
  void UpdateINI();

protected:
  AppWrapper();

  void PopulateSampleRateList(HWND hwndDlg, RtAudio::DeviceInfo* inputDevInfo, RtAudio::DeviceInfo* outputDevInfo);
  void PopulateAudioInputList(HWND hwndDlg, RtAudio::DeviceInfo* info);
  void PopulateAudioOutputList(HWND hwndDlg, RtAudio::DeviceInfo* info);
  void PopulateDriverSpecificControls(HWND hwndDlg);
  void PopulateAudioDialogs(HWND hwndDlg);
  bool PopulateMidiDialogs(HWND hwndDlg);
  void PopulatePreferencesDialog(HWND hwndDlg);

  void ClientResize(HWND hWnd, int nWidth, int nHeight);

  bool InitialiseAudio(unsigned int inId,
                       unsigned int outId,
                       unsigned int sr,
                       unsigned int iovs,
                       unsigned int chnls,
                       unsigned int inChanL,
                       unsigned int outChanL
                      );


private:
  bool InitialiseMidi();
  bool AttachGUI();
};



#endif //_IPLUGAPP_APP_MAIN_H_

