
#include "nucleus/embedded-linux-fbdev-mali-allwinner-mali-utgard/NucleusLinuxFbdevMaliAllwinner.hpp"
#include "nucleus/embedded-linux-fbdev-mali-allwinner-mali-utgard/RunnerLinuxFbdevMaliAllwinner.hpp"

#if (HELA_USE_GL_OES_vertex_array_object == 1) && defined(HELA_USE_GL_OES_vertex_array_object_POLYFILL) && (HELA_USE_GL_OES_vertex_array_object_POLYFILL == 1)
void glGenVertexArrays(GLsizei n, GLuint *arrays) {
  glGenVertexArraysOES(n, arrays);
}
void glBindVertexArray(GLuint array) {
  glBindVertexArrayOES(array);
}
void glDeleteVertexArrays(GLsizei n, const GLuint *arrays) {
  glDeleteVertexArraysOES(n, arrays);
}
GLboolean glIsVertexArray(GLuint array) {
  return glIsVertexArrayOES(array);
}
#endif

namespace hela { namespace nucleus {

  // ----------------------------------------------------------------------------
  RunnerLinuxFbdevMaliAllwinner::RunnerLinuxFbdevMaliAllwinner(NucleusLinuxFbdevMaliAllwinner *aNucleus, const NucleusLinuxFbdevMaliAllwinner::NucleusSettings &settings)
  :m_Nucleus(aNucleus), m_nucleusSettings(settings) {
    spdlog::trace("RunnerLinuxFbdevMaliAllwinner construct");

    m_runnerThreadId = std::this_thread::get_id();
  }

  // ----------------------------------------------------------------------------
  RunnerLinuxFbdevMaliAllwinner::~RunnerLinuxFbdevMaliAllwinner() {
    spdlog::trace("RunnerLinuxFbdevMaliAllwinner destruct");
  }

