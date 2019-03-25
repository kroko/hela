#include "nucleus/desktop-genericos/NucleusGenericOs.hpp"
#include "nucleus/desktop-genericos/RunnerGenericOs.hpp"

namespace hela { namespace nucleus {

  // ----------------------------------------------------------------------------
  NucleusGenericOs::NucleusGenericOs()
  : NucleusAbstract() {
    spdlog::trace("NucleusGenericOs construct");
    // pass also motor settings directly?
    m_runner = std::make_shared<RunnerGenericOs>(this, *dynamic_cast<NucleusSettings *>(s_nucleusSettingsFromMain));
  }

  // ----------------------------------------------------------------------------
  NucleusGenericOs::~NucleusGenericOs() {
    spdlog::trace("NucleusGenericOs destruct");
  }

  // ----------------------------------------------------------------------------
  void NucleusGenericOs::launch() {
    spdlog::trace("NucleusGenericOs launch");
    m_runner->run();
  }

  // ----------------------------------------------------------------------------
  void NucleusGenericOs::quit() {
    spdlog::trace("NucleusGenericOs quit");
    m_runner->quit();
  }

  // ----------------------------------------------------------------------------
  void NucleusGenericOs::setCaptureCursor(bool capture) {
    m_runner->setCaptureCursor(capture);
  }

  // ----------------------------------------------------------------------------
  bool NucleusGenericOs::isCaptureCursor() const {
    return m_runner->isCaptureCursor();
  }

  // ----------------------------------------------------------------------------
  void NucleusGenericOs::setFullScreen(bool fullScreen) {
    m_runner->setFullScreen(fullScreen);
  }

  // ----------------------------------------------------------------------------
  bool NucleusGenericOs::isFullScreen() const {
    return m_runner->isFullScreen();
  }

} }
