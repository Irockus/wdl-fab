#include "app_main.h"
#include "app_resource.h"
#include "IGraphics.h"

// #include "../resource.h"

#if _MSC_VER >=1500
#pragma comment(lib, "comctl32.lib")
#endif


#ifdef OS_WIN
  #include <windows.h>
  #include <shlobj.h>
  #include <sys/stat.h>
#endif

AppWrapper* AppWrapper::_instance = 0;


unsigned int gIOVS = 512;
unsigned int gSigVS = 32;
unsigned int gBufIndex = 0; // Loops 0 to SigVS
unsigned int gVecElapsed = 0;
double gFadeMult = 0.; // Fade multiplier

extern HWND gHWND;

AppWrapper::AppWrapper()
{
	gHWND = 0;

	gHINST = 0;
	gINIPath = new char[MAX_APP_INIPATH];
	gScrollMessage = 0;
	gPluginInstance = 0;
	gDAC = 0;
	gMidiIn = 0;
	gMidiOut = 0;

	gState = new AppState();
	gTempState = 0;
	gActiveState = 0;

}

AppWrapper::~AppWrapper()
{
	SAFE_DEL_ARR(gINIPath);
}

void AppWrapper::Error(const char *msg)
{
#if WIN32
	OutputDebugString(msg);
#else
	std::cerr << msg << std::endl;
#endif
}


bool AppWrapper::AttachGUI()
{
    IGraphics* pGraphics = gPluginInstance->GetGUI();
    
    if (pGraphics)
    {
#ifdef OS_WIN
        if (!pGraphics->OpenWindow(gHWND))
            pGraphics=0;
#else // Cocoa OSX
        if (!pGraphics->OpenWindow(gHWND))
            pGraphics=0;
#endif
        if (pGraphics)
        {
            gPluginInstance->OnGUIOpen();
            return true;
        }
    }
    
    return false;
}

void AppWrapper::Init()
{
    TryToChangeAudioDriverType(); // will init RTAudio with an API type based on gState->mAudioDriverType
    ProbeAudioIO(); // find out what audio IO devs are available and put their IDs in the global variables gAudioInputDevs / gAudioOutputDevs
    InitialiseMidi(); // creates RTMidiIn and RTMidiOut objects
    ProbeMidiIO(); // find out what midi IO devs are available and put their names in the global variables gMidiInputDevs / gMidiOutputDevs
    
    // Initialise the plugin
    gPluginInstance = MakePlug(gMidiOut, &gState->mMidiOutChan);
    gPluginInstance->RestorePreset(0);
    
    ChooseMidiInput(gState->mMidiInDev);
    ChooseMidiOutput(gState->mMidiOutDev);
    
    TryToChangeAudio();
}

void AppWrapper::Cleanup()
{
    try
    {
        // Stop the stream
        gDAC->stopStream();
    }
    catch (RtError& e)
    {
        e.printMessage();
    }
    
    gMidiIn->cancelCallback();
    gMidiIn->closePort();
    gMidiOut->closePort();
    
    if ( gDAC->isStreamOpen() ) gDAC->closeStream();
    
    SAFE_DEL(gPluginInstance);
    SAFE_DEL(gState);
    SAFE_DEL(gTempState);
    SAFE_DEL(gActiveState);
    SAFE_DEL(gMidiIn);
    SAFE_DEL(gMidiOut);
    SAFE_DEL(gDAC);
    SAFE_DEL_ARR(gINIPath);
}