  // ----------------------------------------------------------------------------
  void RunnerLinuxFbdevMaliAllwinner::run() {
    spdlog::trace("RunnerLinuxFbdevMaliAllwinner run");

    // --------------------------------
    //--breaker-nocatch-begin

    // if m_Nucleus is nullptr, this will throw, so will not check in run when accessing m_Nucleus if it is walid any more
    hela::motor::Motor::MotorSettings useMotorOptions = m_Nucleus->getMotor()->getMotorSettings();

    // --------------------------------
    // SETUP

    m_eglDisplayObj = EGL_NO_DISPLAY;
    m_eglSurfaceObj = EGL_NO_SURFACE;
    m_eglContextObj = EGL_NO_CONTEXT;
    m_eglWindowNative = nullptr;

    // --------------------------------
    m_eglDisplayObj = motor_egl_aw::eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (m_eglDisplayObj == EGL_NO_DISPLAY) {
      spdlog::error("eglGetDisplay: No EGL Display {}", m_eglDisplayObj);
      //--breaker-misc
      ghostBuster_();
    }
    else {
      spdlog::trace("eglGetDisplay: Got EGL Display {}", m_eglDisplayObj);
    }

    // --------------------------------
    motor_egl_aw::EGLint eglInitializeResult;
    motor_egl_aw::EGLint eglVersionMajor, eglVersionMinor;
    eglInitializeResult = motor_egl_aw::eglInitialize(m_eglDisplayObj, &eglVersionMajor, &eglVersionMinor);
    if (eglInitializeResult == EGL_FALSE) {
      spdlog::error("eglInitialize: Failed to initialize GL: EGL_FALSE");
      //--breaker-misc
      ghostBuster_();
    }
    else if (eglInitializeResult == EGL_NOT_INITIALIZED) {
      spdlog::error("eglInitialize: Failed to initialize GL: EGL_NOT_INITIALIZED");
      //--breaker-misc
      ghostBuster_();
    }
    else if (eglInitializeResult == EGL_BAD_DISPLAY) {
      spdlog::error("eglInitialize: Failed to initialize GL: EGL_BAD_DISPLAY");
      //--breaker-misc
      ghostBuster_();
    }
    else {
      spdlog::trace("eglInitialize: EGL version: {}.{}", eglVersionMajor, eglVersionMinor);
    }

    // --------------------------------
    // API bind
    motor_egl_aw::EGLint eglBindAPIResult;
    eglBindAPIResult = motor_egl_aw::eglBindAPI(EGL_OPENGL_ES_API);
    if (eglBindAPIResult == EGL_FALSE) {
      spdlog::error("eglBindAPI failed. eglGetError: {0:d} (0x{0:x})", motor_egl_aw::eglGetError());
      //--breaker-misc
      ghostBuster_();
    }
    else {
      spdlog::trace("eglBindAPI: API bound");
    }

    // --------------------------------
    motor_egl_aw::EGLConfig eglConfigObj;
    motor_egl_aw::EGLint eglChooseConfigCount;
    motor_egl_aw::EGLint eglChooseConfigResult;
    motor_egl_aw::EGLint eglChooseConfigAttribList[] = {
      EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
      EGL_RED_SIZE, useMotorOptions.getColorBufferChannelBits(),
      EGL_GREEN_SIZE, useMotorOptions.getColorBufferChannelBits(),
      EGL_BLUE_SIZE, useMotorOptions.getColorBufferChannelBits(),
      EGL_ALPHA_SIZE, useMotorOptions.getColorBufferChannelBits(),
      // EGL_BUFFER_SIZE, useMotorOptions.getColorBufferChannelBits() * 4,
      EGL_SAMPLE_BUFFERS, 1,
      EGL_SAMPLES, useMotorOptions.getMsaa(),
      EGL_DEPTH_SIZE, useMotorOptions.isDepthBufferEnabled() ? useMotorOptions.getDepthBufferBits() : 0,
      EGL_STENCIL_SIZE, useMotorOptions.isStencilBufferEnabled() ? useMotorOptions.getStencilBufferBits() : 0,
      EGL_TRANSPARENT_TYPE, EGL_NONE, // awww, would be fun though :)

      EGL_BIND_TO_TEXTURE_RGB, EGL_DONT_CARE,
      EGL_BIND_TO_TEXTURE_RGBA, EGL_DONT_CARE,
      EGL_MAX_SWAP_INTERVAL, EGL_DONT_CARE,
      EGL_MIN_SWAP_INTERVAL, EGL_DONT_CARE,
      EGL_NATIVE_RENDERABLE, EGL_TRUE,
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT, // let us be very clear - ELG, thou shalt not use pbuffer
      EGL_CONFORMANT, EGL_OPENGL_ES2_BIT,
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
      EGL_NONE
    };

    eglChooseConfigResult = motor_egl_aw::eglChooseConfig(m_eglDisplayObj, eglChooseConfigAttribList, &eglConfigObj, 1, &eglChooseConfigCount);
    if (eglChooseConfigResult == EGL_FALSE){
      spdlog::error("eglChooseConfig: Failed to choose config. eglGetError: {0:d} (0x{0:x})", motor_egl_aw::eglGetError());
      //--breaker-misc
      ghostBuster_();
    }
    else if (eglChooseConfigCount < 1 || eglConfigObj == NULL) {
      spdlog::error("eglChooseConfig: no matching configs. config count reported:  {}", eglChooseConfigCount);
      //--breaker-misc
      ghostBuster_();
    }
    else {
      spdlog::trace("eglChooseConfig: EGL chose config, having count of {}", eglChooseConfigCount);
    }

    // --------------------------------
    // EGLNativeWindowType m_eglWindowNative;
    // fbdev_window windowFbdev = {static_cast<short unsigned>(screenSize.x), static_cast<short unsigned>(screenSize.y)};
    // m_eglWindowNative = &windowFbdev;
    // header says that struct fbdev_window holds unsigned short, Hela uses uint16_t, but implementation may differ, thus cast
    m_eglWindowNative = std::make_shared<motor_egl_aw::fbdev_window>();
    m_eglWindowNative->width = static_cast<unsigned short>(m_nucleusSettings.getWindowSize().x);
    m_eglWindowNative->height = static_cast<unsigned short>(m_nucleusSettings.getWindowSize().y);

    // --------------------------------
    motor_egl_aw::EGLint eglCreateWindowSurfaceAttribList[] = {
      EGL_RENDER_BUFFER, EGL_BACK_BUFFER, // double buffering!
      EGL_NONE};

    // EGLSurface m_eglSurfaceObj;
    m_eglSurfaceObj = motor_egl_aw::eglCreateWindowSurface(m_eglDisplayObj, eglConfigObj, m_eglWindowNative.get(), eglCreateWindowSurfaceAttribList);
    if (m_eglSurfaceObj == EGL_NO_SURFACE) {
      spdlog::error("eglCreateWindowSurface: Failed to create surface. eglGetError: {0:d} (0x{0:x})", motor_egl_aw::eglGetError());
      spdlog::error("!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?");
      spdlog::error("DID YOU SET --width AND --height LARGER THAN YOUR MONITOR RESOLUTION?");
      spdlog::error("!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?");
      //--breaker-misc
      ghostBuster_();
    }
    else {
      spdlog::trace("eglCreateWindowSurface: Created surface");
    }

    // --------------------------------
    motor_egl_aw::EGLBoolean eglSurfaceAttribResult;
    eglSurfaceAttribResult = motor_egl_aw::eglSurfaceAttrib(m_eglDisplayObj, m_eglSurfaceObj, EGL_MULTISAMPLE_RESOLVE, EGL_MULTISAMPLE_RESOLVE_DEFAULT);
    spdlog::trace("eglSurfaceAttrib: EGL_MULTISAMPLE_RESOLVE: {0:d} (0x{0:x})", eglSurfaceAttribResult);
    eglSurfaceAttribResult = motor_egl_aw::eglSurfaceAttrib(m_eglDisplayObj, m_eglSurfaceObj, EGL_SWAP_BEHAVIOR, EGL_BUFFER_DESTROYED);
    spdlog::trace("eglSurfaceAttrib: EGL_SWAP_BEHAVIOR: {0:d} (0x{0:x})", eglSurfaceAttribResult);

    // --------------------------------
    // EGLContext m_eglContextObj;
    motor_egl_aw::EGLint eglCreateContextAttribList[] = {
      EGL_CONTEXT_CLIENT_VERSION, 2, // use old alias insted of EGL_CONTEXT_MAJOR_VERSION, EGL_CONTEXT_MINOR_VERSION
      EGL_NONE};

    m_eglContextObj = motor_egl_aw::eglCreateContext(m_eglDisplayObj, eglConfigObj, EGL_NO_CONTEXT, eglCreateContextAttribList);
    if (m_eglContextObj == EGL_NO_CONTEXT) {
      spdlog::error("eglCreateContext: Failed to create context. eglGetError: {0:d} (0x{0:x})", motor_egl_aw::eglGetError());
      //--breaker-misc
      ghostBuster_();
    }
    else {
      spdlog::trace("eglCreateContext: Created context");
    }

    // --------------------------------
    motor_egl_aw::EGLint eglMakeCurrentResult;
    eglMakeCurrentResult = motor_egl_aw::eglMakeCurrent(m_eglDisplayObj, m_eglSurfaceObj, m_eglSurfaceObj, m_eglContextObj);
    if (eglMakeCurrentResult == EGL_FALSE || m_eglContextObj == nullptr) {
      spdlog::error("eglMakeCurrent: Failed to connect context to surface. eglGetError: {0:d} (0x{0:x})", motor_egl_aw::eglGetError());
      //--breaker-misc
      ghostBuster_();
    }
    else {
      spdlog::trace("eglMakeCurrent: Connected context to surface");
    }

    spdlog::trace("--------------------------------");
    spdlog::trace("eglQuerySurface: is EGL_RENDER_BUFFER using EGL_BACK_BUFFER?");
    motor_egl_aw::EGLint value;
    bool answered = motor_egl_aw::eglQuerySurface(m_eglDisplayObj, m_eglSurfaceObj, EGL_RENDER_BUFFER, &value);
    if (answered) {
      if (value == EGL_BACK_BUFFER) {
        spdlog::trace("eglQuerySurface: EGL_RENDER_BUFFER: {0:d} (0x{0:x}) - using baaaak bufaaa baby!", value);
      }
      else {
        spdlog::warn("eglQuerySurface: EGL_RENDER_BUFFER: {0:d} (0x{0:x}) - no back buffer :(", value);
      }
    } else {
      spdlog::warn("eglQuerySurface: could not query EGL_RENDER_BUFFER");
    }

    // --------------------------------
    // Print some info

    spdlog::debug("--------------------------------");
    spdlog::debug("EGL INFO");
    spdlog::debug("EGL version: {}.{}", eglVersionMajor, eglVersionMinor);
    spdlog::debug("--------------------------------");
    spdlog::debug("GL INFO");
    spdlog::debug("GL Version: {}", (char *)glGetString(GL_VERSION));
    spdlog::debug("GL GLSL version: {}", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
    spdlog::debug("GL Renderer: {}", (char *)glGetString(GL_RENDERER));
    spdlog::debug("GL Vendor: {}", (char *)glGetString(GL_VENDOR));
    spdlog::debug("GL Extensions: {}", (char *)glGetString(GL_EXTENSIONS));

    spdlog::debug("Display: EGL_CLIENT_APIS: {}", motor_egl_aw::eglQueryString(m_eglDisplayObj, EGL_CLIENT_APIS));
    spdlog::debug("Display: EGL_VENDOR: {}", motor_egl_aw::eglQueryString(m_eglDisplayObj, EGL_VENDOR));
    spdlog::debug("Display: EGL_VERSION: {}", motor_egl_aw::eglQueryString(m_eglDisplayObj, EGL_VERSION));
    spdlog::debug("Display: EGL_EXTENSIONS: {}", motor_egl_aw::eglQueryString(m_eglDisplayObj, EGL_EXTENSIONS));

    GLint glExtraData;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &glExtraData);
    spdlog::debug("GL_MAX_VERTEX_ATTRIBS Max no. of vertex attributes supported: {}", glExtraData);
    // it seems we have 16 on this allwinner, which is enough to do some job
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &glExtraData);
    spdlog::debug("GL_MAX_TEXTURE_IMAGE_UNITS Max no. of sim tex units supported: {}", glExtraData);
    // it seems we have 8 on this allwinner, fine

