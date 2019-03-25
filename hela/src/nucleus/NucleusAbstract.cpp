#include "nucleus/NucleusAbstract.hpp"

namespace hela { namespace nucleus {

  NucleusAbstract* NucleusAbstract::s_nucleusInstance = nullptr;
  NucleusAbstract::NucleusSettings* NucleusAbstract::s_nucleusSettingsFromMain = nullptr;

  // fallback when compiler does not corporate
  // void signalMainForwarderNoinfo(int signum) {
  //   NucleusAbstract::getNucleus()->signalMainHandlerMemberNoinfo(signum);
  // }

  // ============================================================================
  // NucleusSettings

  // SoC is low memory, tie down vtable
  NucleusAbstract::NucleusSettings::~NucleusSettings() {}

  // init
  void NucleusAbstract::NucleusSettings::init(int argc, char const * const argv[], const hela::motor::Motor_S& motor) {
    spdlog::trace("NucleusAbstract::NucleusSettings init");

    // Just push all raw command line arguments
    for (int32_t arg = 0; arg < argc; ++arg) {
      m_nucleusSettingsCommandLineArgsRawSplit.push_back(argv[arg]);
    }
    // This is where we look for --width and --height as specified by consumer
    // That takes precedence over preferred size set by coder

    // A better approach would be using some command line arg parser lib
    // getopt is possibility, but I really want this to work eventually on MSW devbench
    // Boost.ProgramOptions is a nogo, Hela does not want to be fat
    // There are some lightweight herader only command line parsers out there
    // Whatever, let us do naive manual stuff using just populated string vector
    // Implement "long version" that is "--parameter value"

    // This assumes if --width || --height is specified multiple times use first occurance
    bool isWidthAndheightSpecified = true;
    hela::vec::vec2<uint16_t> consumerSize(128, 128);
    auto findConsumerSize = [](const std::vector<std::string>& args, const std::string term, bool& found, uint16_t& size)->void {
      auto sIt = std::find(args.begin(), args.end(), term);
      if (sIt != args.end() && std::next(sIt) != args.end()) {
        size = static_cast<uint16_t>(std::stoi(*(std::next(sIt))));
      } else {
        found = false;
      }
    };

    findConsumerSize(m_nucleusSettingsCommandLineArgsRawSplit, "--width", isWidthAndheightSpecified, consumerSize.x);
    findConsumerSize(m_nucleusSettingsCommandLineArgsRawSplit, "--height", isWidthAndheightSpecified, consumerSize.y);
    if (!isWidthAndheightSpecified) {
      #if defined(HELA_LINUX_ARM_FBDEV_MALI_ALLWINNER)
        spdlog::warn("Nucleus says you have not set --width and/or --height. Will use values set by coder.");
        spdlog::warn("Note that it may lead to EGL_BAD_ALLOC if coder has set default values that do not fit in your output resolution.");
      #endif
    } else {
      spdlog::debug("Nucleus got command line arguments for width and height. Will construct window of size {}x{}", consumerSize.x, consumerSize.y);
      setWindowSize(consumerSize);
    }

    // TODO: maybe implement std::pair container that holds structured command line stuff

    m_nucleusSettingsMotor = motor;

  }

  // ============================================================================
  // NucleusAbstract

  // ----------------------------------------------------------------------------
  void NucleusAbstract::prepareLaunch() {
    // called from main 1/6
    spdlog::trace("NucleusAbstract prepareLaunch");
    // To set target specific overrides implement static void initialize(..) in Nucleus<Something>
    // Currently there are no overrides for Allwinner or Generic
    // set up system for retreiving path to assets directory
  }

  // ----------------------------------------------------------------------------
  void NucleusAbstract::initialize(int argc, char const * const argv[], NucleusSettings *settings, hela::motor::Motor_S motor) {
    // Called from main 2/6
    spdlog::trace("NucleusAbstract initialize");
    // Initialize settings for Nucleus
    settings->init(argc, argv, motor);

    // Assign them to static truth location after this point
    s_nucleusSettingsFromMain = settings;

  }


