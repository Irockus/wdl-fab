#ifndef _IPLUGBASE_
#define _IPLUGBASE_

#define IPLUG_VERSION 0x010000
#define IPLUG_VERSION_MAGIC 'pfft'

#include "Containers.h"
#include "IPlugStructs.h"
#include "IParam.h"
#include "Hosts.h"
#include "Log.h"
#include "NChanDelay.h"

// Uncomment to enable IPlug::OnIdle() and IGraphics::OnGUIIdle().
// #define USE_IDLE_CALLS

#define MAX_EFFECT_NAME_LEN 128
#define DEFAULT_BLOCK_SIZE 1024
#define DEFAULT_TEMPO 120.0

// All version ints are stored as 0xVVVVRRMM: V = version, R = revision, M = minor revision.

class IGraphics;

/**
Base Class for all IPlug plug-ins and the standalone application.
*/
class IPlugBase
{
public:
  /// Use IPLUG_CTOR instead of calling directly (defined in IPlug_include_in_plug_src.h).
  IPlugBase(int nParams,
            const char* channelIOStr,
            int nPresets,
            const char* effectName,
            const char* productName,
            const char* mfrName,
            int vendorVersion,
            int uniqueID,
            int mfrID,
            int latency,
            bool plugDoesMidi,
            bool plugDoesChunks,
            bool plugIsInst,
            EAPI plugAPI);

  // ----------------------------------------
  // Your plugin class implements these.
  // There are default impls, mostly just for reference.

  virtual ~IPlugBase();

   /**
    Called internally by the framework, should be overloaded with your custom re-initialization code. 
	Implementations should set a mutex lock like in the no-op!
	*/
  virtual void Reset() { TRACE; IMutexLock lock(this); }
  
  /** Called whenever a plug-in parameter was initialized or changed by the host.
		Called for all parameters on load too.
		Implementations should set a mutex lock like in the no-op!
   */
  virtual void OnParamChange(int paramIdx) { IMutexLock lock(this); }
  /** 
		Default pass-through.  Inputs and outputs are [nChannel][nSample].
		Mutex is already locked. \see ProcessSingleReplacing
   */
  virtual void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  /** 
		Single-precision version of the pass-through.  Inputs and outputs are [nChannel][nSample].
		Mutex is already locked. \see ProcessDoubleReplacing
   */
  virtual void ProcessSingleReplacing(float** inputs, float** outputs, int nFrames);
  /** Called when GUI opens, in case the audio processing thread needs to do anything when the GUI opens.
	(like for example, set some state dependent initial values for controls). \see OnGUIClose
   */
  virtual void OnGUIOpen() { TRACE; }  
  /** Called when GUI opens, in case the audio processing thread needs to do anything when the GUI opens.
	(like for example, set some state dependent initial values for controls).  \see OnGUIOpen
   */
  virtual void OnGUIClose() { TRACE; }
  /** This is an idle call from the audio processing thread, as opposed to
		IGraphics::OnGUIIdle which is called from the GUI thread.
		Only active if USE_IDLE_CALLS is defined. <BR>
		Does nothing by default.
   */
  virtual void OnIdle() {}

  /** Not usually needed ... Reset is called on activate regardless of whether this is implemented.
      Also different hosts have different interpretations of "activate".<BR>
      Implementations should set a mutex lock like in the no-op!
   */
  virtual void OnActivate(bool active) { TRACE;  IMutexLock lock(this); }

  virtual void ProcessMidiMsg(IMidiMsg* pMsg);
  virtual void ProcessSysEx(ISysEx* pSysEx) {}

  virtual bool MidiNoteName(int noteNumber, char* rName) { *rName = '\0'; return false; }

  /// Implementations should set a mutex lock and call SerializeParams() after custom data is serialized
  virtual bool SerializeState(ByteChunk* pChunk) { TRACE; return SerializeParams(pChunk); }
  /// Return the new chunk position (endPos). Implementations should set a mutex lock and call UnserializeParams() after custom data is unserialized
  virtual int UnserializeState(ByteChunk* pChunk, int startPos) { TRACE; return UnserializeParams(pChunk, startPos); }

