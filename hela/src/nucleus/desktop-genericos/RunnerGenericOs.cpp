#include "nucleus/desktop-genericos/RunnerGenericOs.hpp"
#include "nucleus/desktop-genericos/NucleusGenericOs.hpp"
#include "nucleus/desktop-genericos/EventCodesMapGenericOs.hpp"

namespace hela { namespace nucleus {

  struct GLFWMouseAbsToDelta_t {
    hela::vec::vec2<double> prevAbs;
    hela::vec::vec2<double> currAbs;
    hela::vec::vec2<double> deltaAbs;
    bool itialized;
    GLFWMouseAbsToDelta_t():
    prevAbs(hela::vec::vec2<double>(0.0f, 0.0f)),
    currAbs(hela::vec::vec2<double>(0.0f, 0.0f)),
    deltaAbs(hela::vec::vec2<double>(0.0f, 0.0f)),
    itialized(false) { }
  };

  // ============================================================================
  class NucleusGlfwCallbacks {

  public:

    // ----------------------------------------------------------------------------
    static void registerNucleus(NucleusGenericOs* nucleus) {
      s_nucleus = nucleus;
      if (s_nucleus == nullptr) {
        //--breaker-throw
        throw std::runtime_error("Cannot register Nucleus");
      };
    }

    // ----------------------------------------------------------------------------
    static void registerRunner(RunnerGenericOs* runner) {
      s_runner = runner;
    }

    // ----------------------------------------------------------------------------
    static void glfwErrorClbk(int error, const char* description)
    {
      spdlog::error("GLFW internal error {}: {}", error, description);
    }

    // ----------------------------------------------------------------------------
    static void glfwWindowSizeClbk(motor_glfw::GLFWwindow* window, int width, int height)
    {
      // spdlog::trace("GLFW: window resized: {}x{}", width, height);
      // we don't use window size, we use framebuffer size, mkay
      // int left, top, right, bottom;
      // glfwGetWindowFrameSize(window, &left, &top, &right, &bottom);

      int nW, nH;
      motor_glfw::glfwGetFramebufferSize(window, &nW, &nH);
      getNucleus()->eventWindowResize(width, height);
    }

    // ----------------------------------------------------------------------------
    static void glfwWindowPosClbk(motor_glfw::GLFWwindow* /*window*/, int xpos, int ypos) {
      spdlog::trace("GLFW: window repositioned: {},{}", xpos, ypos);
    }

    // ----------------------------------------------------------------------------
    static void glfwWindowCloseClbk(motor_glfw::GLFWwindow* window) {
      spdlog::trace("GLFW: window close callback");
      bool doWeAllowClose = true;
      if (!doWeAllowClose) {
        motor_glfw::glfwSetWindowShouldClose(window, GLFW_FALSE);
      }
    }

    // ----------------------------------------------------------------------------
    static void glfwWindowIconifyClbk(motor_glfw::GLFWwindow* /*window*/, int iconified) {
      if (iconified) {
        spdlog::trace("GLFW: window minimized");
      } else {
        spdlog::trace("GLFW: window un-minimized (not maximized)");
      }
    }

    // ----------------------------------------------------------------------------
    static void glfwWindowFocusClbk(motor_glfw::GLFWwindow* /*window*/, int focused) {
      if (focused) {
        spdlog::trace("GLFW: window focused");
      } else {
        spdlog::trace("GLFW: window blurred");
      }
    }

    // ----------------------------------------------------------------------------
    static void glfwWindowRefreshClbk(motor_glfw::GLFWwindow* window)
    {
      spdlog::trace("GLFW: window damaged, needs to be refreshed");
       motor_glfw::glfwSwapBuffers(window);
    }

    // ----------------------------------------------------------------------------
    static void glfwFramebufferSizeClbk(motor_glfw::GLFWwindow* /*window*/, int width, int height)
    {
      // spdlog::trace("GLFW: framebuffer resized: {}x{}", width, height);
      getNucleus()->eventWindowResize(width, height);
    }

    // ----------------------------------------------------------------------------
    static void glfwKeyClbk(motor_glfw::GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/) {
      // spdlog::trace("GLFW: key event");

      getNucleus()->eventKeyboardKey(GLFW_KEY_TO_LINUX_MAP(key), GLFW_ACTION_TO_LINUX_MAP(action));
      // POLL
      // int state = glfwGetKey(window, GLFW_KEY_E);
      // if (state == GLFW_PRESS) { doStuff(); }
    }