  NucleusAbstract::NucleusAbstract() {
    // Called from main 3/6
    spdlog::trace("NucleusAbstract construct");
    s_nucleusInstance = this;
    m_nucleusCommandLineArgsRawSplit = s_nucleusSettingsFromMain->getCommandLineArgsRawSplit();
    m_nucleusMotor = s_nucleusSettingsFromMain->getMotor();

    try {
      // set binary path
      // will not use filesystem tools to check if first entry is valid
      // if somebody has messed with command line holder later when trying to load data from assets...
      m_nucleusBinaryExecPath = m_nucleusCommandLineArgsRawSplit.at(0);
      spdlog::trace("NucleusAbstract Current binary executable path is {}", m_nucleusBinaryExecPath);

      m_nucleusBinaryDirecoryPath = [](const std::string& path)->std::string {
        std::size_t found = path.find_last_of("/\\"); return (path.substr(0, found)+"/");
      }(m_nucleusBinaryExecPath);

      spdlog::trace("NucleusAbstract Current binary directory path is {}", m_nucleusBinaryDirecoryPath);

      // set default where assets directory is just besides binary
      m_AssetDirectoryPath = m_nucleusBinaryDirecoryPath + "assets/";
#if defined(HELA_MACOS)
      // In case of macos we copy assets directory in bundle. we will not start using Obj-C++ & NSBundle for this, hela-no
      m_AssetDirectoryPath = m_nucleusBinaryDirecoryPath + "../Resources/assets/";
#endif
      spdlog::trace("NucleusAbstract Current assets path is {}", m_AssetDirectoryPath);
    }
    catch (std::exception &e) {
      throw std::runtime_error("Cannot create paths to binary and assets as somebody has messed up command line arguments holder: " + std::string(e.what()));
    }
    catch (...) {
      throw std::runtime_error("Cannot create paths to binary and assets as somebody has messed up command line arguments holder");
    }

  }

  // ----------------------------------------------------------------------------
  void NucleusAbstract::executeLaunch() {
    // Called from main 4/6

    // register signals
#if defined(HELA_POSIX)
    spdlog::trace("NucleusAbstract SIGNAL register main");

    // --------------------------------
    // create signals
    struct sigaction act; memset(&act, 0, sizeof(act));
    // use sa_handler notion, clear SA_SIGINFO
    act.sa_flags = SA_RESTART;
    // sa_handler, switch to signalMainForwarderNoinfo if we get pointer incompatibilities
    act.sa_handler = &NucleusAbstract::signalMainHandlerStaticNoinfo;
    // empty mask
    sigemptyset(&act.sa_mask);
    // fill actions
    sigaction(SIGCHLD, &act, 0);
    sigaction(SIGTERM, &act, 0);
    sigaction(SIGINT, &act, 0);
    sigaction(SIGQUIT, &act, 0);
    sigaction(SIGHUP, &act, 0);
    sigaction(SIGABRT, &act, 0);

#endif

    spdlog::trace("NucleusAbstract executeLaunch");
    try {
      launch();
    }
    catch (std::exception &e) {
      throw std::runtime_error("executeLaunch got exception from launch: " + std::string(e.what()));
    }
    catch (...) {
      throw std::runtime_error("executeLaunch got exception from launch");
    }
  }

  // ----------------------------------------------------------------------------
  NucleusAbstract::~NucleusAbstract() {
    // Called from main 5/6
    spdlog::trace("NucleusAbstract destruct");
  }

  // ----------------------------------------------------------------------------
  void NucleusAbstract::cleanupLaunch() {
    // Called from main 6/6
    spdlog::trace("NucleusAbstract cleanupLaunch");
    // If you using prepareLaunch did something outside Nucleus be sure to clean up after yourself
  }