  /// RTAS & AAX only, override in plug-ins that do chunks
  virtual bool CompareState(const unsigned char* incomingState, int startPos);

#ifndef OS_IOS
  /// iOS specific resize callback
  virtual void OnWindowResize() {}
#endif
  /// Implement this and return true to trigger your custom about box, when someone clicks about in the menu of a standalone
  virtual bool HostRequestingAboutBox() { return false; }

  /// Implement this to do something specific when IPlug is aware of the host. May get called multiple times
  virtual void OnHostIdentified() { return; };

  virtual void PopupHostContextMenuForParam(int param, int x, int y) { return; }; //! VST3 only, call it from the GUI

  // ----------------------------------------
  // Your plugin class, or a control class, can call these functions.
  /// Total number of parameters for the plugin
  int NParams() { return mParams.GetSize(); }
  /// Get an IParam object instance from its index.
  IParam* GetParam(int idx) { return mParams.Get(idx); }
  /// Get the singleton IGraphics instance, main graphics container generally used to access or populate its graphical controls.
  IGraphics* GetGUI() { return mGraphics; }

  const char* GetEffectName() { return mEffectName; } //! Return your effect name, as it will appear in the host popup menus
  int GetEffectVersion(bool decimal);   //! when decimal is true format is VVVVRRMM, otherwise it is 0xVVVVRRMM.
  void GetEffectVersionStr(char* str, size_t maxLen);//! Gets the effect version and returns it in the specified output str. \see GetEffectVersion
  /// Return your manufacturer name
  const char* GetMfrName() { return mMfrName; }
  /// Return your plug-in or standalone product name
  const char* GetProductName() { return mProductName; }
  /// Return your plugin unique ID 
  int GetUniqueID() { return mUniqueID; }
  /// Return your manufacturer unique ID 
  int GetMfrID() { return mMfrID; }

  virtual void SetParameterFromGUI(int idx, double normalizedValue);
  // If a parameter change comes from the GUI, midi, or external input,
  // the host needs to be informed in case the changes are being automated.
  virtual void BeginInformHostOfParamChange(int idx) = 0;
  virtual void InformHostOfParamChange(int idx, double normalizedValue) = 0;
  virtual void EndInformHostOfParamChange(int idx) = 0;

  virtual void InformHostOfProgramChange() = 0;

  // ----------------------------------------
  // Useful stuff for your plugin class or an outsider to call,
  // most of which is implemented by the API class.

  double GetSampleRate() { return mSampleRate; }
  int GetBlockSize() { return mBlockSize; }
  int GetLatency() { return mLatency; }

  bool GetIsBypassed() { return mIsBypassed; }

  /** @name Channel API.
		In ProcessDoubleReplacing you are always guaranteed to get valid pointers	to all the channels the plug-in requested.  
		If the host hasn't connected all the pins, the unconnected channels will be full of zeros.
   */
  /// @{
  int NInChannels() { return mInChannels.GetSize(); }
  int NOutChannels() { return mOutChannels.GetSize(); }
  bool IsInChannelConnected(int chIdx);
  bool IsOutChannelConnected(int chIdx);
  /// @}
  
  virtual bool IsRenderingOffline() { return false; };
  virtual int GetSamplePos() = 0;   // Samples since start of project.
  virtual double GetTempo() = 0;
  double GetSamplesPerBeat();
  virtual void GetTimeSig(int* pNum, int* pDenom) = 0;
  virtual void GetTime(ITimeInfo* pTimeInfo) = 0;
  virtual EHost GetHost() { return mHost; }
  virtual EAPI GetAPI() { return mAPI; }
  const char* GetAPIString();
  int GetHostVersion(bool decimal); // Decimal = VVVVRRMM, otherwise 0xVVVVRRMM.
  void GetHostVersionStr(char* str, size_t len);
  const char* GetArchString();

  // Tell the host that the graphics resized.
  // Should be called only by the graphics object when it resizes itself.
  virtual void ResizeGraphics(int w, int h) = 0;

  void EnsureDefaultPreset();

protected:
  // ----------------------------------------
  // Useful stuff for your plug-in class to call, implemented here or in the API class, or partly in both.

  /// @{ for labelling individual inputs/outputs (VST2)
  void SetInputLabel(int idx, const char* pLabel);
  void SetOutputLabel(int idx, const char* pLabel);
  