    // ----------------------------------------------------------------------------
    static void glfwCursorPosClbk(motor_glfw::GLFWwindow* /*window*/, double xpos, double ypos) {
      // spdlog::trace("GLFW: cursor position event: {},{}", xpos, ypos);

      // GLFW gives absolute coordinates
      // However Linux kernel gives deltas and user code should use deltas (and do absolutes manually if wanted)

      if (!s_glfwAbsToDelta.itialized) {
        s_glfwAbsToDelta.currAbs = hela::vec::vec2<double>(xpos, ypos);
        s_glfwAbsToDelta.prevAbs = s_glfwAbsToDelta.currAbs;
        s_glfwAbsToDelta.itialized = true;
      }
      s_glfwAbsToDelta.currAbs = hela::vec::vec2<double>(xpos, ypos);
      s_glfwAbsToDelta.deltaAbs = s_glfwAbsToDelta.currAbs - s_glfwAbsToDelta.prevAbs;
      s_glfwAbsToDelta.prevAbs = s_glfwAbsToDelta.currAbs;

      getNucleus()->eventMousePositionDelta(static_cast<int>(std::ceil(s_glfwAbsToDelta.deltaAbs.x)), static_cast<int>(std::ceil(s_glfwAbsToDelta.deltaAbs.y)));

      // We still pass also absolute coordinates, because why not
      getNucleus()->eventMousePositionAbs(xpos, ypos);
    }


    // ----------------------------------------------------------------------------
    static void glfwCursorEnterClbk(motor_glfw::GLFWwindow* /*window*/, int entered)
    {
      if (entered) {
        spdlog::trace("GLFW: Cursor entered window");
      } else {
        spdlog::trace("GLFW: Cursor left window");
      }
    }

    // ----------------------------------------------------------------------------
    static void glfwMouseButtonClbk(motor_glfw::GLFWwindow* /*window*/, int button, int action, int /*mods*/)
    {
      getNucleus()->eventMouseButton(GLFW_MOUSE_BUTTON_TO_LINUX_MAP(button), GLFW_ACTION_TO_LINUX_MAP(action));
    }

    // ----------------------------------------------------------------------------
    static void glfwScrollClbk(motor_glfw::GLFWwindow* /*window*/, double xoffset, double yoffset)
    {
      // spdlog::trace("GLFW: mouse scroll event {},{}", xoffset, yoffset);
      // it seems that GLFW minimum delta is ~0.1
      getNucleus()->eventMouseWheel(static_cast<int>(xoffset*10.0f), static_cast<int>(yoffset*10.0f));
    }

    // ----------------------------------------------------------------------------
    static void glfwDropClbk(motor_glfw::GLFWwindow* /*window*/, int count, const char** paths)
    {
      //spdlog::trace("GLFW: dropped event: drop count: {}", count);
      //for (int i = 0;  i < count;  ++i) {
      //  spdlog::trace("[{}]: {}", i, paths[i]);
      //}
      getNucleus()->eventWindowDrop(count, paths);
    }

    // ----------------------------------------------------------------------------
    static void glfwMonitorClbk(motor_glfw::GLFWmonitor* /*monitor*/, int event)
    {
      if (event == GLFW_CONNECTED)
      {
        spdlog::trace("GLFW: Monitor connected");
      }
      else if (event == GLFW_DISCONNECTED)
      {
        spdlog::trace("GLFW: Monitor disconnected");
      }
    }

    // ----------------------------------------------------------------------------

  protected:
    static NucleusGenericOs* getNucleus() {
      if (s_nucleus == nullptr) {
        //--breaker-throw
        throw std::runtime_error("GLFW sees nucleus as null");
      }
      return s_nucleus;
    }
    static RunnerGenericOs* getRunner() {
      if (s_runner == nullptr) {
        //--breaker-throw
        throw std::runtime_error("GLFW sees runner as null");
      }
      return s_runner;
    }

  private:
    static NucleusGenericOs* s_nucleus;
    static RunnerGenericOs* s_runner;
    static GLFWMouseAbsToDelta_t s_glfwAbsToDelta;

  };

  // ============================================================================

