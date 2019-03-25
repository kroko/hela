#ifndef NucleusAbstract_hpp
#define NucleusAbstract_hpp

#include "Hela.hpp"
#include "Motor.hpp"

#if defined(HELA_POSIX)
#include <csignal>
#endif

// prototype for nucleus in hela
namespace hela { namespace nucleus {

  // fallback to freerunning function if compiler has issues with pointer compatibilities
  // void signalMainForwarderNoinfo(int signum);

  // ============================================================================
  class NucleusAbstract {
    
  public:
    // Forward Nucleus inners
    class NucleusSettings;
    
  protected:
    NucleusAbstract();
    
    static void   prepareLaunch();
    static void   initialize(int argc, char const * const argv[], NucleusSettings *settings, hela::motor::Motor_S motor);
    void          executeLaunch();
    virtual void  launch() = 0; // force platorm specific
    static void   cleanupLaunch();


  public:

    static void signalMainHandlerStaticNoinfo(int signum);

  protected:
    static NucleusAbstract* s_nucleusInstance;
    static NucleusSettings* s_nucleusSettingsFromMain;
    
  public:
    
    virtual ~NucleusAbstract();
    static NucleusAbstract* getNucleus() { return s_nucleusInstance; }
    static NucleusSettings* getSettings() { return s_nucleusSettingsFromMain; }
    
    hela::motor::Motor_S getMotor() const { return m_nucleusMotor; }
    virtual void abstractSetup_();
    virtual void abstractUpdate_();
    virtual void abstractDraw_();
    virtual void abstractCleanup_();
    
    virtual void quit() = 0;
    virtual bool eventHasKeyboard(bool fakeReturnValue) const = 0;
    virtual bool eventHasMouse(bool fakeReturnValue) const = 0;
    
  public:
    
    virtual void setup() {}
    virtual void update() {}
    virtual void draw() {}
    virtual void cleanup() {}
    
    virtual void eventKeyboardKey(unsigned short /*key*/, int /*action*/) {}
    virtual void eventMousePositionDelta(int /*xpos*/, int /*ypos*/) {}
    virtual void eventMouseButton(unsigned short /*button*/, int /*action*/) {}
    virtual void eventMouseWheel(int /*xoffset*/, int /*yoffset*/) {}
    
    // Irrelevant for Allwinner, implemented for devbench
    virtual void eventMousePositionAbs(double /*xpos*/, double /*ypos*/) {}
    virtual void eventWindowResize(int /*width*/, int /*height*/) {}
    virtual void eventWindowDrop(int /*count*/, const char** /*paths*/) {}
    
  public:
    
    float getElapsedSeconds() const {
      return abstractTimeGetCurrentSeconds();
    }
    uint32_t getElapsedFrames() const {
      return m_frameCount;
    }
    std::string getBinaryExecPathString() const {
      return m_nucleusBinaryExecPath;
    }
    std::string getBinaryDirectoryPathString() const {
      return m_nucleusBinaryDirecoryPath;
    }
    std::string getAssetDirectoryPathString() const {
      return m_AssetDirectoryPath;
    }
    void setAssetDirectoryPathString(std::string path) {
      m_AssetDirectoryPath = path;
    }
    std::string constructAssetPathString(const std::string& path) const {
      return m_AssetDirectoryPath + path;
    }
    virtual void setCaptureCursor(bool /*capture*/ = true) { }
    virtual bool isCaptureCursor() const { return true; }
    virtual void setFullScreen(bool /*fullScreen*/ = true) { }
    virtual bool isFullScreen() const { return true; }
    
  private:
    hela::motor::Motor_S m_nucleusMotor;
    
    std::vector<std::string> m_nucleusCommandLineArgsRawSplit;

    uint32_t m_frameCount = 0;
    int64_t m_abstractTimeStartMarkerEpochMs = 0;
    