  const WDL_String* GetInputLabel(int idx) { return &(mInChannels.Get(idx)->mLabel); }
  const WDL_String* GetOutputLabel(int idx) { return &(mOutChannels.Get(idx)->mLabel); }
  /// @}
  
  // for labelling bus inputs/outputs (AU/VST3)
  void SetInputBusLabel(int idx, const char* pLabel);
  void SetOutputBusLabel(int idx, const char* pLabel);

  const WDL_String* GetInputBusLabel(int idx) { return mInputBusLabels.Get(idx); }
  const WDL_String* GetOutputBusLabel(int idx) { return mOutputBusLabels.Get(idx); }

  /// Simple channel input / output count pair structure
  struct ChannelIO
  {
    int mIn, mOut;
    ChannelIO(int nIn, int nOut) : mIn(nIn), mOut(nOut) {}
  };

  WDL_PtrList<ChannelIO> mChannelIO;
  bool LegalIO(int nIn, int nOut);    //! -1 for either means check the other value only.
  void LimitToStereoIO();

  void InitChunkWithIPlugVer(ByteChunk* pChunk);
  int GetIPlugVerFromChunk(ByteChunk* pChunk, int* pPos);

  void SetHost(const char* host, int version);   //! Version = 0xVVVVRRMM.
  virtual void HostSpecificInit() { return; };
#ifndef OS_IOS
  virtual void AttachGraphics(IGraphics* pGraphics);
#endif

  // If latency changes after initialization (often not supported by the host).
  virtual void SetLatency(int samples);
  virtual bool SendMidiMsg(IMidiMsg* pMsg) = 0;
  bool SendMidiMsgs(WDL_TypedBuf<IMidiMsg>* pMsgs);
  virtual bool SendSysEx(ISysEx* pSysEx) { return false; }
  bool IsInst() { return mIsInst; }
  bool DoesMIDI() { return mDoesMIDI; }

  /** @name  Non-Chunks Based Preset API
	You can't use these three methods with chunks-based plugins, because there is no way to set the custom data.
   */
  /// @{
  void MakeDefaultPreset(const char* name = 0, int nPresets = 1);
  /** MakePreset(name, param1, param2, ..., paramN).
   */
  void MakePreset(const char* name, ...);
  /** MakePresetFromNamedParams(name, nParamsNamed, paramEnum1, paramVal1, paramEnum2, paramVal2, ..., paramEnumN, paramVal2)
      nParamsNamed may be less than the total number of params.
   */
  void MakePresetFromNamedParams(const char* name, int nParamsNamed, ...);
  /// @}
  
   /** @name  Chunks Based Preset API
		Use these methods with chunks-based plugins
    */
  /// @{
  void MakePresetFromChunk(const char* name, ByteChunk* pChunk);
  void MakePresetFromBlob(const char* name, const char* blob, int sizeOfChunk);
  
  bool DoesStateChunks() { return mStateChunks; }
  /// Will append if the chunk is already started
  bool SerializeParams(ByteChunk* pChunk);
  int UnserializeParams(ByteChunk* pChunk, int startPos); //! Returns the new chunk position (endPos)
  /// @}

#ifndef OS_IOS
  virtual void RedrawParamControls();  // Called after restoring state.
#endif

  // ----------------------------------------
  // Internal IPlug stuff (but API classes need to get at it).

  void OnParamReset();  //! Call OnParamChange(each param) + Reset().

  void PruneUninitializedPresets();

  // Unserialize / SerializePresets - Only used by VST2
  bool SerializePresets(ByteChunk* pChunk);
  int UnserializePresets(ByteChunk* pChunk, int startPos); // Returns the new chunk position (endPos).

  // Set connection state for n channels.
  // If a channel is connected, we expect a call to attach the buffers before each process call.
  // If a channel is not connected, we attach scratch buffers now and don't need to do anything else.
  void SetInputChannelConnections(int idx, int n, bool connected);
  void SetOutputChannelConnections(int idx, int n, bool connected);

