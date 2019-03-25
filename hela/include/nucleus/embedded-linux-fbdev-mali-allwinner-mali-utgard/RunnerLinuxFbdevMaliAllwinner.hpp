#ifndef RunnerLinuxFbdevMaliAllwinner_hpp
#define RunnerLinuxFbdevMaliAllwinner_hpp

#include "nucleus/embedded-linux-fbdev-mali-allwinner-mali-utgard/NucleusLinuxFbdevMaliAllwinner.hpp"

#if !defined(HELA_POSIX)
  #error "Hela uses POSIX stuff here, which you are not"
#endif

// Implementation / driver / window
#define EGL_EGLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>
#define motor_egl_aw // make fake namespace-like marker for Allwinner's EGL to differentiate global stuff easier

// OpenGL
#include "nucleus/MotorStrap.hpp"

// STL
#include <deque>

// I’m your density
#include <future>

#ifdef __cplusplus
extern "C" {
#endif

// Try blocking keyboard output to terminal while smashing buttons if no exclusive
#include <termios.h>

// For events
#include <linux/input.h>  // for input_event struct

#include <fcntl.h>  // for open
#include <unistd.h> // for read
#include <dirent.h> // for dir stuff
#include <errno.h>  // for errno https://github.com/torvalds/linux/blob/master/include/uapi/asm-generic/errno-base.h

#ifdef __cplusplus
}
#endif

// turning this on is too excessive
#define HELA_EXCLUSIVE_IOCTL 0

namespace hela { namespace nucleus {
  
  class NucleusLinuxFbdevMaliAllwinner;
  
  class RunnerLinuxFbdevMaliAllwinner {
    
    //: friends
    friend class NucleusLinuxFbdevMaliAllwinner;
    
  public:
    
    RunnerLinuxFbdevMaliAllwinner(NucleusLinuxFbdevMaliAllwinner *aNucleus, const NucleusLinuxFbdevMaliAllwinner::NucleusSettings &settings);
    virtual ~RunnerLinuxFbdevMaliAllwinner();
    
  public:
    
    const NucleusLinuxFbdevMaliAllwinner* getNucleus() const { return m_Nucleus; }
    bool runnerHasKeyboard() const;
    bool runnerHasMouse() const;
    
  protected:
    
    void quit();

  private:
    
    void run();
    void ghostBuster_();
    NucleusLinuxFbdevMaliAllwinner* m_Nucleus = nullptr;
    NucleusLinuxFbdevMaliAllwinner::NucleusSettings m_nucleusSettings;
    bool m_flagShouldQuit = false;
    
  private:
    
    motor_egl_aw::EGLDisplay m_eglDisplayObj;
    motor_egl_aw::EGLSurface m_eglSurfaceObj;
    motor_egl_aw::EGLContext m_eglContextObj;
    // fbdev_window is actually EGLNativeWindowType though chain of typedefs
    std::shared_ptr<motor_egl_aw::fbdev_window> m_eglWindowNative;
    
  private:

    std::thread::id m_runnerThreadId;
    std::promise<void> m_promiseShouldQuit;
    std::future<void> m_futureShouldQuit;
    std::thread m_thIo;
    std::mutex m_mutexIoMain;
    mutable std::mutex m_mutexIoStateGetter;
    mutable std::condition_variable m_condIoOpener;
    bool m_ioOpenStageEnded = false;
    void threadedIo_(std::future<void> futureQuitSignal);
    
  private:
    
    void runnerTermiosLock_();
    void runnerTermiosUnlock_();
    struct termios m_runnerTermiosCatcher;
    struct termios m_runnerTermiosSaver;
    
  private:
    
    bool m_flagHasKeyboard = false;
    bool m_flagHasMouse = false;
    
    int m_fileDescriptorKeyboard = -1;
    int m_fileDescriptorMouse = -1;
    
    struct input_event m_bufferSinglesizeInputEventKeyboard;
    struct input_event m_bufferSinglesizeInputEventMouse;
    
    // 1) all input events read are pushed in this vector
    // 2) we do n "dimensions", because we have n streams
    // 3) multiple dimentions are actually not needed here needed as we could join them in 1 dimension
    // because we read all event streams sequentially and on one thread
    // 4) of course sequential read is not precise for combined state, for that we should do timeval based rearrangement
    // 5) thus we simplyfy combined state and just put keyboard events in front (but only seemingly) of mouse events
    // 6) we care for EV_SYN event types in order to join relative events before sending to user code as it is expected
    // that each mouse event will result in some kind of expensive linear algebra.
    // if applicable user can accumulate even more events (say, per frame)
    // 7) simply assume that if type is EV_KEY then if code is smaller than 256 is potentioal keyboard keys, everything above is potential mouse buttons
    std::size_t m_accumulatorSizePreferred = 32;
    std::vector<std::deque<struct input_event> > m_inputEventsAccumulator;
    
    void helaIoEventsAccumulate_(struct input_event *event, std::size_t streamIdx);
    void helaIoThreadSpawn_();
    void helaIoThreadPoll_();
    void helaIoThreadJoin_();
    
  }; // RunnerLinuxFbdevMaliAllwinner
  
} }

#endif // RunnerLinuxFbdevMaliAllwinner_hpp