    // use since epoch as that might come handy
    int64_t abstractTimeGetEpochMs() const {
      return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
    
    // do float not double on Allwinner
    float abstractTimeGetCurrentSeconds() const {
      return static_cast<float>(abstractTimeGetEpochMs() - m_abstractTimeStartMarkerEpochMs) / 1000.0f;
    }
    
    std::string m_nucleusBinaryExecPath;
    std::string m_nucleusBinaryDirecoryPath;
    std::string m_AssetDirectoryPath;
    
    // ----------------------------------------------------------------------------
    // Perf stats structure
    
  private:
    struct PerfStats_t {
      int64_t startMs;
      int64_t endMs;
      int64_t maxMs;
      int64_t minMs;
      uint64_t count;
      int64_t maxFirst;
      int64_t maxSecond;
      int64_t currMs;
      int64_t prevMs;
      int64_t deltaCurrMs;
      uint16_t ignoredFramesCurr;
      uint16_t ignoredFramesStartLimit;
      PerfStats_t():
      startMs(0),
      endMs(0),
      maxMs(0),
      minMs(INT_MAX),
      count(0),
      maxFirst(0),
      maxSecond(0),
      currMs(0),
      prevMs(0),
      deltaCurrMs(0),
      ignoredFramesCurr(0),
      ignoredFramesStartLimit(10) {}
    } m_perfStats;
    
    // ----------------------------------------------------------------------------
    // NucleusSettings
  public:
    
    typedef std::function<void (NucleusSettings *settings)>  NucleusSettingsFn;
    
    class NucleusSettings {
      
      //: friends
      friend NucleusAbstract;
      
    public:
      
      // --------------------------------
      NucleusSettings() :
      m_windowSize(hela::vec::vec2<uint16_t>(640, 480)),
      m_userInputAllowed(true),
      m_perfStatsEnabled(false),
      m_windowFitMonitor(false),
      m_fullScreen(false),
      m_resizable(true),
      m_borderless(false),
      m_alwaysOnTop(false),
      m_captureCursor(false),
      m_title("My name is Hela") {
        spdlog::trace("NucleusAbstract::NucleusSettings construct");
      }
      
      virtual ~NucleusSettings();
      
      // --------------------------------
      hela::motor::Motor_S getMotor() const {
        return m_nucleusSettingsMotor;
      }
      
      // --------------------------------
      // Relevant for FBdev
      // This is the main mechanism how user passes stuff to your app from console
      // It is expected that user will pass and you should implement sertting window size andm (if exists) maybe deferred buffer size
      // These are raw arguments!
      const std::vector<std::string>& getCommandLineArgsRawSplit() const {
        return m_nucleusSettingsCommandLineArgsRawSplit;
      }
      void setCommandLineArgsRawSplit(const std::vector<std::string> &commandLineArgsRawSplit) {
        m_nucleusSettingsCommandLineArgsRawSplit = commandLineArgsRawSplit;
      }
      
      // --------------------------------
      // Relevant for FBdev
      // You can pass in app what you wish for, however...
      // ... user could and should pass command line arguments that take precedence over this
      void setWindowSize(const hela::vec::vec2<uint16_t>& size) {
        m_windowSize = hela::vec::vec2<uint16_t>(size.x, size.y);
      }
      void setWindowSize(uint16_t windowSizeX, uint16_t windowSizeY) {
        m_windowSize = hela::vec::vec2<uint16_t>(windowSizeX, windowSizeY);
      }
      hela::vec::vec2<uint16_t> getWindowSize() const {
        return m_windowSize;
      }

      // --------------------------------
      // Relevant for FBdev
      void setUserInputAllowed(bool allowed = true) {
        m_userInputAllowed = allowed;
      }
      bool isUserInputAllowed() const {
        return m_userInputAllowed;
      }

      // --------------------------------
      // Relevant for FBdev
      void setPerfStats(bool enable = true) {
        m_perfStatsEnabled = enable;
      }
      bool isPerfStats() const {
        return m_perfStatsEnabled;
      }
      
      // --------------------------------
      // Irrelevant for FBdev
      // Implemented for devbench
      void setWindowPos(const hela::vec::vec2<int16_t>& windowPos) {
        m_windowPos = hela::vec::vec2<int16_t>(windowPos.x, windowPos.y);
      }
      void setWindowPos(int16_t windowPosX, int16_t windowPosY) {
        m_windowPos = hela::vec::vec2<int16_t>(windowPosX, windowPosY);
      }
      hela::vec::vec2<int16_t> getWindowPos() const {
        return m_windowPos;
      }
      
      // --------------------------------
      // Irrelevant for FBdev
      // Implemented for devbench
      void setWindowFitMonitor(bool fit = true) {
        m_windowFitMonitor = fit;
      }
      bool isWindowFitMonitor() const {
        return m_windowFitMonitor;
      }
      
      // --------------------------------
      // Irrelevant for FBdev
      // Implemented for devbench
      void setFullScreen(bool fullScreen = true) {
        m_fullScreen = fullScreen;
      }
      bool isFullScreen() const {
        return m_fullScreen;
      }
      
      // --------------------------------
      // Irrelevant for FBdev
      // Implemented for devbench
      void setResizable(bool resizable = true) {
        m_resizable = resizable;
      }
      bool isResizable() const {
        return m_resizable;
      }
      
      // --------------------------------
      // Irrelevant for FBdev
      // Implemented for devbench
      void setBorderless(bool borderless = true) {
        m_borderless = borderless;
      }
      bool isBorderless() const {
        return m_borderless;
      }
      
      // --------------------------------
      // Irrelevant for FBdev
      // Implemented for devbench
      void setAlwaysOnTop(bool alwaysOnTop = true) {
        m_alwaysOnTop = alwaysOnTop;
      }
      bool  isAlwaysOnTop() const {
        return m_alwaysOnTop;
      }
      
      // --------------------------------
      // Irrelevant for FBdev
      // Implemented for devbench
      void setCaptureCursor(bool capture = true) {
        m_captureCursor = capture;
      }
      bool  isCaptureCursor() const {
        return m_captureCursor;
      }
      
      
      // --------------------------------
      // Irrelevant for FBdev
      // Implemented for devbench
      void setTitle(const std::string &title) {
        m_title = title;
      }
      const std::string& getTitle() const {
        return m_title;
      }
      
    protected:
      void init(int argc, char const * const argv[], const hela::motor::Motor_S& motor);
      
      hela::motor::Motor_S      m_nucleusSettingsMotor;
      std::vector<std::string>  m_nucleusSettingsCommandLineArgsRawSplit;
      
      // Window stuff
      hela::vec::vec2<uint16_t> m_windowSize;
      bool                      m_userInputAllowed;
      bool                      m_perfStatsEnabled;
      hela::vec::vec2<int16_t>  m_windowPos;
      bool                      m_windowFitMonitor;
      bool                      m_fullScreen;
      bool                      m_resizable;
      bool                      m_borderless;
      bool                      m_alwaysOnTop;
      bool                      m_captureCursor;
      std::string               m_title;
      
    }; // NucleusAbstract::NucleusSettings
    
  }; // NucleusAbstract
  