    // --------------------------------
    // Set swap interval
    motor_egl_aw::eglSwapInterval(m_eglDisplayObj, useMotorOptions.isVsync() ? 1 : 0);


    //--breaker-nocatch-end
    // --------------------------------

    // --------------------------------
    // Test VAO
    try {
      #if HELA_USE_GL_OES_vertex_array_object == 1
      #if HELA_USE_GL_OES_vertex_array_object_POLYFILL == 1
        {
          spdlog::trace("Will try to create, bind, check and destroy VAO testobject (within polyfill) ... ");
          GLuint VAOGLES2;
          glGenVertexArrays(1, &VAOGLES2);
          bool passDuring = glIsVertexArray(VAOGLES2);
          glBindVertexArray(VAOGLES2);
          glBindVertexArray(0);
          glDeleteVertexArrays(1, &VAOGLES2);
          bool passAfter = glIsVertexArray(VAOGLES2);
          if (passDuring && !passAfter) {
            spdlog::trace("OK!");
          }
          else {
            spdlog::warn("VAO testobject (within polyfill) failed");
          }
          spdlog::trace("--------------------------------");
        }
      #else
        spdlog::trace("Will try to create, bind, check and destroy VAO testobject (with OES as polyfill is not enabled) ... ");
        GLuint VAOGLES2;
        glGenVertexArraysOES(1, &VAOGLES2);
        bool passDuring = glIsVertexArrayOES(VAOGLES2);
        glBindVertexArrayOES(VAOGLES2);
        glBindVertexArrayOES(0);
        glDeleteVertexArraysOES(1, &VAOGLES2);
        bool passAfter = glIsVertexArrayOES(VAOGLES2);
        if (passDuring && !passAfter) {
          spdlog::trace("OK!");
        }
        else {
          spdlog::warn("VAO testobject (with EOS) failed");
        }
        spdlog::trace("--------------------------------");
      #endif
      #else
        spdlog::trace("No VAO set, will skip VAO testobject");
      #endif
    }
    catch (std::exception &e){
      //--breaker-catch-forward
      // let us set this forward as user code might not use vao and if it does, then logs will show the reason
      spdlog::error("VAO test generation exception, , VAOs will probably not wok. what: ", e.what());
    }
    catch (...) {
      //--breaker-catch-forward
      spdlog::error("VAO test generation exception, VAOs will probably not wok");
    }