  RunnerGenericOs* NucleusGlfwCallbacks::s_runner = nullptr;
  NucleusGenericOs* NucleusGlfwCallbacks::s_nucleus = nullptr;
  GLFWMouseAbsToDelta_t NucleusGlfwCallbacks::s_glfwAbsToDelta = GLFWMouseAbsToDelta_t();

  // =============================m_nucleusSettings===============================================


  // ----------------------------------------------------------------------------
  RunnerGenericOs::RunnerGenericOs(NucleusGenericOs *aNucleus, const NucleusGenericOs::NucleusSettings &settings)
  :m_Nucleus(aNucleus), m_nucleusSettings(settings) {
    spdlog::trace("RunnerGenericOs construct");
    // Initialize GLFW

    spdlog::trace("RunnerGenericOs construct glfwInit");
    motor_glfw::glfwSetErrorCallback(NucleusGlfwCallbacks::glfwErrorClbk);
    if (motor_glfw::glfwInit() == GLFW_FALSE)
    {
      //--breaker-throw
      throw std::runtime_error("GLFW init failed. Aborting.");
    }

    NucleusGlfwCallbacks::registerNucleus(aNucleus);
    NucleusGlfwCallbacks::registerRunner(this);

  }

  // ----------------------------------------------------------------------------
  RunnerGenericOs::~RunnerGenericOs() {
    spdlog::trace("RunnerGenericOs destruct");
    motor_glfw::glfwTerminate();
  }