void AppWrapper::UpdateINI()
{
  char buf[100]; // temp buffer for writing integers to profile strings

  snprintf(buf, sizeof(buf), "%u", gState->mAudioDriverType);
  WritePrivateProfileString("audio", "driver", buf, gINIPath);

  WritePrivateProfileString("audio", "indev", gState->mAudioInDev, gINIPath);
  WritePrivateProfileString("audio", "outdev", gState->mAudioOutDev, gINIPath);

  snprintf(buf, sizeof(buf), "%u", gState->mAudioInChanL);
  WritePrivateProfileString("audio", "in1", buf, gINIPath);
  snprintf(buf, sizeof(buf), "%u", gState->mAudioInChanR);
  WritePrivateProfileString("audio", "in2", buf, gINIPath);
  snprintf(buf, sizeof(buf), "%u", gState->mAudioOutChanL);
  WritePrivateProfileString("audio", "out1", buf, gINIPath);
  snprintf(buf, sizeof(buf), "%u", gState->mAudioOutChanR);
  WritePrivateProfileString("audio", "out2", buf, gINIPath);
  snprintf(buf, sizeof(buf), "%u", gState->mAudioInIsMono);
  WritePrivateProfileString("audio", "monoinput", buf, gINIPath);

  WritePrivateProfileString("audio", "iovs", gState->mAudioIOVS, gINIPath);
  WritePrivateProfileString("audio", "sigvs", gState->mAudioSigVS, gINIPath);

  WritePrivateProfileString("audio", "sr", gState->mAudioSR, gINIPath);

  WritePrivateProfileString("midi", "indev", gState->mMidiInDev, gINIPath);
  WritePrivateProfileString("midi", "outdev", gState->mMidiOutDev, gINIPath);

  snprintf(buf, sizeof(buf), "%u", gState->mMidiInChan);
  WritePrivateProfileString("midi", "inchan", buf, gINIPath);
  snprintf(buf, sizeof(buf), "%u", gState->mMidiOutChan);
  WritePrivateProfileString("midi", "outchan", buf, gINIPath);
}

// returns the device name. Core Audio device names are truncated
std::string AppWrapper::GetAudioDeviceName(int idx)
{
  return gAudioIDDevNames.at(idx);
}

// returns the rtaudio device ID, based on the (truncated) device name
int AppWrapper::GetAudioDeviceID(char* deviceNameToTest)
{
  TRACE;

  for(int i = 0; i < gAudioIDDevNames.size(); i++)
  {
    if(!strcmp(deviceNameToTest, gAudioIDDevNames.at(i).c_str() ))
      return i;
  }

  return -1;
}

unsigned int AppWrapper::GetMIDIInPortNumber(const char* nameToTest)
{
  int start = 1;

  if(!strcmp(nameToTest, "off")) return 0;

  #ifdef OS_OSX
  start = 2;
  if(!strcmp(nameToTest, "virtual input")) return 1;
  #endif

  for (int i = 0; i < gMidiIn->getPortCount(); i++)
  {
    if(!strcmp(nameToTest, gMidiIn->getPortName(i).c_str()))
      return (i + start);
  }

  return -1;
}

unsigned int AppWrapper::GetMIDIOutPortNumber(const char* nameToTest)
{
  int start = 1;

  if(!strcmp(nameToTest, "off")) return 0;

  #ifdef OS_OSX
  start = 2;
  if(!strcmp(nameToTest, "virtual output")) return 1;
  #endif

  for (int i = 0; i < gMidiOut->getPortCount(); i++)
  {
    if(!strcmp(nameToTest, gMidiOut->getPortName(i).c_str()))
      return (i + start);
  }

  return -1;
}

// find out which devices have input channels & which have output channels, add their ids to the lists
void AppWrapper::ProbeAudioIO()
{
  TRACE;

  RtAudio::DeviceInfo info;

  gAudioInputDevs.clear();
  gAudioOutputDevs.clear();
  gAudioIDDevNames.clear();

  unsigned int nDevices = gDAC->getDeviceCount();

  for (int i=0; i<nDevices; i++)
  {
    info = gDAC->getDeviceInfo(i);
    std::string deviceName = info.name;

    #ifdef OS_OSX
    size_t colonIdx = deviceName.rfind(": ");

    if(colonIdx != std::string::npos && deviceName.length() >= 2)
      deviceName = deviceName.substr(colonIdx + 2, deviceName.length() - colonIdx - 2);

    #endif

    gAudioIDDevNames.push_back(deviceName);

    if ( info.probed == false )
      std::cout << deviceName << ": Probe Status = Unsuccessful\n";
    else if ( !strcmp("Generic Low Latency ASIO Driver", deviceName.c_str() ))
      std::cout << deviceName << ": Probe Status = Unsuccessful\n";
    else
    {
      if(info.inputChannels > 0)
        gAudioInputDevs.push_back(i);

      if(info.outputChannels > 0)
        gAudioOutputDevs.push_back(i);
    }
  }
}

