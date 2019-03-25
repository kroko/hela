#include "nucleus/embedded-linux-fbdev-mali-allwinner-mali-utgard/NucleusLinuxFbdevMaliAllwinner.hpp"
#include "nucleus/embedded-linux-fbdev-mali-allwinner-mali-utgard/RunnerLinuxFbdevMaliAllwinner.hpp"

namespace hela { namespace nucleus {

  // ----------------------------------------------------------------------------
  NucleusLinuxFbdevMaliAllwinner::NucleusLinuxFbdevMaliAllwinner()
  : NucleusAbstract() {
    spdlog::trace("NucleusLinuxFbdevMaliAllwinner construct");
    // pass also motor settings directly?
    m_runner = std::make_shared<RunnerLinuxFbdevMaliAllwinner>(this, *dynamic_cast<NucleusSettings *>(s_nucleusSettingsFromMain));
  }

  // ----------------------------------------------------------------------------
  NucleusLinuxFbdevMaliAllwinner::~NucleusLinuxFbdevMaliAllwinner() {
    spdlog::trace("NucleusLinuxFbdevMaliAllwinner destruct");
  }

  // ----------------------------------------------------------------------------
  void NucleusLinuxFbdevMaliAllwinner::launch() {
    spdlog::trace("NucleusLinuxFbdevMaliAllwinner launch");
    m_runner->run();
  }

  // ----------------------------------------------------------------------------
  void NucleusLinuxFbdevMaliAllwinner::quit() {
    spdlog::trace("NucleusLinuxFbdevMaliAllwinner quit");
    m_runner->quit();
  }

  // ----------------------------------------------------------------------------
  bool NucleusLinuxFbdevMaliAllwinner::eventHasKeyboard(bool /*fakeReturnValue*/) const {
    return m_runner->runnerHasKeyboard();
  }

  // ----------------------------------------------------------------------------
  bool NucleusLinuxFbdevMaliAllwinner::eventHasMouse(bool /*fakeReturnValue*/) const {
    return m_runner->runnerHasMouse();
  };

} }