    // --------------------------------
    try {

      if (m_nucleusSettings.isUserInputAllowed()) {
        helaIoThreadSpawn_();
      }
      else {
        spdlog::trace("RunnerLinuxFbdevMaliAllwinner no input asked. Setting simple echo blocking nevertheless.");
        runnerTermiosLock_();
      }

      // Clean out OpenGL errors that might happen due to some bugs so that they don't fall into usercode
      GLenum err;
      while ((err = glGetError()) != GL_NO_ERROR) {
        spdlog::error("Cleaning out OpenGL errors: {0:d} (0x{0:x})", err);
      }

      // User setup routine
      m_Nucleus->abstractSetup_();

      for (;;) {
        eglMakeCurrentResult = motor_egl_aw::eglMakeCurrent(m_eglDisplayObj, m_eglSurfaceObj, m_eglSurfaceObj, m_eglContextObj);

        if (m_flagShouldQuit) {
          break;
        }

        // Poll for and process events
        if (m_nucleusSettings.isUserInputAllowed()) {
          helaIoThreadPoll_();
        }

        // User update routine
        m_Nucleus->abstractUpdate_();

        // User draw routine
        m_Nucleus->abstractDraw_();

        // Swap buffers
        motor_egl_aw::eglSwapBuffers(m_eglDisplayObj, m_eglSurfaceObj);

      }

      // Join before cleanup, as cleanup may have long execution
      if (m_nucleusSettings.isUserInputAllowed()) {
        helaIoThreadJoin_();
      }
      else {
        spdlog::trace("RunnerLinuxFbdevMaliAllwinner unblocking echo");
        runnerTermiosUnlock_();
      }

      // User cleanup routine
      m_Nucleus->abstractCleanup_();

    }
    catch (std::exception &e){
      //--breaker-catch-forward
      // not entirely true as we have also i/O thread in scope
      spdlog::error("User code threw and it was was not caught earlier (setup() should rethrow intentionally on shader, texture etc. errors).");
      spdlog::error("User code exception: ", e.what());
    }
    catch (...) {
      //--breaker-catch-forward
      spdlog::error("User code threw.");
    }