void AppWrapper::ProbeMidiIO()
{
  if ( !gMidiIn || !gMidiOut )
    return;
  else
  {
    int nInputPorts = gMidiIn->getPortCount();

    gMIDIInputDevNames.push_back("off");

    #ifndef OS_WIN
    gMIDIInputDevNames.push_back("virtual input");
    #endif

    for (int i=0; i<nInputPorts; i++ )
    {
      gMIDIInputDevNames.push_back(gMidiIn->getPortName(i));
    }

    int nOutputPorts = gMidiOut->getPortCount();

    gMIDIOutputDevNames.push_back("off");

#ifndef _WIN32
    gMIDIOutputDevNames.push_back("virtual output");
#endif

    for (int i=0; i<nOutputPorts; i++ )
    {
      gMIDIOutputDevNames.push_back(gMidiOut->getPortName(i));
      //This means the virtual output port wont be added as an input
    }
  }
}

bool  AppWrapper::AudioSettingsInStateAreEqual(AppState* os, AppState* ns)
{
  if (os->mAudioDriverType != ns->mAudioDriverType) return false;
  if (strcmp(os->mAudioInDev, ns->mAudioInDev)) return false;
  if (strcmp(os->mAudioOutDev, ns->mAudioOutDev)) return false;
  if (strcmp(os->mAudioSR, ns->mAudioSR)) return false;
  if (strcmp(os->mAudioIOVS, ns->mAudioIOVS)) return false;
  if (strcmp(os->mAudioSigVS, ns->mAudioSigVS)) return false;
  if (os->mAudioInChanL != ns->mAudioInChanL) return false;
  if (os->mAudioInChanR != ns->mAudioInChanR) return false;
  if (os->mAudioOutChanL != ns->mAudioOutChanL) return false;
  if (os->mAudioOutChanR != ns->mAudioOutChanR) return false;
  if (os->mAudioInIsMono != ns->mAudioInIsMono) return false;

  return true;
}

bool  AppWrapper::MIDISettingsInStateAreEqual(AppState* os, AppState* ns)
{
  if (strcmp(os->mMidiInDev, ns->mMidiInDev)) return false;
  if (strcmp(os->mMidiOutDev, ns->mMidiOutDev)) return false;
  if (os->mMidiInChan != ns->mMidiInChan) return false;
  if (os->mMidiOutChan != ns->mMidiOutChan) return false;

  return true;
}

void AppWrapper::MIDICallback( double deltatime, std::vector< unsigned char > *message, void *userData )
{
  if ( message->size() )
  {
    IMidiMsg *myMsg;

    switch (message->size())
    {
      case 1:
        myMsg = new IMidiMsg(0, message->at(0), 0, 0);
        break;
      case 2:
        myMsg = new IMidiMsg(0, message->at(0), message->at(1), 0);
        break;
      case 3:
        myMsg = new IMidiMsg(0, message->at(0), message->at(1), message->at(2));
        break;
      default:
        DBGMSG("NOT EXPECTING %d midi callback msg len\n", (int) message->size());
        break;
    }

    IMidiMsg msg(*myMsg);

    delete myMsg;

    // filter midi messages based on channel, if Instance().gStatus.mMidiInChan != all (0)
    if (Instance().gState->mMidiInChan)
    {
		if (Instance().gState->mMidiInChan == msg.Channel() + 1)
			Instance().gPluginInstance->ProcessMidiMsg(&msg);
    }
    else
    {
		Instance().gPluginInstance->ProcessMidiMsg(&msg);
    }
  }
}