  // ----------------------------------------------------------------------------
  void NucleusAbstract::signalMainHandlerStaticNoinfo(int signum) {
#if defined(HELA_POSIX)
    // do not block signals that are not in signal mask

    char* signalChars = strsignal(signum);
    if (signalChars){
      spdlog::trace("NucleusAbstract::signalMainHandlerStaticNoinfo got signal with name: {}", signalChars);
    } else {
      spdlog::trace("NucleusAbstract::signalMainHandlerStaticNoinfo got signal interpeted as quit");
    }

    // Unbind
    signal(SIGINT, nullptr);
    signal(SIGTERM, nullptr);
    signal(SIGQUIT, nullptr);
    signal(SIGABRT, nullptr);
    signal(SIGHUP, nullptr);

    // Order Nucleus to stop runner, clean up and shut down all parts gracefully
    spdlog::trace("NucleusAbstract::signalMainHandlerStaticNoinfo caught the signal and requesting graceful shutdown");
    NucleusAbstract::getNucleus()->quit();
#endif
  }

  // ----------------------------------------------------------------------------
  // Put here setup routine that is independent of whether Allwinner or devbench
  void NucleusAbstract::abstractSetup_() {
    // set up frame count to 0
    // done in constructor

    // set time start marker
    m_abstractTimeStartMarkerEpochMs = abstractTimeGetEpochMs();

    // set up application
    setup();
  }

  // ----------------------------------------------------------------------------
  // Put here update routine that is independent of whether Allwinner or devbench
  void NucleusAbstract::abstractUpdate_() {

    // update frame count
    ++m_frameCount;

    // update elapsed time
    // nothing to do here, time is retreived in user code upon demand

    // call user code
    update();

    // set perf stats
    if (s_nucleusSettingsFromMain->isPerfStats()) {
      if ( m_perfStats.ignoredFramesCurr < m_perfStats.ignoredFramesStartLimit) {
        ++m_perfStats.ignoredFramesCurr;
      } else {
        m_perfStats.currMs = abstractTimeGetEpochMs();
        if (m_perfStats.startMs == 0) {
          m_perfStats.startMs = m_perfStats.currMs;
          m_perfStats.prevMs = m_perfStats.currMs;
        } else {
          ++m_perfStats.count;
          m_perfStats.deltaCurrMs = m_perfStats.currMs - m_perfStats.prevMs;
          if (m_perfStats.deltaCurrMs < m_perfStats.minMs) {
            m_perfStats.minMs = m_perfStats.deltaCurrMs;
          }
          if (m_perfStats.deltaCurrMs > m_perfStats.maxMs) {
            m_perfStats.maxMs = m_perfStats.deltaCurrMs;
          }
          m_perfStats.prevMs = m_perfStats.currMs;
        }
      }
    }
  }

  // ----------------------------------------------------------------------------
  // Put here update routine that is independent of whether Allwinner or devbench
  void NucleusAbstract::abstractDraw_() {
    draw();
  }

  // ----------------------------------------------------------------------------
  void NucleusAbstract::abstractCleanup_() {
    m_perfStats.endMs = m_perfStats.currMs;

    cleanup();

    if (s_nucleusSettingsFromMain->isPerfStats()) {
      int64_t perfRuntime = m_perfStats.endMs - m_perfStats.startMs;
      float perfMeanMs = static_cast<float>(perfRuntime) / static_cast<float>(m_perfStats.count);
      spdlog::info("--------------------------------");
      spdlog::info("PERFORMANCE DATA");
      spdlog::info("Maximum frame time [ms]: {0:d}", m_perfStats.maxMs);
      spdlog::info("Minimum frame time [ms]: {0:d}", m_perfStats.minMs);
      spdlog::info("Mean frame time [ms]: {0}", perfMeanMs);
      spdlog::info("Minimum [fps]: {0}", (1000.0f / m_perfStats.maxMs));
      spdlog::info("Maximums [fps]: {0}", (1000.0f / m_perfStats.minMs));
      spdlog::info("Mean [fps]: {0}", (1000.0f / perfMeanMs));
      spdlog::info("Runtime [sec]: {0}", (perfRuntime / 1000.0f));
      spdlog::info("Frames [n]: {0:d}", m_perfStats.count);
      spdlog::info("--------------------------------");
    }

  }

} }

// SoC is low memory, tie down vtable
namespace hela { namespace motor {
  Motor::~Motor() {
    spdlog::trace("Motor destruct");
  }
} }