    // --------------------------------
    ghostBuster_();

  }

  // ----------------------------------------------------------------------------
  void RunnerLinuxFbdevMaliAllwinner::ghostBuster_() {

    // --------------------------------
    //--breaker-nocatch-begin

    if (m_eglDisplayObj != EGL_NO_DISPLAY) {
      // Destroy things
      if (m_eglSurfaceObj != EGL_NO_SURFACE) {
        motor_egl_aw::eglDestroySurface(m_eglDisplayObj, m_eglSurfaceObj);
        m_eglSurfaceObj = EGL_NO_SURFACE;
      }
      if (m_eglContextObj != EGL_NO_CONTEXT) {
        motor_egl_aw::eglDestroyContext(m_eglDisplayObj, m_eglContextObj);
        m_eglContextObj = EGL_NO_CONTEXT;
      }

      // Make current nothing
      motor_egl_aw::eglMakeCurrent(m_eglDisplayObj, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

      // Release API (we did eglMakeCurrent to nothing already manually)
      motor_egl_aw::eglReleaseThread();

      // Terminate resources
      motor_egl_aw::eglTerminate(m_eglDisplayObj);
      m_eglDisplayObj = EGL_NO_DISPLAY;
    }

    if (m_eglWindowNative != nullptr) {
      // shared
    }

    //--breaker-nocatch-end
    // --------------------------------

  }

  // ----------------------------------------------------------------------------
  void RunnerLinuxFbdevMaliAllwinner::quit() {
    spdlog::trace("RunnerLinuxFbdevMaliAllwinner quit asked");
    m_flagShouldQuit = true;
  }

  // ----------------------------------------------------------------------------
  void RunnerLinuxFbdevMaliAllwinner::helaIoThreadSpawn_() {

    // promise our I/O thread that we will ask to stop someday and do it via future not atomic reference
    m_futureShouldQuit = m_promiseShouldQuit.get_future();
    m_thIo = std::thread(&RunnerLinuxFbdevMaliAllwinner::threadedIo_, this, std::move(m_futureShouldQuit));

  }

  // ----------------------------------------------------------------------------
  void RunnerLinuxFbdevMaliAllwinner::helaIoThreadJoin_() {

    // keep our promise
    m_promiseShouldQuit.set_value();
    if (m_thIo.joinable()) {
      m_thIo.join();
    }

  }

  // ----------------------------------------------------------------------------
  void RunnerLinuxFbdevMaliAllwinner::helaIoThreadPoll_() {

    // read out accumulator, pass to user code, clear

    // https://raw.githubusercontent.com/torvalds/linux/master/include/uapi/linux/input-event-codes.h
    // https://www.kernel.org/doc/Documentation/input/event-codes.txt
    // struct input_event {
    // struct timeval time;
    // __u16 type;
    // __u16 code;
    // __s32 value;
    // };
    //  struct timeval {
    //    __kernel_time_t    tv_sec;    /* seconds */
    //    __kernel_suseconds_t  tv_usec;  /* microseconds */
    //  };

    std::lock_guard<std::mutex> lockerEventsAccumulator(m_mutexIoMain);

    // TODO: yeah, we could also do this prep in the second thread
    // but then we would have to create new event struct to hold it & dispatch system
    // would take both channels, split RELs up to EV_SYN, for others assume EV_SYN is timeval as keys should not be grouped for user code
    // pack into HelaEvent struct, then sort by HelaEvent timeval, then dispatch
    // ain't got time for that

    for (auto & channel: m_inputEventsAccumulator) {

      hela::vec::vec2<int> mosePosAccumulator(0, 0);
      hela::vec::vec2<int> moseWheelAccumulator(0, 0);
      bool accumulatingMousePos = false;
      bool accumulatingMouseWheel = false;

      for (auto const& ev: channel) {

        // cursor position event or mouse wheel
        if (ev.type == EV_REL) {
          switch (ev.code) {
            case REL_X:
              accumulatingMousePos = true;
              mosePosAccumulator.x += ev.value;
              break;
            case REL_Y:
              accumulatingMousePos = true;
              mosePosAccumulator.y += ev.value;
              break;
            case REL_WHEEL:
              accumulatingMouseWheel = true;
              moseWheelAccumulator.y += ev.value;
              break;
            case REL_HWHEEL:
              accumulatingMouseWheel = true;
              moseWheelAccumulator.x += ev.value;
              break;
            default:
              break;
          }
        }

        // calculate rel accumulations
        if (ev.type == EV_SYN) {
          if (accumulatingMousePos) {
            m_Nucleus->eventMousePositionDelta(mosePosAccumulator.x, mosePosAccumulator.y);
            mosePosAccumulator = hela::vec::vec2<int>(0, 0);
            accumulatingMousePos = false;
          }
          if (accumulatingMouseWheel) {
            m_Nucleus->eventMouseWheel(moseWheelAccumulator.x, moseWheelAccumulator.y);
            moseWheelAccumulator = hela::vec::vec2<int>(0, 0);
            accumulatingMouseWheel = false;
          }
        }

        // keyboard key event
        // do not care about preceding EV_MSC
        // do not care about following EV_SYN (and if multiple EV_KEYs are grouped within one EV_SYN marker)
        // user should know about all of them, there is nothing to join
        if (ev.type == EV_KEY && ev.code < 0x100) {
          m_Nucleus->eventKeyboardKey(ev.code, ev.value);
        }
        // keyboard mouse button event
        // do not care about other markers here, just as for keyboard keys
        if (ev.type == EV_KEY && ev.code > 0xff) {
          m_Nucleus->eventMouseButton(ev.code, ev.value);
        }
      }
      channel.clear(); // do not change capacity
    }

  }

  // ----------------------------------------------------------------------------
  void RunnerLinuxFbdevMaliAllwinner::threadedIo_(std::future<void> futureQuitSignal) {
    // spdlog is thread safe oob
    spdlog::trace("RunnerLinuxFbdevMaliAllwinner threadedIo_ saying hi!");

    // --------------------------------

    int runnerLockExclusiveKeyboard = -1;
    int runnerLockExclusiveMouse = -1;

    auto openDevice = [](std::string envName, std::string fallbackPattern, int& fileDescriptor, int& lockExclusive)->bool {
      bool success = false;
      if (getenv(envName.c_str())) {
        fileDescriptor = open(getenv(envName.c_str()), O_RDONLY | O_NONBLOCK);
        if (fileDescriptor == -1) {
          spdlog::error("{0} environment value {1} specified, but cannot open. Will NOT try autodetect, if you want autodetect, don't specify {0}!", envName, getenv(envName.c_str()));
        }
        else {
          spdlog::debug("{} seems to be opened at handle specified by you {}", envName, getenv(envName.c_str()));
          success = true;
        }
      }
      else {
        // not using C++17 :(
        std::string deviceDirName = "/dev/input/by-id/";
        DIR *dirp = opendir(deviceDirName.c_str());
        if (dirp == NULL) {
          spdlog::warn("{} cannot open {} to run autodetect for device", envName, deviceDirName);
        }
        else {
          spdlog::debug("{} opened {} to search for device", envName, deviceDirName);
          struct dirent *dp;
          std::string deviceFile;
          do {
            dp = readdir(dirp);
            if (dp != NULL) {
              std::string currFile(dp->d_name);
              if (currFile.find(fallbackPattern) != std::string::npos) {
                deviceFile = deviceDirName + currFile;
                spdlog::debug("{} autodetect will try opening {}", envName, deviceFile);
                break;
              }
            }
          }
          while (dp != NULL);

          if (!deviceFile.empty()) {
            fileDescriptor = open(deviceFile.c_str(), O_RDONLY | O_NONBLOCK);
            if (fileDescriptor == -1) {
              spdlog::error("{} autodetect could not open {}", envName, deviceFile);
            }
            else {
              spdlog::debug("{} autodetect opened at handle {}", envName, deviceFile);
              success = true;
            }
          }

        }
        if (dirp != NULL) {
          closedir(dirp);
        }

      }

      if (success) {
#if defined(HELA_EXCLUSIVE_IOCTL) && HELA_EXCLUSIVE_IOCTL == 1
        lockExclusive = ioctl(fileDescriptor, EVIOCGRAB, 1);
        if (lockExclusive != 0) {
          spdlog::warn("{} could not gain exclusive access", envName);
        } else {
          spdlog::trace("{} could gain exclusive access", envName);
        }
#else
        lockExclusive = -1;
#endif
      }

      return success;
    };

    // --------------------------------

    std::unique_lock<std::mutex> lockerStreamOpen(m_mutexIoMain);

    if (std::this_thread::get_id() == m_runnerThreadId) {
      spdlog::error("You have put keyboard and mouse readouts in same thread as main thread! Let's push those SoC's multicores (unless context switching on this SoC makes things worse ^_^)!");
    }

    m_flagHasKeyboard = openDevice("GL_KEYBOARD", "event-kbd", m_fileDescriptorKeyboard, runnerLockExclusiveKeyboard);
    m_flagHasMouse = openDevice("GL_MOUSE", "event-mouse", m_fileDescriptorMouse, runnerLockExclusiveMouse);

    if (runnerLockExclusiveKeyboard == 0) {
      //
    } else {
      spdlog::warn("GL_KEYBOARD was not set to gain exclusive access, we will fallback to termios echo blocking");
      runnerTermiosLock_();
    }

    m_ioOpenStageEnded = true;
    lockerStreamOpen.unlock();
    m_condIoOpener.notify_one();

    // --------------------------------

    const auto futurePromiseTime = std::chrono::milliseconds(1);
    const auto sleepTime = std::chrono::milliseconds(20);
    // don't lock just because of m_flagHas*
    while (futureQuitSignal.wait_for(futurePromiseTime) == std::future_status::timeout) {
      // don't read into sized (some kind of "struct input_event m_bufferMultisize[HARDLIMIT]") buffer
      // just one input_event after another until OS side is drained
      // offload immidately to our inmemory accumulator
      // it is our inmemory accumulator that handles "overflows" (we actually don't care that much)
      // goal: be nice towards OS and read out everything so that we don't hit SYN_DROPPED
      // while EOL or EAGAIN (this is non-blocking) not hit, drain
      
      if (m_flagHasKeyboard) {
        ssize_t numBRead = read(m_fileDescriptorKeyboard, &m_bufferSinglesizeInputEventKeyboard, sizeof(struct input_event));
        while (numBRead > -1) {
          helaIoEventsAccumulate_(&m_bufferSinglesizeInputEventKeyboard, 0);
          numBRead = read(m_fileDescriptorKeyboard, &m_bufferSinglesizeInputEventKeyboard, sizeof(struct input_event));
        }
        if (numBRead == -1) {
          if (errno == EINTR) {
            spdlog::trace("GL_KEYBOARD EINTR SIGNAL");
          }
          else if (errno != EAGAIN) {
            spdlog::warn("GL_KEYBOARD read errno {}", errno);
          }
        }
      }
      if (m_flagHasMouse) {
        ssize_t numBRead = read(m_fileDescriptorMouse, &m_bufferSinglesizeInputEventMouse, sizeof(struct input_event));
        while (numBRead > -1) {
          helaIoEventsAccumulate_(&m_bufferSinglesizeInputEventMouse, 1);
          numBRead = read(m_fileDescriptorMouse, &m_bufferSinglesizeInputEventMouse, sizeof(struct input_event));
        }
        if (numBRead == -1) {
          if (errno == EINTR) {
            spdlog::trace("GL_MOUSE EINTR SIGNAL");
          }
          else if (errno != EAGAIN) {
            spdlog::warn("GL_KEYBOARD read errno {}", errno);
          }
        }
      }

      std::this_thread::sleep_for(sleepTime);
    }

    // --------------------------------
    if (m_flagHasKeyboard) {
      if (runnerLockExclusiveKeyboard == 0) {
#if defined(HELA_EXCLUSIVE_IOCTL) && HELA_EXCLUSIVE_IOCTL == 1
        runnerLockExclusiveKeyboard = ioctl(m_fileDescriptorKeyboard, EVIOCGRAB, 0);
        if (runnerLockExclusiveKeyboard != 0) {
          spdlog::warn("GL_KEYBOARD could not release exclusive access");
        } else {
          spdlog::trace("GL_KEYBOARD released exclusive access");
        }
#endif
      } else {
        spdlog::warn("GL_KEYBOARD restoring echoing");
        runnerTermiosUnlock_();
      }
      close(m_fileDescriptorKeyboard);
    }
    if (m_flagHasMouse) {
      if (runnerLockExclusiveMouse == 0) {
#if defined(HELA_EXCLUSIVE_IOCTL) && HELA_EXCLUSIVE_IOCTL == 1
        runnerLockExclusiveMouse = ioctl(m_fileDescriptorMouse, EVIOCGRAB, 0);
        if (runnerLockExclusiveMouse != 0) {
          spdlog::warn("GL_MOUSE could not release exclusive access");
        } else {
          spdlog::trace("GL_MOUSE released exclusive access");
        }
#endif
      }
      close(m_fileDescriptorMouse);
    }

    // --------------------------------
    spdlog::trace("RunnerLinuxFbdevMaliAllwinner threadedIo_ saying bye!");
  }

  // ----------------------------------------------------------------------------
  void RunnerLinuxFbdevMaliAllwinner::helaIoEventsAccumulate_(struct input_event *event, std::size_t streamIdx) {

    std::lock_guard<std::mutex> lockerEventsAccumulator(m_mutexIoMain);

    while ((streamIdx + 1) > m_inputEventsAccumulator.size()) {
      // expand accumulator till the new stream index is reached, set capacity to avoid reallocations
      m_inputEventsAccumulator.push_back(std::deque<struct input_event>(m_accumulatorSizePreferred));
      if (m_accumulatorSizePreferred < m_inputEventsAccumulator.back().size()) {
        m_accumulatorSizePreferred = m_inputEventsAccumulator.back().size();
      }
      spdlog::trace("m_inputEventsAccumulator expanding for {} and got reserved size {}", streamIdx, m_accumulatorSizePreferred);
    }
    struct input_event ev = *event;
    // TODO: use ring buffer
    // if user has not polled events then our vector has grown
    // not polling might occure because the coders has made graphics that result in one frame per multiple seconds ^_^
    // in this case we "overflow" our m_inputEventsAccumulator
    // push out oldest messages, so that possible "ESC" banging eventually gets to the usercode
    while (m_inputEventsAccumulator.at(streamIdx).size() > (m_accumulatorSizePreferred - 1)) {
      spdlog::trace("you are not polling events quick enough, overflow! deleting older events, adding newer events.");
      m_inputEventsAccumulator.at(streamIdx).pop_front();
    }
    m_inputEventsAccumulator.at(streamIdx).push_back(ev);
  }

  // ----------------------------------------------------------------------------
  void RunnerLinuxFbdevMaliAllwinner::runnerTermiosLock_() {
    spdlog::trace("RunnerLinuxFbdevMaliAllwinner runnerTermiosLock_");
    // Try disabling WASD and other keystrokes going to terminal while running the app
    // This does not hide cursor though, but let us do that in a shell script wrapper (tput civis) as shells differ
    tcgetattr(STDIN_FILENO, &m_runnerTermiosSaver);
    memcpy(&m_runnerTermiosCatcher, &m_runnerTermiosSaver, sizeof(m_runnerTermiosCatcher));
    // instead of chasing down multiple ECHO* flags to be turned off, easier to unset canonical mode and unset ECHO that will chain all OOB
    m_runnerTermiosCatcher.c_lflag &= ~(ICANON|ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &m_runnerTermiosCatcher);
  }

  // ----------------------------------------------------------------------------
  void RunnerLinuxFbdevMaliAllwinner::runnerTermiosUnlock_() {
    spdlog::trace("RunnerLinuxFbdevMaliAllwinner runnerTermiosUnlock_");
    tcsetattr(STDIN_FILENO, TCSANOW, &m_runnerTermiosSaver);
  }

  // ----------------------------------------------------------------------------
  bool RunnerLinuxFbdevMaliAllwinner::runnerHasKeyboard() const {
    // if user cares about state of input availability, main thread will have to wait
    // othervise main thread goes on on doing things and input starts flowing some time in the future
    if (m_nucleusSettings.isUserInputAllowed()) {
      std::unique_lock<std::mutex> lockerWaitOpenCond(m_mutexIoStateGetter);
      m_condIoOpener.wait(lockerWaitOpenCond, [this]{ return m_ioOpenStageEnded; });
    }
    return m_flagHasKeyboard;
  }

  // ----------------------------------------------------------------------------
  bool RunnerLinuxFbdevMaliAllwinner::runnerHasMouse() const {
    // if user cares about state of input availability, main thread will have to wait
    // othervise main thread goes on on doing things and input starts flowing some time in the future
    if (m_nucleusSettings.isUserInputAllowed()) {
      std::unique_lock<std::mutex> lockerWaitOpenCond(m_mutexIoStateGetter);
      m_condIoOpener.wait(lockerWaitOpenCond, [this]{ return m_ioOpenStageEnded; });
    }
    return m_flagHasMouse;
  }

} }