  // ============================================================================
  // Helpers outside class, so we can access them directly from namespace
  // This also serves as a list of what user code might normally ask Hela
  
  // ----------------------------------------------------------------------------
  // Main loop timer and frames

  inline float getElapsedSeconds() {
    return NucleusAbstract::getNucleus()->getElapsedSeconds();
  }
  inline uint32_t getElapsedFrames() {
    return NucleusAbstract::getNucleus()->getElapsedFrames();
  }
  
  // ----------------------------------------------------------------------------
  // Window size. Query for these on setup(), because they might not be defaults or what
  // you have set in preperation setWindowSize() as end has ability to pass --width and --height
  // command line arguments (they are loaded into settings and window is created accordingly)

  inline int getWindowWidth() {
    return NucleusAbstract::getSettings()->getWindowSize().x;
  }
  inline int getWindowHeight() {
    return NucleusAbstract::getSettings()->getWindowSize().y;
  }
  
  // ----------------------------------------------------------------------------
  // User code can ask if to expect IO events at all, based on that you can make decisions
  // On devbench you can pass false in this function to simulate no I/O situation
  // On fbdev input value will be ignored and actual situation is always returned

  inline bool eventHasKeyboard(bool fakeReturnValue = true) {
    return NucleusAbstract::getNucleus()->eventHasKeyboard(fakeReturnValue);
  }
  inline bool eventHasMouse(bool fakeReturnValue = true) {
    return NucleusAbstract::getNucleus()->eventHasMouse(fakeReturnValue);
  }
  
  // ----------------------------------------------------------------------------
  // Use this for asset loading, simply order constructAssetPathString("shaders/uubershader.frag")
  // and you will get back string with absolute path to that file within assets directory

  inline std::string constructAssetPathString(const std::string& path) {
    return NucleusAbstract::getNucleus()->constructAssetPathString(path);
  }
  inline std::string getBinaryExecPathString() {
    return NucleusAbstract::getNucleus()->getBinaryExecPathString();
  }
  inline std::string getBinaryDirectoryPathString() {
    return NucleusAbstract::getNucleus()->getBinaryDirectoryPathString();
  }
  inline std::string getAssetDirectoryPathString() {
    return NucleusAbstract::getNucleus()->getAssetDirectoryPathString();
  }
  inline void setAssetDirectoryPathString(std::string path) {
    NucleusAbstract::getNucleus()->setAssetDirectoryPathString(path);
  }
  
  // ----------------------------------------------------------------------------
  // Handy for devbenches

  inline void setCaptureCursor(bool capture = true) {
    NucleusAbstract::getNucleus()->setCaptureCursor(capture);
  }
  inline bool isCaptureCursor() {
    return NucleusAbstract::getNucleus()->isCaptureCursor();
  }
  inline void setFullScreen(bool fullScreen = true) {
    NucleusAbstract::getNucleus()->setFullScreen(fullScreen);
  }
  inline bool isFullScreen() {
    return NucleusAbstract::getNucleus()->isFullScreen();
  }
  
} }


#endif // NucleusAbstract_hpp