  // ----------------------------------------------------------------------------
  void RunnerGenericOs::run() {
    spdlog::trace("RunnerGenericOs run");

    // --------------------------------
    //--breaker-nocatch-begin

    hela::motor::Motor::MotorSettings useMotorOptions = m_Nucleus->getMotor()->getMotorSettings();

    // --------------------------------
    int glfwVersionMajor, glfwVersionMinor, glfwVersionRevision;
    motor_glfw::glfwGetVersion(&glfwVersionMajor, &glfwVersionMinor, &glfwVersionRevision);
    spdlog::debug("GLFW version: {}.{}.{}", glfwVersionMajor, glfwVersionMinor, glfwVersionRevision);

    // --------------------------------
    // Context related hints
    // This is Motor stuff

#if defined(HELA_MACOS)
    motor_glfw::glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    motor_glfw::glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
#else
    // posiblity to set GLFW_CLIENT_API to GLFW_OPENGL_API / GLFW_OPENGL_ES_API
    // posiblity to set GLFW_CONTEXT_CREATION_API to GLFW_NATIVE_CONTEXT_API / GLFW_EGL_CONTEXT_API
#endif


#if defined(HELA_MACOS)
    // At least 3.3 for OpenGL
    spdlog::warn("I detected macOS, the only OS from big-three which essentially does not support open standard, cross platform API for graphics programming and compute as OpenGL (and OpenCL) is depreciated and Vulkan is not introduced and Apple controls drivers. I pity the fool!");
    spdlog::warn("Setting OpenGL version to 3.3 (if possible it will autoupgrade to highest possible which is 4.1) and core profile, ignoring motor settings you set in your code.");
    motor_glfw::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    motor_glfw::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    motor_glfw::glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    motor_glfw::glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
    std::pair<uint8_t,uint8_t> useVersion = useMotorOptions.getVersion();
    spdlog::debug("Setting OpenGL version to {}.{} and core profile to {}. Remember, OpenGL context will be set forward-compatible.", useVersion.first, useVersion.second, useMotorOptions.isCoreProfile());
    motor_glfw::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, useVersion.first);
    motor_glfw::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, useVersion.second);
    motor_glfw::glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    motor_glfw::glfwWindowHint(GLFW_OPENGL_PROFILE, (useMotorOptions.isCoreProfile()) ? GLFW_OPENGL_CORE_PROFILE : GLFW_OPENGL_COMPAT_PROFILE);
#endif
    motor_glfw::glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
    motor_glfw::glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_NO_ROBUSTNESS);
    motor_glfw::glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, GLFW_ANY_RELEASE_BEHAVIOR);
    motor_glfw::glfwWindowHint(GLFW_CONTEXT_NO_ERROR, GLFW_FALSE);

    // --------------------------------
    // Window related hints
    // This is Nucleus stuff
    // Setting resizable makes no sense as any app on Mali FBdev will be launched and stays constant
    motor_glfw::glfwWindowHint(GLFW_RESIZABLE, m_nucleusSettings.isResizable());
    motor_glfw::glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    motor_glfw::glfwWindowHint(GLFW_DECORATED, !m_nucleusSettings.isBorderless());
    motor_glfw::glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    motor_glfw::glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_TRUE);
    motor_glfw::glfwWindowHint(GLFW_FLOATING, m_nucleusSettings.isAlwaysOnTop());
    motor_glfw::glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);

    // --------------------------------
    // Framebuffer related hints
    // This is Motor stuff
    // Set buffer bits
    // Normalize RGBA to be 8 on each channel on Allwinner and testbenches
    // TODO: however make setting depth bits from user code

    // color buffer channel bits

    motor_glfw::glfwWindowHint(GLFW_RED_BITS, useMotorOptions.getColorBufferChannelBits());
    motor_glfw::glfwWindowHint(GLFW_GREEN_BITS, useMotorOptions.getColorBufferChannelBits());
    motor_glfw::glfwWindowHint(GLFW_BLUE_BITS, useMotorOptions.getColorBufferChannelBits());
    motor_glfw::glfwWindowHint(GLFW_ALPHA_BITS, useMotorOptions.getColorBufferChannelBits());

    // depth and stencil buffer bits
    motor_glfw::glfwWindowHint(GLFW_DEPTH_BITS, useMotorOptions.getDepthBufferBits());
    motor_glfw::glfwWindowHint(GLFW_STENCIL_BITS, useMotorOptions.getStencilBufferBits());

    // Depth and stecils will be on in GLFW anyways

    // MSAA
    motor_glfw::glfwWindowHint(GLFW_SAMPLES, useMotorOptions.getMsaa());

    /// nono
    motor_glfw::glfwWindowHint(GLFW_STEREO, GLFW_FALSE);

    // set to true?
    motor_glfw::glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_FALSE);

    // Doooh!
    motor_glfw::glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

    // --------------------------------
    // Monitor related hints
    motor_glfw::glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE); // when fullscreen set to maximum?

    // --------------------------------
    // Monitors all
    int monitorsCount;
    motor_glfw::glfwGetMonitors(&monitorsCount);
    // GLFWmonitor** monitorsAll = motor_glfw::glfwGetMonitors(&monitorsCount);
    spdlog::trace("Found {} monitors, will use primary", monitorsCount);

    // --------------------------------
    // Monitor primary

    motor_glfw::GLFWmonitor* glfwMonitorPrimary = motor_glfw::glfwGetPrimaryMonitor();
    const motor_glfw::GLFWvidmode* glfwModeMonitorPrimary = motor_glfw::glfwGetVideoMode(glfwMonitorPrimary);
    hela::vec::vec2<int> monitorPrimarySize(glfwModeMonitorPrimary->width, glfwModeMonitorPrimary->height);
    spdlog::trace("Primary monitor size: {}x{}", monitorPrimarySize.x, monitorPrimarySize.y);

    // Remember that GLFWwindow object is actually BOTH window and a context

    // glfwSetWindowMonitor to toggle
    // const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    // glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);

    bool doFullscreen = m_nucleusSettings.isFullScreen();
    hela::vec::vec2<int> doSize(static_cast<int>(m_nucleusSettings.getWindowSize().x), static_cast<int>(m_nucleusSettings.getWindowSize().y));

    spdlog::trace("You asked for window size: {}x{}", doSize.x, doSize.y);

    // calculate fit size always anyways
    // store fit size for windowed mode setting when fullscreen asked on entry an user later switches to windowed
    // assume max 90% fit fow windowed mode
    constexpr float fitPct = 0.9f;
    hela::vec::vec2<int> fitSize = doSize;
    float fitScaler = std::min(
                            fitPct * static_cast<float>(monitorPrimarySize.x)/static_cast<float>(fitSize.x),
                            fitPct * static_cast<float>(monitorPrimarySize.y)/static_cast<float>(fitSize.y)
                          );

    if (fitScaler < 1.0f) {
      fitScaler = std::max(0.1f, std::min(fitScaler, 1.0f));
      fitSize.x = static_cast<int>(fitScaler * static_cast<float>(fitSize.x));
      fitSize.y = static_cast<int>(fitScaler * static_cast<float>(fitSize.y));
    }

    if (m_nucleusSettings.isWindowFitMonitor()) {
      if (fitScaler < 1.0f) {
        spdlog::trace("You asked for windowed mode. However as primary monitor is smaller than asked window size I am going to set {}% fit resulting in window size: {}x{}", static_cast<int>(fitPct*100.0f), doSize.x, doSize.y);
      }
      doSize = hela::vec::vec2<int>(fitSize.x, fitSize.y);
    }

    if (doFullscreen) {
      // before doing that save "last windowed mode size"
      m_lastWindowedSize = hela::vec::vec2<int>(doSize.x, doSize.y);

      motor_glfw::glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
      doSize.x = monitorPrimarySize.x;
      doSize.y = monitorPrimarySize.y;
      spdlog::trace("However as fulscreen was asked I am going to set size same as that of the primary monitor: {}x{}", doSize.x, doSize.y);
    }

    m_glfwWindowObj = motor_glfw::glfwCreateWindow(
                                         doSize.x,
                                         doSize.y,
                                         m_nucleusSettings.getTitle().c_str(),
                                         (doFullscreen) ? glfwMonitorPrimary : nullptr,
                                         nullptr);

    if (m_glfwWindowObj == nullptr) {
      //--breaker-misc
      ghostBuster_();
      //--breaker-throw
      throw std::runtime_error("Failed to create GLFW window. Aborting.");
    }

    // glfwSetWindowSize(m_glfwWindowObj, 600, 600);

    // --------------------------------
    // Frambeuffer size

    // Get framebuffer actual size after window creation, because window size does not mean framebuffer size!
    GLsizei strapWindowWidthActual, strapWindowHeightActual;
    motor_glfw::glfwGetFramebufferSize(m_glfwWindowObj, &strapWindowWidthActual, &strapWindowHeightActual);
    // TODO: inform about that our framebuffer function

    // --------------------------------
    // Window size limits
    // enforce minimum limits, but don't enforce maximum
    motor_glfw::glfwSetWindowSizeLimits(m_glfwWindowObj, 64, 64, GLFW_DONT_CARE, GLFW_DONT_CARE);

    // don't enforce aspect ratio, as Allwinner can be connected to any ratio and you want to be able to write for that on desk bench
    // enabling this will throw once resized on macOS, so don't touch it
    // motor_glfw::glfwSetWindowAspectRatio(m_glfwWindowObj, GLFW_DONT_CARE, GLFW_DONT_CARE);

    // --------------------------------
    // Window position
    // motor_glfw::glfwSetWindowPos(m_glfwWindowObj, 100, 100);
    // motor_glfw::glfwGetWindowPos(m_glfwWindowObj, &xpos, &ypos);

    // --------------------------------
    // Window title
    // set on windows creation
    // motor_glfw::glfwSetWindowTitle(m_glfwWindowObj, "Hela"); // u8"Emojis!"

    // --------------------------------
    // Window icon
    // don't care

    // --------------------------------
    // Monitor for window

    // GLFWmonitor* glfwMonitorWindow = glfwGetWindowMonitor(m_glfwWindowObj);
    // const GLFWvidmode* glfwModeMonitorWindow = glfwGetVideoMode(glfwMonitorWindow);
    // enter fullscreen
    // int xPosSaver, yPosSaver;
    // glfwGetWindowPos(window, &xPosSaver, &yPosSaver);
    // int widthSaver, heightSaver;
    // glfwGetWindowSize(window, &width, &height);
    // glfwSetWindowMonitor(m_glfwWindowObj, glfwMonitorWindow, 0, 0, glfwModeMonitorWindow->width, glfwModeMonitorWindow->height, glfwModeMonitorWindow->refreshRate);
    // switch back
    // glfwSetWindowMonitor(m_glfwWindowObj, NULL, xPosSaver, yPosSaver, widthSaver, heightSaver, 0);

    // --------------------------------
    // Setup callbacks for window/context

    motor_glfw::glfwSetWindowSizeCallback(m_glfwWindowObj, NucleusGlfwCallbacks::glfwWindowSizeClbk);
    motor_glfw::glfwSetWindowPosCallback(m_glfwWindowObj, NucleusGlfwCallbacks::glfwWindowPosClbk);
    motor_glfw::glfwSetWindowCloseCallback(m_glfwWindowObj, NucleusGlfwCallbacks::glfwWindowCloseClbk);
    motor_glfw::glfwSetWindowIconifyCallback(m_glfwWindowObj, NucleusGlfwCallbacks::glfwWindowIconifyClbk);
    motor_glfw::glfwSetWindowFocusCallback(m_glfwWindowObj, NucleusGlfwCallbacks::glfwWindowFocusClbk);
    motor_glfw::glfwSetWindowRefreshCallback(m_glfwWindowObj, NucleusGlfwCallbacks::glfwWindowRefreshClbk);
    motor_glfw::glfwSetFramebufferSizeCallback(m_glfwWindowObj, NucleusGlfwCallbacks::glfwFramebufferSizeClbk);

    // --------------------------------
    // Setup callbacks for I/O
    if (m_nucleusSettings.isUserInputAllowed()) {
      motor_glfw::glfwSetInputMode(m_glfwWindowObj, GLFW_STICKY_KEYS, GLFW_TRUE); // enable in case we use polling via glfwGetKey()
      motor_glfw::glfwSetKeyCallback(m_glfwWindowObj, NucleusGlfwCallbacks::glfwKeyClbk);
      motor_glfw::glfwSetInputMode(m_glfwWindowObj, GLFW_CURSOR, (m_nucleusSettings.isCaptureCursor()) ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
      motor_glfw::glfwSetCursorPosCallback(m_glfwWindowObj, NucleusGlfwCallbacks::glfwCursorPosClbk);
      motor_glfw::glfwSetCursorEnterCallback(m_glfwWindowObj, NucleusGlfwCallbacks::glfwCursorEnterClbk);
      motor_glfw::glfwSetInputMode(m_glfwWindowObj, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
      motor_glfw::glfwSetMouseButtonCallback(m_glfwWindowObj, NucleusGlfwCallbacks::glfwMouseButtonClbk);
      motor_glfw::glfwSetScrollCallback(m_glfwWindowObj, NucleusGlfwCallbacks::glfwScrollClbk);
      // don't use
      // glfwSetCharCallback(m_glfwWindowObj, nullptr);
      // glfwSetCharModsCallback(m_glfwWindowObj, nullptr);
    }

    // glfwSetJoystickCallback(glfwJoystickClbk);

    motor_glfw::glfwSetDropCallback(m_glfwWindowObj, NucleusGlfwCallbacks::glfwDropClbk);

    // --------------------------------
    // Setup callbacks for monitors
    motor_glfw::glfwSetMonitorCallback(NucleusGlfwCallbacks::glfwMonitorClbk);


    // Make the window's context current
    motor_glfw::glfwMakeContextCurrent(m_glfwWindowObj);

    if (!::gladLoadGL()) {
      //--breaker-misc
      ghostBuster_();
      //--breaker-throw
      throw std::runtime_error("GLAD load failed. Aborting.");
    }

    // --------------------------------
    // Print some info

    spdlog::debug("--------------------------------");
    spdlog::debug("GL INFO");
    spdlog::debug("GL Version: {}", (char *)glGetString(GL_VERSION));
    spdlog::debug("GL GLSL version: {}", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
    spdlog::debug("GL Renderer: {}", (char *)glGetString(GL_RENDERER));
    spdlog::debug("GL Vendor: {}", (char *)glGetString(GL_VENDOR));
    // don't print extensions on devbenches, does not matter
    //spdlog::debug("GL Extensions:");
    //GLint numExt, eIdx;
    //glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);
    //for (eIdx = 0; eIdx < numExt; ++eIdx) {
    // spdlog::debug("[{:03d}] {}", eIdx, glGetStringi(GL_EXTENSIONS, eIdx));
    //}

    // don't print extra info on devbenches, does not matter
    //GLint glExtraData;
    //glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &glExtraData);
    //spdlog::debug("GL_MAX_VERTEX_ATTRIBS Max no. of vertex attributes supported: {}", glExtraData);
    //glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &glExtraData);
    //spdlog::debug("GL_MAX_TEXTURE_IMAGE_UNITS Max no. of sim tex units supported: {}", glExtraData);

    // --------------------------------
    // Set swap interval
    motor_glfw::glfwSwapInterval(useMotorOptions.isVsync() ? 1 : 0);

    // --------------------------------
    // Window visibility
    // We could create window with hint glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    // and show it here
    if (!motor_glfw::glfwGetWindowAttrib(m_glfwWindowObj, GLFW_VISIBLE)) {
      motor_glfw::glfwShowWindow(m_glfwWindowObj);
    }

    // --------------------------------
    if (!doFullscreen) {
      motor_glfw::glfwGetWindowPos(m_glfwWindowObj, &m_lastWindowedPosition.x, &m_lastWindowedPosition.y);
    } else {
      m_lastWindowedPosition.x = static_cast<int>(static_cast<float>(monitorPrimarySize.x - fitSize.x) * 0.5f);
      m_lastWindowedPosition.y = static_cast<int>(static_cast<float>(monitorPrimarySize.y - fitSize.y) * 0.5f);
    }

    // don't use GLFW timing functions, use our own
    //motor_glfw::glfwSetTime(motor_glfw::glfwGetTime());
    // double gltfTime = motor_glfw::glfwGetTime();

    //--breaker-nocatch-end
    // --------------------------------

    // --------------------------------
    try {

      // Clean out any OpenGL errors, because I said so
      GLenum err;
      while ((err = glGetError()) != GL_NO_ERROR) {
        spdlog::error("Cleaning out OpenGL errors: {0:d} (0x{0:x})", err);
      }

      // User setup routine
      m_Nucleus->abstractSetup_();

      for (;;) {
        motor_glfw::glfwMakeContextCurrent(m_glfwWindowObj);

        if (m_flagShouldQuit || motor_glfw::glfwWindowShouldClose(m_glfwWindowObj)) {
          break;
        }

        // Poll for and process events
        motor_glfw::glfwPollEvents();

        // User update routine
        m_Nucleus->abstractUpdate_();

        // User draw routine
        m_Nucleus->abstractDraw_();

        // Swap buffers
        motor_glfw::glfwSwapBuffers(m_glfwWindowObj);

      }

      // User cleanup routine
      m_Nucleus->abstractCleanup_();

    }
    // don't rethrow here, log and pass to ghostBuster_ for orderly GLFW cleanup
    catch (std::exception &e){
      //--breaker-catch-forward
      // not entirely true as we have also GLFW in scope
      spdlog::error("User code threw and it was was not caught earlier (setup() should rethrow intentionally on shader, texture etc. errors).");
      spdlog::error("User code exception: {}", e.what());
    }
    catch (...) {
      //--breaker-catch-forward
      spdlog::error("User code threw.");
    }

    // --------------------------------
    ghostBuster_();
    
  }

  // ----------------------------------------------------------------------------
  void RunnerGenericOs::ghostBuster_() {
    // --------------------------------
    //--breaker-nocatch-begin

    // Destroy
    if (m_glfwWindowObj != nullptr) {
      spdlog::trace("GLFW about to exit, has window object, cleaning up.");

      // Unregister callbacks
      motor_glfw::glfwSetInputMode(m_glfwWindowObj, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      motor_glfw::glfwSetWindowSizeCallback(m_glfwWindowObj, nullptr);
      motor_glfw::glfwSetWindowPosCallback(m_glfwWindowObj, nullptr);
      motor_glfw::glfwSetWindowCloseCallback(m_glfwWindowObj, nullptr);
      motor_glfw::glfwSetWindowIconifyCallback(m_glfwWindowObj, nullptr);
      motor_glfw::glfwSetWindowFocusCallback(m_glfwWindowObj, nullptr);
      motor_glfw::glfwSetWindowRefreshCallback(m_glfwWindowObj, nullptr);
      motor_glfw::glfwSetFramebufferSizeCallback(m_glfwWindowObj, nullptr);
      motor_glfw::glfwSetKeyCallback(m_glfwWindowObj, nullptr);
      motor_glfw::glfwSetCursorPosCallback(m_glfwWindowObj, nullptr);
      motor_glfw::glfwSetCursorEnterCallback(m_glfwWindowObj, nullptr);
      motor_glfw::glfwSetMouseButtonCallback(m_glfwWindowObj, nullptr);
      motor_glfw::glfwSetScrollCallback(m_glfwWindowObj, nullptr);
      motor_glfw::glfwSetDropCallback(m_glfwWindowObj, nullptr);
      motor_glfw::glfwSetMonitorCallback(nullptr);

      // Destroy window
      motor_glfw::glfwDestroyWindow(m_glfwWindowObj);
    }

    // Terminate
    spdlog::trace("GLFW terminating");
    motor_glfw::glfwTerminate();

    //--breaker-nocatch-end
    // --------------------------------
  }

  // ----------------------------------------------------------------------------
  void RunnerGenericOs::quit() {
    spdlog::trace("RunnerGenericOs quit asked");
    // set both flags, that are available, to be double sure ^_^
    motor_glfw::glfwSetWindowShouldClose(m_glfwWindowObj, GLFW_TRUE);
    m_flagShouldQuit = true;
  }

  // ----------------------------------------------------------------------------
  void RunnerGenericOs::setCaptureCursor(bool capture) {
    if (capture) {
      motor_glfw::glfwSetInputMode(m_glfwWindowObj, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
      motor_glfw::glfwSetInputMode(m_glfwWindowObj, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
  }

  // ----------------------------------------------------------------------------
  bool RunnerGenericOs::isCaptureCursor() const {
    return (motor_glfw::glfwGetInputMode(m_glfwWindowObj, GLFW_CURSOR) == GLFW_CURSOR_DISABLED);
  }

  // ----------------------------------------------------------------------------
  void RunnerGenericOs::setFullScreen(bool fullScreen) {
    //
    spdlog::trace("RunnerGenericOs setFullScreen asked");

    if ((fullScreen && isFullScreen()) || (!fullScreen && !isFullScreen())) {
      return;
    }

    motor_glfw::GLFWmonitor* glfwMonitorToUse;
    glfwMonitorToUse = motor_glfw::glfwGetWindowMonitor(m_glfwWindowObj);
    if (glfwMonitorToUse == nullptr) {
      glfwMonitorToUse = motor_glfw::glfwGetPrimaryMonitor();
      if (glfwMonitorToUse == nullptr) return;
    }
    const motor_glfw::GLFWvidmode* glfwModeMonitorToUse = motor_glfw::glfwGetVideoMode(glfwMonitorToUse);
    if (fullScreen) {

      motor_glfw::glfwGetWindowSize(m_glfwWindowObj, &m_lastWindowedSize.x, &m_lastWindowedSize.y);
      motor_glfw::glfwGetWindowPos(m_glfwWindowObj, &m_lastWindowedPosition.x, &m_lastWindowedPosition.y);
      motor_glfw::glfwSetWindowMonitor(
                           m_glfwWindowObj,
                           glfwMonitorToUse,
                           0, 0,
                           glfwModeMonitorToUse->width,
                           glfwModeMonitorToUse->height,
                           glfwModeMonitorToUse->refreshRate);
    } else {
      motor_glfw::glfwSetWindowMonitor(
                           m_glfwWindowObj,
                           nullptr,
                           m_lastWindowedPosition.x,
                           m_lastWindowedPosition.y,
                           m_lastWindowedSize.x,
                           m_lastWindowedSize.y,
                           glfwModeMonitorToUse->refreshRate);
      motor_glfw::glfwSetWindowTitle(m_glfwWindowObj, m_nucleusSettings.getTitle().c_str());
    }

      // int left, top, right, bottom;
      // glfwGetWindowFrameSize(window, &left, &top, &right, &bottom);
  }

  // ----------------------------------------------------------------------------
  bool RunnerGenericOs::isFullScreen() const {
    spdlog::trace("RunnerGenericOs isFullScreen asked");

    motor_glfw::GLFWmonitor* glfwMonitorToChekck;
    glfwMonitorToChekck = motor_glfw::glfwGetWindowMonitor(m_glfwWindowObj);
    if (glfwMonitorToChekck == nullptr) {
      glfwMonitorToChekck = motor_glfw::glfwGetPrimaryMonitor();
      if (glfwMonitorToChekck == nullptr) return false;
    }
    const motor_glfw::GLFWvidmode* glfwModeMonitorToCheck = motor_glfw::glfwGetVideoMode(glfwMonitorToChekck);

    int windowWidthCurr, windowHeightCurr;
    motor_glfw::glfwGetWindowSize(m_glfwWindowObj, &windowWidthCurr, &windowHeightCurr);
    if (windowWidthCurr == glfwModeMonitorToCheck->width && windowHeightCurr == glfwModeMonitorToCheck->height) {
      return true;
    }
    return false;
  }

} }