int AppWrapper::AudioCallback(void *outputBuffer,
                  void *inputBuffer,
                  unsigned int nFrames,
                  double streamTime,
                  RtAudioStreamStatus status,
                  void *userData )
{
  if ( status )
    std::cout << "Stream underflow detected!" << std::endl;

  double* inputBufferD = (double*)inputBuffer;
  double* outputBufferD = (double*)outputBuffer;

  int inRightOffset = 0;

  if(!Instance().gState->mAudioInIsMono)
    inRightOffset = nFrames;

  if (gVecElapsed > N_VECTOR_WAIT) // wait N_VECTOR_WAIT * iovs before processing audio, to avoid clicks
  {
    for (int i=0; i<nFrames; i++)
    {
		Instance().gBufIndex %= Instance().gSigVS;

		if (Instance().gBufIndex == 0)
      {
        double* inputs[2] = {inputBufferD + i, inputBufferD + inRightOffset + i};
        double* outputs[2] = {outputBufferD + i, outputBufferD + nFrames + i};

		Instance().gPluginInstance->LockMutexAndProcessDoubleReplacing(inputs, outputs, Instance().gSigVS);
      }

      // fade in
      if (gFadeMult < 1.)
      {
        gFadeMult += (1. / nFrames);
      }

      outputBufferD[i] *= gFadeMult;
      outputBufferD[i + nFrames] *= gFadeMult;

      outputBufferD[i] *= APP_MULT;
      outputBufferD[i + nFrames] *= APP_MULT;

	  Instance().gBufIndex++;
    }
  }
  else
  {
    memset(outputBuffer, 0, nFrames * 2 * sizeof(double));
  }

  gVecElapsed++;

  return 0;
}

bool AppWrapper::TryToChangeAudioDriverType()
{
  TRACE;

  if (gDAC)
  {
    if (gDAC->isStreamOpen())
    {
      gDAC->closeStream();
    }

    DELETE_NULL(gDAC);
  }

#ifdef OS_WIN
  if(gState->mAudioDriverType == DAC_ASIO)
    gDAC = new RtAudio(RtAudio::WINDOWS_ASIO);
  else
    gDAC = new RtAudio(RtAudio::WINDOWS_DS);
#elif defined OS_OSX
  if(gState->mAudioDriverType == DAC_COREAUDIO)
    gDAC = new RtAudio(RtAudio::MACOSX_CORE);
  //else
  //gDAC = new RtAudio(RtAudio::UNIX_JACK);
#endif

  if(gDAC)
    return true;
  else
    return false;
}

bool AppWrapper::TryToChangeAudio()
{
  TRACE;

  int inputID = -1;
  int outputID = -1;

#ifdef OS_WIN
  if(gState->mAudioDriverType == DAC_ASIO)
    inputID = GetAudioDeviceID(gState->mAudioOutDev);
  else
    inputID = GetAudioDeviceID(gState->mAudioInDev);
#else
  inputID = GetAudioDeviceID(gState->mAudioInDev);
#endif

  outputID = GetAudioDeviceID(gState->mAudioOutDev);

  int samplerate = atoi(gState->mAudioSR);
  int iovs = atoi(gState->mAudioIOVS);

  if (inputID != -1 && outputID != -1)
  {
    return InitialiseAudio(inputID, outputID, samplerate, iovs, NUM_CHANNELS, gState->mAudioInChanL - 1, gState->mAudioOutChanL - 1);
  }

  return false;
}

bool AppWrapper::InitialiseAudio(unsigned int inId,
                     unsigned int outId,
                     unsigned int sr,
                     unsigned int iovs,
                     unsigned int chnls,
                     unsigned int inChanL,
                     unsigned int outChanL
                    )
{
  TRACE;

  if (gDAC->isStreamOpen())
  {
    if (gDAC->isStreamRunning())
    {
      try
      {
        gDAC->abortStream();
      }
      catch (RtError& e)
      {
        e.printMessage();
      }
    }

    gDAC->closeStream();
  }

  RtAudio::StreamParameters iParams, oParams;
  iParams.deviceId = inId;
  iParams.nChannels = chnls;
  iParams.firstChannel = inChanL;

  oParams.deviceId = outId;
  oParams.nChannels = chnls;
  oParams.firstChannel = outChanL;

  gIOVS = iovs; // gIOVS may get changed by stream
  gSigVS = atoi(gState->mAudioSigVS); // This is done here so that it changes when the callback is stopped

  DBGMSG("\ntrying to start audio stream @ %i sr, %i iovs, %i sigvs\nindev = %i:%s\noutdev = %i:%s\n", sr, iovs, gSigVS, inId, GetAudioDeviceName(inId).c_str(), outId, GetAudioDeviceName(outId).c_str());

  RtAudio::StreamOptions options;
  options.flags = RTAUDIO_NONINTERLEAVED;
// options.streamName = BUNDLE_NAME; // JACK stream name, not used on other streams

  gBufIndex = 0;
  gVecElapsed = 0;
  gFadeMult = 0.;

  gPluginInstance->SetBlockSize(gSigVS);
  gPluginInstance->SetSampleRate(sr);
  gPluginInstance->Reset();

  try
  {
    TRACE;
    gDAC->openStream( &oParams, &iParams, RTAUDIO_FLOAT64, sr, &gIOVS, &AudioCallback, NULL, &options);
    gDAC->startStream();

    memcpy(gActiveState, gState, sizeof(AppState)); // copy state to active state
  }
  catch ( RtError& e )
  {
    e.printMessage();
    return false;
  }

  return true;
}

