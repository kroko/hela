#ifndef RunnerGenericOs_hpp
#define RunnerGenericOs_hpp

#include "nucleus/desktop-genericos/NucleusGenericOs.hpp"

// Implementation / driver / window
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#define motor_glfw // make fake namespace-like marker for GLFW to differentiate global stuff easier

// OpenGL
#include "nucleus/MotorStrap.hpp"

namespace hela { namespace nucleus {

  class NucleusGenericOs;

  class RunnerGenericOs {

    //: friends
    friend class NucleusGenericOs;

  public:

    RunnerGenericOs(NucleusGenericOs *aNucleus, const NucleusGenericOs::NucleusSettings &settings);
    virtual ~RunnerGenericOs();

  public:

    const NucleusGenericOs* getNucleus() const { return m_Nucleus; }
    void setCaptureCursor(bool capture = true);
    bool isCaptureCursor() const;
    void setFullScreen(bool fullScreen = true);
    bool isFullScreen() const;

  protected:

    void quit();

  private:

    void run();
    void ghostBuster_();
    NucleusGenericOs* m_Nucleus;
    NucleusGenericOs::NucleusSettings m_nucleusSettings;
    bool m_flagShouldQuit = false;

  private:

    // is it window, is it context? shaaaapeshifter!
    motor_glfw::GLFWwindow* m_glfwWindowObj = nullptr;

  private:

    // Use int type so we can pass in references
    hela::vec::vec2<int> m_lastWindowedSize;
    hela::vec::vec2<int> m_lastWindowedPosition;

  }; // RunnerGenericOs

} }

#endif // RunnerGenericOs_h
