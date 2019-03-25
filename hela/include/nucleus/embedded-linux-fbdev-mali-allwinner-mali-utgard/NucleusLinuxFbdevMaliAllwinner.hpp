#ifndef NucleusLinuxFbdevMaliAllwinner_hpp
#define NucleusLinuxFbdevMaliAllwinner_hpp

#include "nucleus/NucleusAbstract.hpp"

#if !defined(HELA_POSIX)
  #error "Hela uses POSIX stuff here, which you are not"
#endif

namespace hela { namespace nucleus {

  class RunnerLinuxFbdevMaliAllwinner;

  class NucleusLinuxFbdevMaliAllwinner : public NucleusAbstract {
  public:

    NucleusLinuxFbdevMaliAllwinner();
    virtual ~NucleusLinuxFbdevMaliAllwinner();

    void quit() override;

    bool eventHasKeyboard(bool /*fakeReturnValue*/ = true) const override;
    bool eventHasMouse(bool /*fakeReturnValue*/ = true) const override;

    template<typename NucleusT>
    static void main(
                     int argc,
                     char const * const argv[],
                     const NucleusSettingsFn &settingsFn = NucleusSettingsFn(),
                     const hela::motor::Motor::MotorSettingsFn &motorMotorSettingsFn = hela::motor::Motor::MotorSettingsFn()
                   );

  protected:

    void  launch() override;

  private:
    
    std::shared_ptr<RunnerLinuxFbdevMaliAllwinner> m_runner = nullptr;

  }; // NucleusLinuxFbdevMaliAllwinner

  template<typename NucleusT>
  void NucleusLinuxFbdevMaliAllwinner::main(
                                            int argc,
                                            char const * const argv[],
                                            const NucleusSettingsFn &nucleusSettingsFn,
                                            const hela::motor::Motor::MotorSettingsFn &motorMotorSettingsFn
                                          ) {
    try {
      NucleusLinuxFbdevMaliAllwinner::prepareLaunch();
      {
        NucleusSettings nucleusSettings;
        if (nucleusSettingsFn) {
          nucleusSettingsFn(&nucleusSettings);
        }
        hela::motor::Motor::MotorSettings motorSettings;
        if (motorMotorSettingsFn) {
          motorMotorSettingsFn(&motorSettings);
        }
        hela::motor::Motor_S motor = std::make_shared<hela::motor::Motor>(motorSettings);
        NucleusLinuxFbdevMaliAllwinner::initialize(argc, argv, &nucleusSettings, motor);
        {
          std::shared_ptr<NucleusLinuxFbdevMaliAllwinner> nucleus = std::make_shared<NucleusT>();
          nucleus->executeLaunch();
        }
      }
      NucleusLinuxFbdevMaliAllwinner::cleanupLaunch();
    }
    catch (std::exception &e){
      spdlog::error("Main got excepton: ", e.what());
    }
    catch (...) {
      spdlog::error("Main got excepton");
    }

  } // main

} }

#endif // NucleusLinuxFbdevMaliAllwinner_hpp