bool AppWrapper::InitialiseMidi()
{
  try
  {
    gMidiIn = new RtMidiIn();
  }
  catch ( RtError &error )
  {
	DELETE_NULL(gMidiIn);
    error.printMessage();
    return false;
  }

  try
  {
    gMidiOut = new RtMidiOut();
  }
  catch ( RtError &error )
  {
	DELETE_NULL(gMidiOut);
    error.printMessage();
    return false;
  }

  gMidiIn->setCallback( &MIDICallback );
  gMidiIn->ignoreTypes( !ENABLE_SYSEX, !ENABLE_MIDICLOCK, !ENABLE_ACTIVE_SENSING );

  return true;
}

bool AppWrapper::ChooseMidiInput(const char* pPortName)
{
  unsigned int port = GetMIDIInPortNumber(pPortName);

  if(port == -1)
  {
    strcpy(gState->mMidiInDev, "off");
    UpdateINI();
    port = 0;
  }
  /*
  IMidiMsg msg;
  msg.MakeControlChangeMsg(IMidiMsg::kAllNotesOff, 127, 0);

  std::vector<unsigned char> message;
  message.push_back( msg.mStatus );
  message.push_back( msg.mData1 );
  message.push_back( msg.mData2 );

  gPluginInstance->ProcessMidiMsg(&msg);
  */
  if (gMidiIn)
  {
    gMidiIn->closePort();

    if (port == 0)
    {
      return true;
    }
    #ifdef OS_WIN
    else
    {
      gMidiIn->openPort(port-1);
      return true;
    }
    #else
    else if(port == 1)
    {
      std::string virtualMidiInputName = "To ";
      virtualMidiInputName += gPluginInstance->GetEffectName();
      gMidiIn->openVirtualPort(virtualMidiInputName);
      return true;
    }
    else
    {
      gMidiIn->openPort(port-2);
      return true;
    }
    #endif
  }

  return false;
}

bool AppWrapper::ChooseMidiOutput(const char* pPortName)
{
  unsigned int port = GetMIDIOutPortNumber(pPortName);

  if(port == -1)
  {
    strcpy(gState->mMidiOutDev, "off");
    UpdateINI();
    port = 0;
  }

  if (gMidiOut)
  {
    /*
    IMidiMsg msg;
    msg.MakeControlChangeMsg(IMidiMsg::kAllNotesOff, 127, 0);

    std::vector<unsigned char> message;
    message.push_back( msg.mStatus );
    message.push_back( msg.mData1 );
    message.push_back( msg.mData2 );

    gMidiOut->sendMessage( &message );
    */
    gMidiOut->closePort();

    if (port == 0)
    {
      return true;
    }
    #ifdef OS_WIN
    else
    {
      gMidiOut->openPort(port-1);
      return true;
    }
    #else
    else if(port == 1)
    {
      std::string virtualMidiOutputName = "From ";
      virtualMidiOutputName += gPluginInstance->GetEffectName();;
      gMidiOut->openVirtualPort(virtualMidiOutputName);
      return true;
    }
    else
    {
      gMidiOut->openPort(port-2);
      return true;
    }
    #endif
  }

  return false;
}