  void AttachInputBuffers(int idx, int n, double** ppData, int nFrames);
  void AttachInputBuffers(int idx, int n, float** ppData, int nFrames);
  void AttachOutputBuffers(int idx, int n, double** ppData);
  void AttachOutputBuffers(int idx, int n, float** ppData);
  void PassThroughBuffers(float sampleType, int nFrames);
  void PassThroughBuffers(double sampleType, int nFrames);
  void ProcessBuffers(float sampleType, int nFrames);
  void ProcessBuffers(double sampleType, int nFrames);
  void ProcessBuffersAccumulating(float sampleType, int nFrames);
  void ZeroScratchBuffers();

public:
  void ModifyCurrentPreset(const char* name = 0);     //! Sets the currently active preset to whatever current params are.
  int NPresets() { return mPresets.GetSize(); }
  int GetCurrentPresetIdx() { return mCurrentPresetIdx; }
  bool RestorePreset(int idx);
  bool RestorePreset(const char* name);
  const char* GetPresetName(int idx);

  virtual void DirtyPTCompareState() {}; //! needed in chunks based plug-ins to tell PT a non-indexed param changed and to turn on the compare light

  /// Dump the current state as source code for a call to MakePresetFromNamedParams / MakePresetFromBlob
  void DumpPresetSrcCode(const char* filename, const char* paramEnumNames[]);
  void DumpPresetBlob(const char* filename);

  virtual void PresetsChangedByHost() {} //! Notifies if host changed presets, does nothing by default
  void DirtyParameters(); //! hack to tell the host to dirty file state, when a preset is recalled
  
#ifndef OS_IOS
  /** @name Program and Bank API
	API to Load or Save FXP/FXB Program/Banks
  */
  /// @{
  bool SaveProgramAsFXP(const char* defaultFileName = "");
  bool SaveBankAsFXB(const char* defaultFileName = "");
  bool LoadProgramFromFXP();
  bool LoadBankFromFXB();
  /// @}
#endif

  void SetSampleRate(double sampleRate);
  virtual void SetBlockSize(int blockSize); // overridden in IPlugAU

  WDL_Mutex mMutex;
  
  /**
	Wrapping Mutex class enter and leave a critical section for the scope of that instance.
   */
  struct IMutexLock
  {
    WDL_Mutex* mpMutex;
    IMutexLock(IPlugBase* pPlug) : mpMutex(&(pPlug->mMutex)) { mpMutex->Enter(); } //! Constructor Enters the mutex
    ~IMutexLock() { if (mpMutex) { mpMutex->Leave(); } } //! Destructor Releases the mutex
    void Destroy() { mpMutex->Leave(); mpMutex = 0; }
  };

private:
  char mEffectName[MAX_EFFECT_NAME_LEN], mProductName[MAX_EFFECT_NAME_LEN], mMfrName[MAX_EFFECT_NAME_LEN];
  int mUniqueID, mMfrID, mVersion;   //  Version stored as 0xVVVVRRMM: V = version, R = revision, M = minor revision.

  EAPI mAPI;
  EHost mHost;
  int mHostVersion;   //  Version stored as 0xVVVVRRMM: V = version, R = revision, M = minor revision.

  /**
	InChannel defines an input channel label, double buffers and if it is connected
  */
  struct InChannel
  {
    bool mConnected;
    double** mSrc;   // Points into mInData.
    WDL_TypedBuf<double> mScratchBuf;
    WDL_String mLabel;
  };

  /**
	OutChannel defines an output channel label, buffers (both double and float) and if it is connected
  */
  struct OutChannel
  {
    bool mConnected;
    double** mDest;  // Points into mOutData.
    float* mFDest;
    WDL_TypedBuf<double> mScratchBuf;
    WDL_String mLabel;
  };

protected:
  bool mStateChunks, mIsInst, mDoesMIDI, mIsBypassed;
  int mCurrentPresetIdx;
  double mSampleRate;
  int mBlockSize, mLatency;
  WDL_String mPreviousPath; // for saving/loading fxps
  NChanDelayLine* mDelay; // for delaying dry signal when mLatency > 0 and plugin is bypassed
  WDL_PtrList<const char> mParamGroups;

private:
  IGraphics* mGraphics;
  WDL_PtrList<IParam> mParams;
  WDL_PtrList<IPreset> mPresets;
  WDL_TypedBuf<double*> mInData, mOutData;
  WDL_PtrList<InChannel> mInChannels;
  WDL_PtrList<OutChannel> mOutChannels;
  WDL_PtrList<WDL_String> mInputBusLabels;
  WDL_PtrList<WDL_String> mOutputBusLabels;
};

#endif
